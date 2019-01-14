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
#include "ltcpp/lexer/detail/scan_whitespace.hpp"
#include "ltcpp/lexer/token.hpp"

#include "./check_scan.hpp"
#include <catch2/catch.hpp>
#include <iterator>
#include <sstream>
#include <string>

#define CHECK_SCAN_WHITESPACE_IMPL(spaces, coord, expected) {                                      \
   auto in = std::istringstream{spaces};                                                           \
   in.unsetf(std::ios_base::skipws);                                                               \
   auto result = scan_whitespace_like(in, coord);                                                  \
   REQUIRE(result);                                                                                \
   CHECK(*result == expected);                                                                     \
}                                                                                                  \

template<class R>
auto distance(R&& r)
{
   return static_cast<std::intmax_t>(std::size(r));
}

#define CHECK_SCAN_WHITESPACE(spaces, line_delta, column_delta) {                                  \
   auto s = std::string(spaces);                                                                   \
   auto const delta_coordinate = source_coordinate{                                                \
      source_coordinate::line_type{line_delta},                                                    \
      source_coordinate::column_type{column_delta}                                                 \
   };                                                                                              \
   constexpr auto default_whitespace = source_coordinate{};                                        \
   CHECK_SCAN_WHITESPACE_IMPL(s, default_whitespace,                                               \
      source_coordinate::shift(default_whitespace, delta_coordinate));                             \
   constexpr auto non_default_whitespace = source_coordinate{                                      \
      source_coordinate::line_type{38},                                                            \
      source_coordinate::column_type{42}                                                           \
   };                                                                                              \
   CHECK_SCAN_WHITESPACE_IMPL(s, non_default_whitespace,                                           \
      source_coordinate::shift(non_default_whitespace, delta_coordinate));                         \
}                                                                                                  \


TEST_CASE("Check numbers can be correctly scanned in") {
   using ltcpp::source_coordinate;
   using ltcpp::detail_lexer::scan_whitespace_like;
   using column_type = source_coordinate::column_type;
   using line_type = source_coordinate::line_type;
   using namespace std::string_literals;
   using namespace std::string_view_literals;

   CHECK_SCAN_WHITESPACE("", line_type{0}, column_type{0});
   SECTION("Scan spaces") {
      CHECK_SCAN_WHITESPACE(" ", line_type{0}, column_type{1});
      CHECK_SCAN_WHITESPACE("  ", line_type{0}, column_type{2});
      CHECK_SCAN_WHITESPACE("  a", line_type{0}, column_type{2});
   }
   SECTION("Scan tabs") {
      CHECK_SCAN_WHITESPACE("\t", line_type{0}, column_type{1});
      CHECK_SCAN_WHITESPACE("\t\t", line_type{0}, column_type{2});
      CHECK_SCAN_WHITESPACE("\t\tthough\tful", line_type{0}, column_type{2});
   }
   SECTION("Scan new lines") {
      CHECK_SCAN_WHITESPACE("\n", line_type{1}, column_type{0});
      CHECK_SCAN_WHITESPACE("\n\n", line_type{2}, column_type{0});
      CHECK_SCAN_WHITESPACE("\n\ncare bear cou\n\tdown", line_type{2}, column_type{0});

      CHECK_SCAN_WHITESPACE("\r\nelectrode diglett \nido\ran mankey", line_type{1}, column_type{0});
      CHECK_SCAN_WHITESPACE("\r\n\r\n", line_type{2}, column_type{0});
      CHECK_SCAN_WHITESPACE("\r\n\n", line_type{2}, column_type{0});
      CHECK_SCAN_WHITESPACE("\n\r\n", line_type{2}, column_type{0});
      CHECK_SCAN_WHITESPACE("\n\r", line_type{1}, column_type{0});
      CHECK_SCAN_WHITESPACE("\n\r\n\rthe\re's a s\nake in my boo\t!", line_type{2}, column_type{0});
   }
   SECTION("Scan carriage returns") {
      CHECK_SCAN_WHITESPACE("\r", line_type{0}, column_type{1});
      CHECK_SCAN_WHITESPACE("\r\r", line_type{0}, column_type{2});
      CHECK_SCAN_WHITESPACE("\r\rregiice \regirock \registeel", line_type{0}, column_type{2});
   }
   SECTION("Scan form feeds") {
      CHECK_SCAN_WHITESPACE("\f", line_type{1}, column_type{0});
      CHECK_SCAN_WHITESPACE("\f\f", line_type{2}, column_type{0});
      CHECK_SCAN_WHITESPACE("\f\f\n", line_type{3}, column_type{0});
      CHECK_SCAN_WHITESPACE("\f\f\r\n", line_type{3}, column_type{0});
      CHECK_SCAN_WHITESPACE("\f\f\n\r", line_type{3}, column_type{0});
      CHECK_SCAN_WHITESPACE("\f\f", line_type{2}, column_type{0});
      CHECK_SCAN_WHITESPACE("\flord \farquad", line_type{1}, column_type{0});
   }

   SECTION("Scan single-line comment") {
      CHECK_SCAN_WHITESPACE("//", line_type{0}, column_type{2});

      constexpr auto hello_world = "// hello world!"sv;
      CHECK_SCAN_WHITESPACE(hello_world, line_type{0}, column_type{distance(hello_world)});

      constexpr auto hello_world_lf = "// hello world\n"sv;
      CHECK_SCAN_WHITESPACE(hello_world_lf, line_type{1}, column_type{0});

      constexpr auto hello_lf_world_crlf = "// hello\n"
                                           "//\tworld\r\n"sv;
      CHECK_SCAN_WHITESPACE(hello_lf_world_crlf, line_type{2}, column_type{0});
   }
   SECTION("Scan multi-line comment") {
      SECTION("Well-formed comments") {
         constexpr auto empty = "/**/"sv;
         CHECK_SCAN_WHITESPACE(empty, line_type{0}, column_type{distance(empty)});
         CHECK_SCAN_WHITESPACE("/**/alakazam", line_type{0}, column_type{distance(empty)});

         constexpr auto single_line = "/* alakazam */"sv;
         CHECK_SCAN_WHITESPACE(single_line, line_type{0}, column_type{distance(single_line)});

         constexpr auto multi_line = "/**\n"
                                    " * \\brief This is a multi-line comment\r\n"
                                    " */"sv;
         CHECK_SCAN_WHITESPACE(multi_line, line_type{2}, column_type{3});

         constexpr auto fake_end = "/*/ hahaha */"sv;
         CHECK_SCAN_WHITESPACE(fake_end, line_type{0}, column_type{distance(fake_end)});
      }
      SECTION("Unterminated comment") {
         auto comment = "\n"
                        "   /* this is the comment that never ends,\n"
                        "      it just goes on and on, my friend,\n"
                        "      some people started writing it,\n"
                        "      not knowing what it was,\n"
                        "      and they'll continue singing it, because...\n"s;
         auto in = std::istringstream{comment};
         in.unsetf(std::ios_base::skipws);
         auto result = scan_whitespace_like(in, source_coordinate{});
         REQUIRE(in.eof());
         REQUIRE(not result);
         CHECK(result.error().begin == source_coordinate{line_type{2}, column_type{4}});
         CHECK(result.error().end == source_coordinate{line_type{7}, column_type{1}});
      }
   }
}
