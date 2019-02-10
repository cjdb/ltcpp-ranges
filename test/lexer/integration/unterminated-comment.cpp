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
#include "ltcpp/lexer/lexer.hpp"
#include "ltcpp/lexer/token.hpp"
#include "ltcpp/source_coordinate.hpp"
#include "ltcpp/reporter.hpp"

#include <string>
#include <sstream>
#include "./test_common.hpp"

int main()
{
   // Unterminated comment
   using ltcpp::token, ltcpp::token_kind, ltcpp::source_coordinate;
   using column_type = source_coordinate::column_type;
   using line_type = source_coordinate::line_type;

   auto errors = std::ostringstream{};
   auto report = ltcpp::reporter{errors};

   auto const source = std::string{
      "fun main/*() -> int32\n"
      "{\n"
      "   abacus;\n"
      "}\n"
   };

   auto const tokens = generate_tokens(source, report);
   auto const expected_tokens = std::vector{
      token{
         token_kind::fun_,
         "fun",
         source_coordinate{line_type{1}, column_type{1}},
         source_coordinate{line_type{1}, column_type{4}}
      },
      token{
         token_kind::identifier,
         "main",
         source_coordinate{line_type{1}, column_type{5}},
         source_coordinate{line_type{1}, column_type{9}}
      },
      token{
         token_kind::eof,
         "$",
         source_coordinate{line_type{5}, column_type{1}},
         source_coordinate{line_type{5}, column_type{1}}
      }
   };

   CHECK_EQUAL(tokens, expected_tokens);
   CHECK(report.errors() == 1);
   CHECK(report.warnings() == 0);
   CHECK(errors.str() == "lexical error at {1:9}: unterminated multi-line comment.\n");

   return ::test_result();
}
