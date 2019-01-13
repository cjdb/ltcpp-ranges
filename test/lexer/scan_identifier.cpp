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
#include "ltcpp/lexer/detail/scan_identifier.hpp"

#include "./check_scan.hpp"
#include <catch2/catch.hpp>
#include <string>
#include <string_view>

TEST_CASE("Checks that ltcpp::detail_lexer::scan_identifier is correct.") {
   using ltcpp::token_kind;
   using ltcpp::detail_lexer::scan_identifier;
   using namespace std::string_literals;
   using namespace std::string_view_literals;

   SECTION("Check identifiers") {
      CHECK_SCAN(scan_identifier, "_", token_kind::identifier, ""sv);
      CHECK_SCAN(scan_identifier, "hello", token_kind::identifier, ""sv);
      CHECK_SCAN(scan_identifier, "a1b2", token_kind::identifier, ""sv);
      CHECK_SCAN(scan_identifier, "tin", token_kind::identifier, ""sv);
      CHECK_SCAN(scan_identifier, "break1", token_kind::identifier, ""sv);
   }
   SECTION("Check logical operators") {
      CHECK_SCAN(scan_identifier, "and", token_kind::and_, ""sv);
      CHECK_SCAN(scan_identifier, "or", token_kind::or_, ""sv);
      CHECK_SCAN(scan_identifier, "not", token_kind::not_, ""sv);
   }
   SECTION("Check type specifiers") {
      CHECK_SCAN(scan_identifier, "bool", token_kind::bool_, ""sv);
      CHECK_SCAN(scan_identifier, "char", token_kind::char_, ""sv);
      CHECK_SCAN(scan_identifier, "double", token_kind::double_, ""sv);
      CHECK_SCAN(scan_identifier, "int", token_kind::int_, ""sv);
      CHECK_SCAN(scan_identifier, "string", token_kind::string_, ""sv);
   }
   SECTION("Check boolean literals") {
      CHECK_SCAN(scan_identifier, "true", token_kind::boolean_literal, ""sv);
      CHECK_SCAN(scan_identifier, "false", token_kind::boolean_literal, ""sv);
   }
   SECTION("Check keywords") {
      CHECK_SCAN(scan_identifier, "break", token_kind::break_, ""sv);
      CHECK_SCAN(scan_identifier, "const", token_kind::const_, ""sv);
      CHECK_SCAN(scan_identifier, "continue", token_kind::continue_, ""sv);
      CHECK_SCAN(scan_identifier, "for", token_kind::for_, ""sv);
      CHECK_SCAN(scan_identifier, "if", token_kind::if_, ""sv);
      CHECK_SCAN(scan_identifier, "return", token_kind::return_, ""sv);
      CHECK_SCAN(scan_identifier, "while", token_kind::while_, ""sv);
   }
   SECTION("Check strings with symbols that aren't part of the set of identifiers") {
      CHECK_SCAN(scan_identifier, "cursor 1", token_kind::identifier, "cursor"sv);
      CHECK_SCAN(scan_identifier, "cursor!1", token_kind::identifier, "cursor"sv);
   }
}
