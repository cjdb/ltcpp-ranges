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
#include <algorithm>
#include <cassert>
#include <cjdb/cctype/is_newline.hpp>
#include <cjdb/cctype/is_printable.hpp>
#include <cjdb/cctype/isspace.hpp>
#include <cjdb/functional/rangecmp/partial_not_equal_to.hpp>
#include <istream>
#include "ltcpp/consume_istream_while.hpp"
#include "ltcpp/lexer/detail/scan_whitespace.hpp"
#include "ltcpp/source_coordinate.hpp"
#include <string_view>
#include "tl/expected.hpp"
#include <variant>

namespace ltcpp::detail_lexer {
   using namespace std::string_literals;

   struct not_comment {};

   using whitespace_error = std::variant<not_comment, unterminated_comment_error>;
   using expected = tl::expected<source_coordinate, whitespace_error>;

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
      auto const last_line_begin = std::adjacent_find(rbegin(code_points), rend(code_points),
         cjdb::is_newline).base();
      auto const last_line_end = end(code_points);

      return source_coordinate{
         source_coordinate::line_type{
            std::count_if(begin(code_points), end(code_points), cjdb::is_newline)
         },
         source_coordinate::column_type{std::distance(last_line_begin, last_line_end)}
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
         for (auto c = '\0'; in.get(c) and c != '*';) {
            comment += c;
         }
         comment += in.good() ? "*" : "";
      } while (in and in.peek() != '/');

      if (in) {
         // take '/' from the stream or else it won't be consumed!
         comment += static_cast<char>(in.get());
      }

      auto const cursor = ::ltcpp::detail_lexer::cursor_delta(comment);

      if (in) {
         return cursor;
      }
      else {
         return tl::unexpected{
            unterminated_comment_error{
               source_coordinate{},
               *cursor
            }
         };
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
         return tl::unexpected{not_comment{}};
      }
   }

   /// \brief Reads all whitespace and comments until a non-whitespace, non-comment character is
   ///        encountered.
   ///
   /// No subsequent characters are read.
   ///
   tl::expected<source_coordinate, unterminated_comment_error>
   scan_whitespace_like(std::istream& in, source_coordinate cursor) noexcept
   {
      auto unterminated_comment = std::optional<unterminated_comment_error>{};
      for (auto c = '\0'; in.get(c) and (cjdb::isspace(c) or c == '/');) {
         if (c == '/') {
            auto result = ::ltcpp::detail_lexer::skip_comment(in);
            if (result) {
               cursor = source_coordinate::shift(cursor, *result);
            }
            else if (std::holds_alternative<unterminated_comment_error>(result.error())) {
               unterminated_comment = std::get<unterminated_comment_error>(result.error());
            }
            else {
               break; // this wasn't a comment, let's get out of here
            }
         }
         else if (auto result = ::ltcpp::detail_lexer::scan_whitespace(in, c); result) {
            // better to check now, when it's always safe, than to deref and have the code become
            // undefined one day later on.
            cursor = source_coordinate::shift(cursor, *result);
         }
         else {
            assert(false); // logic error, I guess?
         }
      }

      if (in) {
         in.unget(); // take_while consumes an extra character
      }
      else if (unterminated_comment) {
         unterminated_comment->begin = cursor;
         unterminated_comment->end = source_coordinate::shift(cursor, unterminated_comment->end);
         return tl::unexpected{*unterminated_comment};
      }

      return cursor;
   }
} // namespace ltcpp::detail_lexer
