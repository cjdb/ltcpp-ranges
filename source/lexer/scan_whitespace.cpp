//
//  Copyright Christopher Di Bella
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include <cjdb/cctype/is_newline.hpp>
#include <cjdb/cctype/is_printable.hpp>
#include <cjdb/cctype/isspace.hpp>
#include <cjdb/functional/rangecmp/partial_not_equal_to.hpp>
#include <istream>
#include "ltcpp/consume_istream_while.hpp"
#include "ltcpp/lexer/detail/scan_whitespace.hpp"
#include "ltcpp/irregular_transform.hpp"
#include "ltcpp/source_coordinate.hpp"
#include <range/v3/action/push_back.hpp>
#include <range/v3/algorithm/adjacent_find.hpp>
#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/generate.hpp>
#include <range/v3/view/take_while.hpp>
#include <range/v3/view/reverse.hpp>
#include <string_view>
#include "tl/expected.hpp"

namespace ltcpp::detail_lexer {
   using namespace ranges;
   using namespace std::string_literals;

   enum class unexpected_whitespace { not_comment, unterminated_comment };
   using expected = tl::expected<source_coordinate, unexpected_whitespace>;

   /// \brief Given a string of UTF-8 coded characters, calculates the delta that a
   ///        source_coordinate moves by.
   ///
   /// The source_coordinate line delta is increased by one for every occurrence of "\n" or "\f";
   /// the source_coordinate column delta is increased by one for every printable character.
   /// \param code_points A string of UTF-8 code points.
   /// \returns A source_coordinate object containing the computed delta.
   /// \note This function has linear complexity.
   /// \note The algorithm used to find line separators is non-greedy.
   ///
   expected cursor_delta(std::string_view const code_points) noexcept
   {
      auto const reversed_code_points = code_points | view::reverse;
      auto const last_line = iterator_range{
         adjacent_find(reversed_code_points, cjdb::is_newline).base(),
         end(code_points)
      };
      return expected{
         source_coordinate{
            source_coordinate::column_type{distance(last_line)},
            source_coordinate::line_type{count_if(code_points, cjdb::is_newline)}
         }
      };
   }

   /// \brief Reads in all characters until the first non-whitespace character is encountered.
   ///
   /// Subsequent characters are not read by the stream.
   /// \param in The stream to be read from.
   /// \returns A source_coordinate delta indicating how many whitespace characters were read.
   /// \note This function has linear complexity.
   ///
   expected scan_whitespace(std::istream& in, char space) noexcept
   {
      auto whitespace = consume_istream_while(in, cjdb::isspace);
      return ::ltcpp::detail_lexer::cursor_delta(space + whitespace);
   }

   /// \brief Reads in all characters until the first newline character character is encountered.
   ///
   /// Subsequent characters are not read by the stream.
   /// \param in The stream to be read from.
   /// \returns A source_coordinate delta indicating how many characters were read.
   /// \note This function has linear complexity.
   ///
   expected skip_single_line_comment(std::istream& in) noexcept
   {
      auto comment = consume_istream_while(in, std::not_fn(cjdb::is_newline));
      return ::ltcpp::detail_lexer::cursor_delta("//" + comment);
   }

   /// \brief Reads in all characters until the first occurrence of the character sequence "*/".
   ///
   /// Subsequent characters are not read by the stream.
   /// \param in The stream to be read from.
   /// \returns A source_coordinate delta indicating how many characters were read.
   ///
   expected skip_multiline_comment(std::istream& in) noexcept
   {
      auto comment = "/*"s;
      do {
         action::push_back(
            comment,
            istream_range<char>(in) | view::take_while(cjdb::ranges::partial_not_equal_to{'*'})
         );
         comment += in.good() ? "*" : "";
      } while (in and in.peek() != '/');

      if (in) {
         // take '/' from the stream or else it won't be consumed!
         comment += static_cast<char>(in.get());
      }

      if (in) {
         return ::ltcpp::detail_lexer::cursor_delta(comment);
      }
      else {
         return tl::unexpected{unexpected_whitespace::unterminated_comment};
      }
   }

   /// \brief Reads one single-line or multi-line comment.
   /// \param in
   /// \returns A source_coordinate object that contains the size of the comment if a comment is
   ///          present; std::nullopt otherwise.
   ///
   expected skip_comment(std::istream& in) noexcept
   {
      if (auto c = '\0'; in.get(c) and c == '/') {
         return ::ltcpp::detail_lexer::skip_single_line_comment(in);
      }
      else if (c == '*') {
         return ::ltcpp::detail_lexer::skip_multiline_comment(in);
      }
      else { // this '/' wasn't the start of a comment
         in.unget();
         return tl::unexpected{unexpected_whitespace::not_comment};
      }
   }

   /// \brief Reads all whitespace and comments until a non-whitespace, non-comment character is
   ///        encountered.
   ///
   /// No subsequent characters are read.
   ///
   std::pair<source_coordinate, bool>
   scan_whitespace_like(std::istream& in, source_coordinate cursor) noexcept
   {
      // Deduces if a single character is whitespace or a forward slash.
      auto is_whitespaceish = [](auto const c) noexcept {
         return cjdb::isspace(c) or c == '/';
      };

      // Dispatches to the comment scanner if the character is a forward slash and the whitespace
      // scanner otherwise.
      auto read_whitespaceish = [&in](auto const c) noexcept {
         return (c == '/') ? ::ltcpp::detail_lexer::skip_comment(in)
                           : ::ltcpp::detail_lexer::scan_whitespace(in, c);
      };

      // Deduces if the whitespaceish character was actually just a forward slash.
      auto whitespace_confirmed = [](auto&& x) noexcept {
         return x or x.error() == unexpected_whitespace::unterminated_comment;
      };

      // yucky state here
      bool unterminated_comment = false;

      // Transforms an expected object into a coordinate object.
      // returns *coordinate if the value has been set; a source_coordinate equivalent to {0:0}
      // otherwise.
      // If the expected object's value has not been set, then the unterminated_comment flag is
      // raised to indicate that we've reached an error state.
      auto unwrap_coordinate = [&unterminated_comment](auto const coordinate) noexcept {
         if (coordinate) {
            return *coordinate;
         }
         else {
            unterminated_comment = true;
            return source_coordinate{
               source_coordinate::column_type{0},
               source_coordinate::line_type{0}
            };
         }
      };

      auto whitespace_distance = istream_range<char>(in)
                               | view::take_while(is_whitespaceish)
                               | ltcpp::irregular_transform(read_whitespaceish)
                               | view::take_while(whitespace_confirmed)
                               | ltcpp::irregular_transform(unwrap_coordinate);
      cursor = accumulate(whitespace_distance, cursor, &source_coordinate::shift);

      if (in) {
         in.unget(); // take_while consumes an extra character
      }

      return {cursor, unterminated_comment};
   }
} // namespace ltcpp::detail_lexer
