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
#include "ltcpp/lexer/detail/scan_symbol.hpp"
#include "ltcpp/lexer/token.hpp"

#include "./check_scan.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Check symbols are scanned correctly") {
   using ltcpp::token_kind;
   using namespace std::string_literals;
   using namespace std::string_view_literals;

   auto scan_symbol = [](auto& in, auto&& y) noexcept {
      auto current = static_cast<char>(in.get());
      return ltcpp::detail_lexer::scan_symbol(in, current, y);
   };

   CHECK_SCAN(scan_symbol, "++", token_kind::increment, ""sv);
   CHECK_SCAN(scan_symbol, "--", token_kind::decrement, ""sv);

   SECTION("arithmetic") {
      CHECK_SCAN(scan_symbol, "+", token_kind::plus, ""sv);
      CHECK_SCAN(scan_symbol, "-", token_kind::minus, ""sv);
      CHECK_SCAN(scan_symbol, "*", token_kind::times, ""sv);
      CHECK_SCAN(scan_symbol, "/", token_kind::divide, ""sv);
      CHECK_SCAN(scan_symbol, "%", token_kind::modulo, ""sv);
   }

   SECTION("assignment") {
      CHECK_SCAN(scan_symbol, "=", token_kind::assign, ""sv);
      CHECK_SCAN(scan_symbol, "+=", token_kind::plus_eq, ""sv);
      CHECK_SCAN(scan_symbol, "-=", token_kind::minus_eq, ""sv);
      CHECK_SCAN(scan_symbol, "*=", token_kind::times_eq, ""sv);
      CHECK_SCAN(scan_symbol, "/=", token_kind::divide_eq, ""sv);
      CHECK_SCAN(scan_symbol, "%=", token_kind::modulo_eq, ""sv);
   }

   SECTION("comparison") {
      CHECK_SCAN(scan_symbol, "==", token_kind::equal_to, ""sv);
      CHECK_SCAN(scan_symbol, "!=", token_kind::not_equal_to, ""sv);
      CHECK_SCAN(scan_symbol, "<", token_kind::less, ""sv);
      CHECK_SCAN(scan_symbol, "<=", token_kind::less_equal, ""sv);
      CHECK_SCAN(scan_symbol, ">", token_kind::greater, ""sv);
      CHECK_SCAN(scan_symbol, ">=", token_kind::greater_equal, ""sv);
   }

   SECTION("separators") {
      CHECK_SCAN(scan_symbol, ".", token_kind::dot, ""sv);
      CHECK_SCAN(scan_symbol, ",", token_kind::comma, ""sv);
      CHECK_SCAN(scan_symbol, ";", token_kind::semicolon, ""sv);
      CHECK_SCAN(scan_symbol, "{", token_kind::brace_open, ""sv);
      CHECK_SCAN(scan_symbol, "}", token_kind::brace_close, ""sv);
      CHECK_SCAN(scan_symbol, "(", token_kind::paren_open, ""sv);
      CHECK_SCAN(scan_symbol, ")", token_kind::paren_close, ""sv);
      CHECK_SCAN(scan_symbol, "[", token_kind::square_open, ""sv);
      CHECK_SCAN(scan_symbol, "]", token_kind::square_close, ""sv);
   }

   SECTION("Check tokens with strange ends to ensure they only conform to the first token.") {
      CHECK_SCAN(scan_symbol, "+ =", token_kind::plus, "+"sv);
      CHECK_SCAN(scan_symbol, "+-", token_kind::plus, "+"sv);
      CHECK_SCAN(scan_symbol, "%%", token_kind::modulo, "%"sv);
   }

   SECTION("Check non-existing tokens") {
      CHECK_SCAN(scan_symbol, "@", token_kind::unknown_token, ""sv);
      CHECK_SCAN(scan_symbol, "#", token_kind::unknown_token, ""sv);
      CHECK_SCAN(scan_symbol, "$", token_kind::unknown_token, ""sv);
      CHECK_SCAN(scan_symbol, "&", token_kind::unknown_token, ""sv);
      CHECK_SCAN(scan_symbol, "|", token_kind::unknown_token, ""sv);
      CHECK_SCAN(scan_symbol, ":", token_kind::unknown_token, ""sv);
      CHECK_SCAN(scan_symbol, "!", token_kind::unknown_token, ""sv);
      CHECK_SCAN(scan_symbol, "\\", token_kind::unknown_token, ""sv);
      CHECK_SCAN(scan_symbol, "?", token_kind::unknown_token, ""sv);
   }
}
