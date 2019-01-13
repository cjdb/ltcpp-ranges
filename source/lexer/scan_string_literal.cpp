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
#include "ltcpp/lexer/token.hpp"

#include <cjdb/cctype/is_newline.hpp>
#include <gsl/gsl>
#include <istream>
#include "ltcpp/consume_istream_while.hpp"
#include "ltcpp/lexer/detail/scan_string_literal.hpp"
#include <range/v3/action/adjacent_remove_if.hpp>
#include <range/v3/algorithm/count.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/drop_while.hpp>
#include <range/v3/view/slice.hpp>
#include <range/v3/view/split.hpp>
#include <string>
#include <string_view>

namespace {
   using namespace ranges;
   using namespace std::string_view_literals;

   constexpr auto legal_escapes = R"(bfnrt\'")"sv;

   /// \brief A predicate that determines if two elements in a string are an escape sequence.
   ///
   /// This predicate is used to split on escape sequences.
   /// \param first Iterator to the beginning of the range.
   /// \param last Sentinel denoting the end of the range.
   /// \returns A paired boolean and iterator to the backslash.
   /// \note The return type is prescribed by split_view.
   ///
   auto is_escape = [](auto first, auto last) constexpr noexcept {
      auto result = false;
      if (auto next = ranges::next(first); next != last) {
         result = *first == '\\' and legal_escapes.find(*next) != std::string_view::npos;
      }

      return std::make_pair(result, first);
   };

   /// \brief Checks each escape sequence is a legal escape sequence.
   /// \returns `token_kind::string_literal` if all escape sequences are legal;
   ///          `token_kind::ivalid_escape_sequence` otherwise.
   ///
   ltcpp::token_kind validate_escapes(std::string_view const lexeme) noexcept
   {
      auto no_illegal_escapes = [](auto&& x) constexpr noexcept {
         // the first two characters need to be checked separately to the rest of the literal,
         // because we get a false positive otherwise ("\\" has two backslashes, which immediately
         // disqualifies it, if we were to bundle the first two characters together).
         if (distance(x) >= 2 and x[0] == '\\' and legal_escapes.find(x[1]) == std::string_view::npos) {
            return false;
         }
         else {
            return count(x | view::drop(2), '\\') == 0;
         }
      };

      auto invalid_escapes = lexeme
                           | view::split(is_escape)
                           | view::drop_while(no_illegal_escapes);

      return empty(invalid_escapes) ? ltcpp::token_kind::string_literal
                                    : ltcpp::token_kind::invalid_escape_sequence;
   }

   /// \brief Checks that the string literal is terminated by a close-quote and that all escape
   ///        sequences are legal sequences.
   /// \returns token_kind::string_literal if the string is well-formed;
   ///          token_kind::unterminated_string_literal if it doesn't end with a close-quote;
   ///          token_kind::invalid_escape_sequence otherwise.
   ltcpp::token_kind validate_string_literal(std::string_view const lexeme) noexcept
   {
      // TODO: replace ends_with(lexeme, r2) with lexeme.ends_with(r2) when GCC 9 is released.
      auto ends_with = [](auto const& r1, auto const& r2) {
         return equal(r1 | view::drop(size(r1) - size(r2)), r2);
      };
      using namespace std::string_view_literals;
      if (not ends_with(lexeme, "\""sv) or ends_with(lexeme, R"(\")"sv)) {
         return ltcpp::token_kind::unterminated_string_literal;
      }

      // substr removes the enclosing quotes, which create noise.
      return validate_escapes(lexeme.substr(1, size(lexeme) - 1));
   }

   /// \brief Erases the backslash preceding a checked escape sequence.
   ///
   void erase_backslashes(std::string& string_literal) noexcept
   {
      constexpr auto legal_escape = [](auto const x, auto const y) constexpr noexcept {
         return x == '\\' and (std::iscntrl(y) or y == '\\' or y == '\"' or y == '\'');
      };
      string_literal |= action::adjacent_remove_if(legal_escape);
   }

   /// \brief Transforms escaped characters into their actual character representation.
   ///
   void generate_escapes(std::string& string_literal) noexcept
   {
      auto transform_escape = [](auto&& subrange) noexcept {
         Expects(distance(subrange) >= 2);
         auto& escape_char = *next(begin(subrange));
         switch (escape_char) {
         case 'b':
            escape_char = '\b';
            break;
         case 'f':
            escape_char = '\f';
            break;
         case 'n':
            escape_char = '\n';
            break;
         case 'r':
            escape_char = '\r';
            break;
         case 't':
            escape_char = '\t';
            break;
         case '\'':
            escape_char = '\'';
            break;
         case '\"':
            escape_char = '\"';
            break;
         case '\\':
            escape_char = '\\';
            break;
         }
      };

      // the first subrange doesn't have any *legal* escapes, so we can just ignore it.
      auto escapes = string_literal | view::split(is_escape) | view::drop(1);
      for_each(escapes, transform_escape);
      erase_backslashes(string_literal);
   }

   std::string scan_string_literal(std::istream& in) noexcept
   {
      constexpr auto not_string_literal_sentinel = [](char const x) constexpr noexcept {
         return not (x == '"' or cjdb::is_newline(x));
      };

      auto string_body = std::string{};
      do {
         string_body += static_cast<char>(in.get());
         string_body += ltcpp::consume_istream_while(in, not_string_literal_sentinel);
      } while (in.peek() == '"' and string_body.back() == '\\');

      if (in.peek() == '"') {
         string_body += static_cast<char>(in.get());
      }
      return string_body;
   }
} // namespace

namespace ltcpp::detail_lexer {
   using namespace ranges;

   token scan_string_literal(std::istream& in, source_coordinate const cursor) noexcept
   {
      auto string_body = ::scan_string_literal(in);
      auto const kind = ::validate_string_literal(string_body);
      if (kind == token_kind::string_literal) {
         ::generate_escapes(string_body);
      }
      return ltcpp::make_token(std::move(string_body), cursor, kind);
   }
} // namespace lexer::detail_lexer
