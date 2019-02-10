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
   // Check a program starting on Line 1, Column 1
   using ltcpp::token, ltcpp::token_kind, ltcpp::source_coordinate;
   using column_type = source_coordinate::column_type;
   using line_type = source_coordinate::line_type;

   auto errors = std::ostringstream{};
   auto report = ltcpp::reporter{errors};

   auto const source = std::string{
      "fun main() -> int32\n"
      "{\n"
      "   print(\"Hello, world!\\n\");\n"
      "}\n"
   };

   auto tokens = generate_tokens(source, report);
   assert(size(tokens) == 14);

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
         token_kind::paren_open,
         "(",
         source_coordinate{line_type{1}, column_type{9}},
         source_coordinate{line_type{1}, column_type{10}}
      },
      token{
         token_kind::paren_close,
         ")",
         source_coordinate{line_type{1}, column_type{10}},
         source_coordinate{line_type{1}, column_type{11}}
      },
      token{
         token_kind::arrow,
         "->",
         source_coordinate{line_type{1}, column_type{12}},
         source_coordinate{line_type{1}, column_type{14}}
      },
      token{
         token_kind::int32,
         "int32",
         source_coordinate{line_type{1}, column_type{15}},
         source_coordinate{line_type{1}, column_type{20}}
      },
      token{
         token_kind::brace_open,
         "{",
         source_coordinate{line_type{2}, column_type{1}},
         source_coordinate{line_type{2}, column_type{2}}
      },
      token{
         token_kind::identifier,
         "print",
         source_coordinate{line_type{3}, column_type{4}},
         source_coordinate{line_type{3}, column_type{9}}
      },
      token{
         token_kind::paren_open,
         "(",
         source_coordinate{line_type{3}, column_type{9}},
         source_coordinate{line_type{3}, column_type{10}}
      },
      token{
         token_kind::string_literal,
         "\"Hello, world!\n\"",
         source_coordinate{line_type{3}, column_type{10}},
         source_coordinate{line_type{3}, column_type{26}}
      },
      token{
         token_kind::paren_close,
         ")",
         source_coordinate{line_type{3}, column_type{26}},
         source_coordinate{line_type{3}, column_type{27}}
      },
      token{
         token_kind::semicolon,
         ";",
         source_coordinate{line_type{3}, column_type{27}},
         source_coordinate{line_type{3}, column_type{28}}
      },
      token{
         token_kind::brace_close,
         "}",
         source_coordinate{line_type{4}, column_type{1}},
         source_coordinate{line_type{4}, column_type{2}}
      },
      token{
         token_kind::eof,
         "$",
         source_coordinate{line_type{5}, column_type{1}},
         source_coordinate{line_type{5}, column_type{1}}
      }
   };

   CHECK_EQUAL(tokens, expected_tokens);
   CHECK(report.errors() == 0);
   CHECK(report.warnings() == 0);
   CHECK(empty(errors.str()));

   return ::test_result();
}
