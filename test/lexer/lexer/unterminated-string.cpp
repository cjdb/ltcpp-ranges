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
   // String literal not terminated
   using ltcpp::token, ltcpp::token_kind, ltcpp::source_coordinate;
   using column_type = source_coordinate::column_type;
   using line_type = source_coordinate::line_type;

   auto errors = std::ostringstream{};
   auto report = ltcpp::reporter{errors};

   auto const source = std::string{
      "\tvoid\rmain($)\n"
      "}\n"
      "\treturn\"This string is terminated.\"\n"
      "\f"
      "return\"This string is not terminated.\n"
      "   return \"This string is also not terminated\\\"\n"
   };
   auto const tokens = generate_tokens(source, report);
   auto const expected_tokens = std::vector{
      token{
         token_kind::void_,
         "void",
         source_coordinate{line_type{1}, column_type{2}},
         source_coordinate{line_type{1}, column_type{6}}
      },
      token{
         token_kind::identifier,
         "main",
         source_coordinate{line_type{1}, column_type{7}},
         source_coordinate{line_type{1}, column_type{11}}
      },
      token{
         token_kind::paren_open,
         "(",
         source_coordinate{line_type{1}, column_type{11}},
         source_coordinate{line_type{1}, column_type{12}}
      },
      token{
         token_kind::unknown_token,
         "$",
         source_coordinate{line_type{1}, column_type{12}},
         source_coordinate{line_type{1}, column_type{13}}
      },
      token{
         token_kind::paren_close,
         ")",
         source_coordinate{line_type{1}, column_type{13}},
         source_coordinate{line_type{1}, column_type{14}}
      },
      token{
         token_kind::brace_close,
         "}",
         source_coordinate{line_type{2}, column_type{1}},
         source_coordinate{line_type{2}, column_type{2}}
      },
      token{
         token_kind::return_,
         "return",
         source_coordinate{line_type{3}, column_type{2}},
         source_coordinate{line_type{3}, column_type{8}}
      },
      token{
         token_kind::string_literal,
         "\"This string is terminated.\"",
         source_coordinate{line_type{3}, column_type{8}},
         source_coordinate{line_type{3}, column_type{36}}
      },
      token{
         token_kind::return_,
         "return",
         source_coordinate{line_type{5}, column_type{1}},
         source_coordinate{line_type{5}, column_type{7}}
      },
      token{
         token_kind::unterminated_string_literal,
         "\"This string is not terminated.",
         source_coordinate{line_type{5}, column_type{7}},
         source_coordinate{line_type{5}, column_type{38}}
      },
      token{
         token_kind::return_,
         "return",
         source_coordinate{line_type{6}, column_type{4}},
         source_coordinate{line_type{6}, column_type{10}}
      },
      token{
         token_kind::unterminated_string_literal,
         "\"This string is also not terminated\\\"",
         source_coordinate{line_type{6}, column_type{11}},
         source_coordinate{line_type{6}, column_type{48}}
      },
      token{
         token_kind::eof,
         "$",
         source_coordinate{line_type{7}, column_type{1}},
         source_coordinate{line_type{7}, column_type{1}}
      }
   };
   CHECK_EQUAL(tokens, expected_tokens);
   CHECK(report.errors() == 3);
   CHECK(report.warnings() == 0);
   CHECK(errors.str() ==
      "lexical error at {1:12}: unknown token: \"$\".\n"
      "lexical error at {5:7}: unterminated string literal: \"\"This string is not terminated.\".\n"
      "lexical error at {6:11}: unterminated string literal: \"\"This string is also not terminated\\\"\".\n");

   return ::test_result();
}
