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
#include "../../simple_test.hpp"
#include <string>
#include <string_view>

int main()
{
   // Checks that ltcpp::detail_lexer::scan_identifier is correct.
   using ltcpp::token_kind;
   using ltcpp::detail_lexer::scan_identifier;
   using namespace std::string_literals;
   using namespace std::string_view_literals;

   { // Check identifiers
      CHECK_SCAN(scan_identifier, "_", token_kind::identifier, ""sv);
      CHECK_SCAN(scan_identifier, "hello", token_kind::identifier, ""sv);
      CHECK_SCAN(scan_identifier, "a1b2", token_kind::identifier, ""sv);
      CHECK_SCAN(scan_identifier, "tin", token_kind::identifier, ""sv);
      CHECK_SCAN(scan_identifier, "break1", token_kind::identifier, ""sv);
   }
   { // Check logical operators
      CHECK_SCAN(scan_identifier, "and", token_kind::and_, ""sv);
      CHECK_SCAN(scan_identifier, "or", token_kind::or_, ""sv);
      CHECK_SCAN(scan_identifier, "not", token_kind::not_, ""sv);
   }
   { // Check type specifiers
      CHECK_SCAN(scan_identifier, "bool", token_kind::bool_, ""sv);
      CHECK_SCAN(scan_identifier, "char8", token_kind::char8, ""sv);
      CHECK_SCAN(scan_identifier, "float16", token_kind::float16, ""sv);
      CHECK_SCAN(scan_identifier, "float32", token_kind::float32, ""sv);
      CHECK_SCAN(scan_identifier, "float64", token_kind::float64, ""sv);
      CHECK_SCAN(scan_identifier, "int8", token_kind::int8, ""sv);
      CHECK_SCAN(scan_identifier, "int16", token_kind::int16, ""sv);
      CHECK_SCAN(scan_identifier, "int32", token_kind::int32, ""sv);
      CHECK_SCAN(scan_identifier, "int64", token_kind::int64, ""sv);
      CHECK_SCAN(scan_identifier, "void", token_kind::void_, ""sv);
   }
   { // Check boolean literals
      CHECK_SCAN(scan_identifier, "true", token_kind::boolean_literal, ""sv);
      CHECK_SCAN(scan_identifier, "false", token_kind::boolean_literal, ""sv);
   }
   { // Check keywords
      CHECK_SCAN(scan_identifier, "assert", token_kind::assert_, ""sv);
      CHECK_SCAN(scan_identifier, "break", token_kind::break_, ""sv);
      CHECK_SCAN(scan_identifier, "continue", token_kind::continue_, ""sv);
      CHECK_SCAN(scan_identifier, "for", token_kind::for_, ""sv);
      CHECK_SCAN(scan_identifier, "fun", token_kind::fun_, ""sv);
      CHECK_SCAN(scan_identifier, "if", token_kind::if_, ""sv);
      CHECK_SCAN(scan_identifier, "import", token_kind::import_, ""sv);
      CHECK_SCAN(scan_identifier, "let", token_kind::let_, ""sv);
      CHECK_SCAN(scan_identifier, "module", token_kind::module_, ""sv);
      CHECK_SCAN(scan_identifier, "mutable", token_kind::mutable_, ""sv);
      CHECK_SCAN(scan_identifier, "readable", token_kind::readable_, ""sv);
      CHECK_SCAN(scan_identifier, "ref", token_kind::ref_, ""sv);
      CHECK_SCAN(scan_identifier, "return", token_kind::return_, ""sv);
      CHECK_SCAN(scan_identifier, "while", token_kind::while_, ""sv);
      CHECK_SCAN(scan_identifier, "writable", token_kind::writable_, ""sv);
   }
   { // Check strings with symbols that aren't part of the set of identifiers
      CHECK_SCAN(scan_identifier, "cursor 1", token_kind::identifier, "cursor"sv);
      CHECK_SCAN(scan_identifier, "cursor!1", token_kind::identifier, "cursor"sv);
   }

   return ::test_result();
}
