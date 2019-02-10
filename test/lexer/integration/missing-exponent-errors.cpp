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
   // Floating-point literal with exponent suffix, but missing exponent
   using ltcpp::token, ltcpp::token_kind, ltcpp::source_coordinate;
   using column_type = source_coordinate::column_type;
   using line_type = source_coordinate::line_type;

   auto errors = std::ostringstream{};
   auto report = ltcpp::reporter{errors};

   auto const tokens = generate_tokens("x <- 543e 87.", report);
   auto const expected_tokens = std::vector{
      token{
         token_kind::identifier,
         "x",
         source_coordinate{line_type{1}, column_type{1}},
         source_coordinate{line_type{1}, column_type{2}}
      },
      token{
         token_kind::assign,
         "<-",
         source_coordinate{line_type{1}, column_type{3}},
         source_coordinate{line_type{1}, column_type{5}}
      },
      token{
         token_kind::exponent_lacking_digit,
         "543e",
         source_coordinate{line_type{1}, column_type{6}},
         source_coordinate{line_type{1}, column_type{10}}
      },
      token{
         token_kind::floating_literal,
         "87.",
         source_coordinate{line_type{1}, column_type{11}},
         source_coordinate{line_type{1}, column_type{14}}
      },
      token{
         token_kind::eof,
         "$",
         source_coordinate{line_type{1}, column_type{14}},
         source_coordinate{line_type{1}, column_type{14}}
      }
   };
   CHECK_EQUAL(tokens, expected_tokens);
   CHECK(report.errors() == 1);
   CHECK(report.warnings() == 0);
   CHECK(errors.str() ==
      "lexical error at {1:6}: floating-point exponent lacking digits: \"543e\".\n");

   return ::test_result();
}
