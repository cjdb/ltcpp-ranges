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
#include "ltcpp/lexer/detail/scan_string_literal.hpp"
#include "ltcpp/lexer/token.hpp"

#include "./check_scan.hpp"
#include <catch2/catch.hpp>
#include <string>
#include <string_view>

TEST_CASE("Check string literals") {
   using ltcpp::token_kind;
   using ltcpp::detail_lexer::scan_string_literal;
   using namespace std::string_literals;
   using namespace std::string_view_literals;

   SECTION("Well-formed string literals") {
      CHECK_SCAN(scan_string_literal, R"("")", token_kind::string_literal, ""sv);
      CHECK_SCAN(scan_string_literal, R"("hello")", token_kind::string_literal, ""sv);
      CHECK_SCAN(scan_string_literal, R"("hello\bworld")", token_kind::string_literal,
         "\"hello\bworld\""sv);
      CHECK_SCAN(scan_string_literal, R"("hello\fworld")", token_kind::string_literal,
         "\"hello\fworld\""sv);
      CHECK_SCAN(scan_string_literal, R"("hello\nworld")", token_kind::string_literal,
         "\"hello\nworld\""sv);
      CHECK_SCAN(scan_string_literal, R"("hello\rworld")", token_kind::string_literal,
         "\"hello\rworld\""sv);
      CHECK_SCAN(scan_string_literal, R"("hello\tworld")", token_kind::string_literal,
         "\"hello\tworld\""sv);
      CHECK_SCAN(scan_string_literal, R"("hello\'world")", token_kind::string_literal,
         "\"hello'world\""sv);
      CHECK_SCAN(scan_string_literal, R"("hello\"world")", token_kind::string_literal,
         R"("hello"world")"sv);
      CHECK_SCAN(scan_string_literal, R"("hello\\world")", token_kind::string_literal,
         R"("hello\world")"sv);
   }

   SECTION("Unterminated string literals") {
      CHECK_SCAN(scan_string_literal, R"("hello)", token_kind::unterminated_string_literal, ""sv);
      CHECK_SCAN(scan_string_literal, R"("hello\")", token_kind::unterminated_string_literal,
         R"("hello\")"sv);
   }

   SECTION("Invalid escape sequences") {
      CHECK_SCAN(scan_string_literal, R"("\x")", token_kind::invalid_escape_sequence, ""sv);
      CHECK_SCAN(scan_string_literal, R"("hell\o")", token_kind::invalid_escape_sequence, ""sv);
      CHECK_SCAN(scan_string_literal, R"("\thell\o")", token_kind::invalid_escape_sequence, ""sv);
   }
}
