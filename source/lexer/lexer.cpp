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
#include <string_view>

#include "ltcpp/utility/peek.hpp"

namespace {
   using ltcpp::token, ltcpp::token_kind, ltcpp::reporter, ltcpp::source_coordinate;
   using ltcpp::pass;
   namespace lexer = ltcpp::detail_lexer;

   void check_for_error(reporter& report, token const& t) noexcept
   {
      using namespace std::string_view_literals;
      auto const message =
         t.kind() == token_kind::exponent_lacking_digit      ? "floating-point exponent lacking digits: \""sv
       : t.kind() == token_kind::too_many_radix_points       ? "too many radix points in floating-point literal: \""sv
       : t.kind() == token_kind::unterminated_string_literal ? "unterminated string literal: \""sv
       : t.kind() == token_kind::unknown_token               ? "unknown token: \""sv
                                                             : ""sv;
      if (not empty(message)) {
         report.error(pass::lexical, t.cursor_range().begin, message, t.spelling(), "\".");
      }
      else {
         return;
      }
   }

   using scan_function = token(&)(std::istream&, source_coordinate) noexcept(false);
   token guarded_scan(scan_function scanner, std::istream& in, source_coordinate cursor,
      reporter& report) noexcept(false)
   {
      auto result = scanner(in, cursor);
      check_for_error(report, result);
      return result;
   }

   token scan_non_eof(std::istream& in, reporter& report, source_coordinate cursor, char next)
   noexcept(false)
   {
      if (cjdb::isalpha(next)) {
         return lexer::scan_identifier(in, cursor);
      }
      else if (cjdb::isdigit(next)) {
         return guarded_scan(lexer::scan_number, in, cursor, report);
      }
      else if (next == '.') {
         return guarded_scan(lexer::possibly_float, in, cursor, report);
      }
      else if (next == '"') {
         return guarded_scan(lexer::scan_string_literal, in, cursor, report);
      }
      else {
         return guarded_scan(lexer::scan_symbol, in, cursor, report);
      }
   }

   token scan_eof(std::istream& in, reporter& report,
      tl::expected<source_coordinate, lexer::unterminated_comment_error> cursor_result) noexcept(false)
   {
      in.get();
      assert(in.eof());

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

   /// \brief Extracts the next token from the input stream.
   /// \param istream The stream to extract from.
   /// \param report The warning and error reporter.
   /// \param cursor_result In the event of an unterminated multi-line comment, cursor_result will
   ///        contain both the position of the cursor at the start of the comment and at the end of
   ///        the comment. Otherwise, it contains the coordinate to the first non-whitespace
   ///        character in the file.
   /// \returns A token that contains information about the token's kind, the token's position in
   ///          the source file, and how it is spelt in the source file.
   /// \throws std::runtime_error if a character cannot be read from the stream, and
   ///         `in.eof() == false`.
   ///
   token scan_token(std::istream& in, reporter& report,
      tl::expected<source_coordinate, lexer::unterminated_comment_error> cursor_result) noexcept(false)
   {
      if (auto const next = ltcpp::peek(in); next) {
         assert(cursor_result);
         return scan_non_eof(in, report, *cursor_result, *next);
      }
      else {
         return scan_eof(in, report, std::move(cursor_result));
      }
   }
} // namespace

namespace ltcpp {
   /// \brief Strips all whitespace and comments, and extracts the next token from an input stream.
   /// \param istream The stream to extract from.
   /// \param report The warning and error reporter.
   /// \param cursor The position in the source file that we are currently reading from.
   /// \returns A token that contains information about the token's kind, the token's position in
   ///          the source file, and how it is spelt in the source file.
   /// \throws std::runtime_error if a character cannot be read from the stream, and
   ///         `in.eof() == false`.
   ///
   token generate_token(std::istream& in, reporter& report, source_coordinate const cursor) noexcept(false)
   {
      auto const cursor_result = detail_lexer::scan_whitespace_like(in, cursor);
      auto const token_result = ::scan_token(in, report, cursor_result);
      return token_result;
   }
} // namespace ltcpp
