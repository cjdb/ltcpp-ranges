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

#include <algorithm>
#include <cjdb/cctype/is_newline.hpp>
#include <gsl/gsl>
#include <istream>
#include <iterator>
#include "ltcpp/consume_istream_while.hpp"
#include "ltcpp/lexer/detail/scan_string_literal.hpp"
#include <string>
#include <string_view>

namespace {
   using namespace std::string_view_literals;

   constexpr auto legal_escapes = R"(bfnrt\'")"sv;

   /// \brief Checks each escape sequence is a legal escape sequence.
   /// \returns `token_kind::string_literal` if all escape sequences are legal;
   ///          `token_kind::ivalid_escape_sequence` otherwise.
   ///
   ltcpp::token_kind validate_escapes(std::string_view const lexeme) noexcept
   {
      for (auto i = begin(lexeme); i != end(lexeme); ++i) {
         if (*i == '\\') {
            auto successor = std::next(i);
            if (successor != end(lexeme) and legal_escapes.find(*successor) == std::string_view::npos) {
               return ltcpp::token_kind::invalid_escape_sequence;
            }
            ++i;
         }
      }

      return ltcpp::token_kind::string_literal;
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
         if (r1.size() < r2.size()) {
            return false;
         }
         return std::equal(std::next(begin(r1), static_cast<long int>(size(r1) - size(r2))), end(r1), begin(r2));
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
      auto first = begin(string_literal);
      auto last = end(string_literal);
      while (first != last) {
         first = adjacent_find(first, last, legal_escape);
         if (first != last) {
            last = move(next(first), last, first);
         }
      }
      string_literal.erase(last, end(string_literal));
   }

   /// \brief Transforms escaped characters into their actual character representation.
   ///
   void generate_escapes(std::string& string_literal) noexcept
   {
      for (auto i = begin(string_literal); i != end(string_literal); ++i) {
         if (*i == '\\') {
            if (auto escape_char = next(i); escape_char != end(string_literal)) {
               switch (*escape_char) {
               case 'b':
                  *escape_char = '\b';
                  break;
               case 'f':
                  *escape_char = '\f';
                  break;
               case 'n':
                  *escape_char = '\n';
                  break;
               case 'r':
                  *escape_char = '\r';
                  break;
               case 't':
                  *escape_char = '\t';
                  break;
               case '\'':
                  *escape_char = '\'';
                  break;
               case '\"':
                  *escape_char = '\"';
                  break;
               case '\\':
                  *escape_char = '\\';
                  break;
               }
            }
         }
      }

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
