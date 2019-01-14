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
#include "cjdb/cctype/isalpha.hpp"
#include "cjdb/cctype/isdigit.hpp"
#include <istream>
#include <gsl/gsl>
#include "ltcpp/lexer/detail/scan_identifier.hpp"
#include "ltcpp/lexer/detail/scan_number.hpp"
#include "ltcpp/lexer/detail/scan_string_literal.hpp"
#include "ltcpp/lexer/detail/scan_symbol.hpp"
#include "ltcpp/lexer/detail/scan_whitespace.hpp"
#include "ltcpp/lexer/lexer.hpp"
#include "ltcpp/reporter.hpp"
#include "ltcpp/source_coordinate.hpp"
#include "ltcpp/lexer/token.hpp"

namespace {
   using ltcpp::token, ltcpp::token_kind, ltcpp::reporter, ltcpp::source_coordinate;
   using ltcpp::pass;
   namespace lexer = ltcpp::detail_lexer;

   void check_for_error(reporter& report, token const& t) noexcept(false)
   {
      if (t.kind() == token_kind::exponent_lacking_digit) {
         report.error(pass::lexical, t.position().begin,
            "floating-point exponent lacking digits: \"", t.spelling(), "\".");
      }
      else if (t.kind() == token_kind::too_many_radix_points) {
         report.error(pass::lexical, t.position().begin,
            "too many radix points in floating-point literal: \"", t.spelling(), "\".");
      }
      else if (t.kind() == ltcpp::token_kind::unterminated_string_literal) {
         report.error(pass::lexical, t.position().end,
            "unterminated string literal: \"", t.spelling(), "\".");
      }
      else {
         return;
      }
   }

   /// \brief
   ///
   token scan_token(std::istream& in, reporter& report,
      tl::expected<source_coordinate, lexer::unterminated_comment_error> cursor_result) noexcept(false)
   {
      if (auto const peek = static_cast<char>(in.peek()); cjdb::isalpha(peek)) {
         assert(cursor_result);
         return lexer::scan_identifier(in, *cursor_result);
      }
      else if (cjdb::isdigit(peek)) {
         assert(cursor_result);
         auto result = lexer::scan_number(in, *cursor_result);
         check_for_error(report, result);
         return result;
      }
      else if (peek == '.') {
         assert(cursor_result);
         auto result = lexer::possibly_float(in, *cursor_result);
         check_for_error(report, result);
         return result;
      }
      else if (peek == '"') {
         assert(cursor_result);
         auto result = lexer::scan_string_literal(in, *cursor_result);
         check_for_error(report, result);
         return result;
      }
      else if (auto current = '\0'; in.get(current)) {
         assert(cursor_result);
         return lexer::scan_symbol(in, current, *cursor_result);
      }
      else if (in.eof()) {
         auto eof_cursor = source_coordinate{};

         if (cursor_result) {
            eof_cursor = *cursor_result;
         }
         else {
            auto const error = cursor_result.error();
            report.error(pass::lexical, error.begin, "unterminated multi-line comment.");
            eof_cursor = error.end;
         }

         constexpr auto eof_spelling = "$";
         return token{token_kind::eof, eof_spelling, eof_cursor, eof_cursor};
      }
      else {
         throw std::runtime_error{"Unable to read character from stream."};
      }
   }
} // namespace

namespace ltcpp {
   ///
   ///
   token generate_token(std::istream& in, reporter& report, source_coordinate const cursor) noexcept(false)
   {
      auto const cursor_result = detail_lexer::scan_whitespace_like(in, cursor);
      auto const token_result = ::scan_token(in, report, cursor_result);
      return token_result;
   }
} // namespace ltcpp
