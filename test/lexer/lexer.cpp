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
#include "ltcpp/reporter.hpp"

#include <catch2/catch.hpp>
#include <string>
#include <sstream>

#define CHECK_EQUAL(actual, expected) {       \
   REQUIRE(size(actual) == size(expected));   \
   for (auto i = 0U; i < size(actual); ++i) { \
      CHECK(actual[i] == expected[i]);        \
   }                                          \
}                                             \


std::vector<ltcpp::token> generate_tokens(std::string const& source, ltcpp::reporter& report)
{
   auto in = std::istringstream{source};
   in.unsetf(std::ios_base::skipws);

   auto tokens = std::vector<ltcpp::token>{};
   auto cursor = ltcpp::source_coordinate{};
   while (true) {
      auto token = ltcpp::generate_token(in, report, cursor);
      cursor = token.position().end;
      tokens.push_back(std::move(token));
      if (token.kind() == ltcpp::token_kind::eof) {
         break;
      }
   }
   return tokens;
}

TEST_CASE("Check lexer") {
   using ltcpp::token, ltcpp::token_kind, ltcpp::source_coordinate;
   using column_type = source_coordinate::column_type;
   using line_type = source_coordinate::line_type;
   auto errors = std::ostringstream{};
   auto report = ltcpp::reporter{errors};

   SECTION("Conforming program starting on line 1") {
      auto const source = std::string{
         "int main()\n"
         "{\n"
         "   print(\"Hello, world!\\n\");\n"
         "}\n"
      };

      auto tokens = generate_tokens(source, report);
      REQUIRE(size(tokens) == 12);

      auto const expected_tokens = std::vector{
         token{
            token_kind::int_,
            "int",
            source_coordinate{column_type{1}, line_type{1}},
            source_coordinate{column_type{4}, line_type{1}}
         },
         token{
            token_kind::identifier,
            "main",
            source_coordinate{column_type{5}, line_type{1}},
            source_coordinate{column_type{9}, line_type{1}}
         },
         token{
            token_kind::paren_open,
            "(",
            source_coordinate{column_type{9}, line_type{1}},
            source_coordinate{column_type{10}, line_type{1}}
         },
         token{
            token_kind::paren_close,
            ")",
            source_coordinate{column_type{10}, line_type{1}},
            source_coordinate{column_type{11}, line_type{1}}
         },
         token{
            token_kind::brace_open,
            "{",
            source_coordinate{column_type{1}, line_type{2}},
            source_coordinate{column_type{2}, line_type{2}}
         },
         token{
            token_kind::identifier,
            "print",
            source_coordinate{column_type{4}, line_type{3}},
            source_coordinate{column_type{9}, line_type{3}}
         },
         token{
            token_kind::paren_open,
            "(",
            source_coordinate{column_type{9}, line_type{3}},
            source_coordinate{column_type{10}, line_type{3}}
         },
         token{
            token_kind::string_literal,
            "\"Hello, world!\n\"",
            source_coordinate{column_type{10}, line_type{3}},
            source_coordinate{column_type{26}, line_type{3}}
         },
         token{
            token_kind::paren_close,
            ")",
            source_coordinate{column_type{26}, line_type{3}},
            source_coordinate{column_type{27}, line_type{3}}
         },
         token{
            token_kind::semicolon,
            ";",
            source_coordinate{column_type{27}, line_type{3}},
            source_coordinate{column_type{28}, line_type{3}}
         },
         token{
            token_kind::brace_close,
            "}",
            source_coordinate{column_type{1}, line_type{4}},
            source_coordinate{column_type{2}, line_type{4}}
         },
         token{
            token_kind::eof,
            "$",
            source_coordinate{column_type{1}, line_type{5}},
            source_coordinate{column_type{1}, line_type{5}}
         }
      };

      CHECK_EQUAL(tokens, expected_tokens);
      CHECK(report.errors() == 0);
      CHECK(report.warnings() == 0);
      CHECK(empty(errors.str()));
   }

   SECTION("Conforming program starting on line 2") {
      auto const source = std::string{
         "// conforming program starting on line 2\n"
         "int main()\n"
         "{\n"
         "   print(\"Hello, world!\\n\");\n"
         "}\n"
      };
      auto in = std::istringstream{source};
      in.unsetf(std::ios_base::skipws);

      auto tokens = generate_tokens(source, report);

      REQUIRE(size(tokens) == 12);

      auto const expected_tokens = std::vector{
         token{
            token_kind::int_,
            "int",
            source_coordinate{column_type{1}, line_type{2}},
            source_coordinate{column_type{4}, line_type{2}}
         },
         token{
            token_kind::identifier,
            "main",
            source_coordinate{column_type{5}, line_type{2}},
            source_coordinate{column_type{9}, line_type{2}}
         },
         token{
            token_kind::paren_open,
            "(",
            source_coordinate{column_type{9}, line_type{2}},
            source_coordinate{column_type{10}, line_type{2}}
         },
         token{
            token_kind::paren_close,
            ")",
            source_coordinate{column_type{10}, line_type{2}},
            source_coordinate{column_type{11}, line_type{2}}
         },
         token{
            token_kind::brace_open,
            "{",
            source_coordinate{column_type{1}, line_type{3}},
            source_coordinate{column_type{2}, line_type{3}}
         },
         token{
            token_kind::identifier,
            "print",
            source_coordinate{column_type{4}, line_type{4}},
            source_coordinate{column_type{9}, line_type{4}}
         },
         token{
            token_kind::paren_open,
            "(",
            source_coordinate{column_type{9}, line_type{4}},
            source_coordinate{column_type{10}, line_type{4}}
         },
         token{
            token_kind::string_literal,
            "\"Hello, world!\n\"",
            source_coordinate{column_type{10}, line_type{4}},
            source_coordinate{column_type{26}, line_type{4}}
         },
         token{
            token_kind::paren_close,
            ")",
            source_coordinate{column_type{26}, line_type{4}},
            source_coordinate{column_type{27}, line_type{4}}
         },
         token{
            token_kind::semicolon,
            ";",
            source_coordinate{column_type{27}, line_type{4}},
            source_coordinate{column_type{28}, line_type{4}}
         },
         token{
            token_kind::brace_close,
            "}",
            source_coordinate{column_type{1}, line_type{5}},
            source_coordinate{column_type{2}, line_type{5}}
         },
         token{
            token_kind::eof,
            "$",
            source_coordinate{column_type{1}, line_type{6}},
            source_coordinate{column_type{1}, line_type{6}}
         }
      };

      CHECK_EQUAL(tokens, expected_tokens);

      CHECK(report.errors() == 0);
      CHECK(report.warnings() == 0);
      CHECK(empty(errors.str()));
   }

   SECTION("Assignment with radix-point error") {
      auto const tokens = generate_tokens("x = 10.10.10 .956 a.b", report);
      auto const expected_tokens = std::vector{
         token{
            token_kind::identifier,
            "x",
            source_coordinate{column_type{1}, line_type{1}},
            source_coordinate{column_type{2}, line_type{1}}
         },
         token{
            token_kind::assign,
            "=",
            source_coordinate{column_type{3}, line_type{1}},
            source_coordinate{column_type{4}, line_type{1}}
         },
         token{
            token_kind::too_many_radix_points,
            "10.10.10",
            source_coordinate{column_type{5}, line_type{1}},
            source_coordinate{column_type{13}, line_type{1}}
         },
         token{
            token_kind::floating_literal,
            ".956",
            source_coordinate{column_type{14}, line_type{1}},
            source_coordinate{column_type{18}, line_type{1}}
         },
         token{
            token_kind::identifier,
            "a",
            source_coordinate{column_type{19}, line_type{1}},
            source_coordinate{column_type{20}, line_type{1}}
         },
         token{
            token_kind::dot,
            ".",
            source_coordinate{column_type{20}, line_type{1}},
            source_coordinate{column_type{21}, line_type{1}}
         },
         token{
            token_kind::identifier,
            "b",
            source_coordinate{column_type{21}, line_type{1}},
            source_coordinate{column_type{22}, line_type{1}}
         },
         token{
            token_kind::eof,
            "$",
            source_coordinate{column_type{22}, line_type{1}},
            source_coordinate{column_type{22}, line_type{1}}
         }
      };
      CHECK_EQUAL(tokens, expected_tokens);
      CHECK(report.errors() == 1);
      CHECK(report.warnings() == 0);
      CHECK(errors.str() ==
         "lexical error at {1:5}: too many radix points in floating-point literal: \"10.10.10\".");
   }

   SECTION("Assignment with missing exponent") {
      auto const tokens = generate_tokens("x = 543e 87.", report);
      auto const expected_tokens = std::vector{
         token{
            token_kind::identifier,
            "x",
            source_coordinate{column_type{1}, line_type{1}},
            source_coordinate{column_type{2}, line_type{1}}
         },
         token{
            token_kind::assign,
            "=",
            source_coordinate{column_type{3}, line_type{1}},
            source_coordinate{column_type{4}, line_type{1}}
         },
         token{
            token_kind::exponent_lacking_digit,
            "543e",
            source_coordinate{column_type{5}, line_type{1}},
            source_coordinate{column_type{9}, line_type{1}}
         },
         token{
            token_kind::floating_literal,
            "87.",
            source_coordinate{column_type{10}, line_type{1}},
            source_coordinate{column_type{13}, line_type{1}}
         },
         token{
            token_kind::eof,
            "$",
            source_coordinate{column_type{13}, line_type{1}},
            source_coordinate{column_type{13}, line_type{1}}
         }
      };
      CHECK_EQUAL(tokens, expected_tokens);
      CHECK(report.errors() == 1);
      CHECK(report.warnings() == 0);
      CHECK(errors.str() ==
         "lexical error at {1:5}: floating-point exponent lacking digits: \"543e\".");
   }

   SECTION("Unterminated comment") {
      auto source = std::string{
         "int main/*()\n"
         "{\n"
         "   abacus;\n"
         "}\n"
      };

      auto const tokens = generate_tokens(source, report);
      auto const expected_tokens = std::vector{
         token{
            token_kind::int_,
            "int",
            source_coordinate{column_type{1}, line_type{1}},
            source_coordinate{column_type{4}, line_type{1}}
         },
         token{
            token_kind::identifier,
            "main",
            source_coordinate{column_type{5}, line_type{1}},
            source_coordinate{column_type{9}, line_type{1}}
         },
         token{
            token_kind::eof,
            "$",
            source_coordinate{column_type{1}, line_type{5}},
            source_coordinate{column_type{1}, line_type{5}}
         }
      };

      CHECK_EQUAL(tokens, expected_tokens);
      CHECK(report.errors() == 1);
      CHECK(report.warnings() == 0);
      CHECK(errors.str() == "lexical error at {1:9}: unterminated multi-line comment.");
   }
}
