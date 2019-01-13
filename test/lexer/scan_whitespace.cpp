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
   CHECK(not result.second);                                                                       \
   CHECK(result.first == expected);                                                                \
}                                                                                                  \

template<class R>
auto distance(R&& r)
{
   return static_cast<std::intmax_t>(std::size(r));
}

#define CHECK_SCAN_WHITESPACE(spaces, column_delta, line_delta) {                                  \
   auto s = std::string(spaces);                                                                   \
   auto const delta_coordinate = source_coordinate{                                                \
      source_coordinate::column_type{column_delta},                                                \
      source_coordinate::line_type{line_delta}                                                     \
   };                                                                                              \
   constexpr auto default_whitespace = source_coordinate{};                                        \
   CHECK_SCAN_WHITESPACE_IMPL(s, default_whitespace,                                               \
      source_coordinate::shift(default_whitespace, delta_coordinate));                             \
   constexpr auto non_default_whitespace = source_coordinate{                                      \
      source_coordinate::column_type{42},                                                          \
      source_coordinate::line_type{38}                                                             \
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

   CHECK_SCAN_WHITESPACE("", column_type{0}, line_type{0});
   SECTION("Scan spaces") {
      CHECK_SCAN_WHITESPACE(" ", column_type{1}, line_type{0});
      CHECK_SCAN_WHITESPACE("  ", column_type{2}, line_type{0});
      CHECK_SCAN_WHITESPACE("  a", column_type{2}, line_type{0});
   }
   SECTION("Scan tabs") {
      CHECK_SCAN_WHITESPACE("\t", column_type{1}, line_type{0});
      CHECK_SCAN_WHITESPACE("\t\t", column_type{2}, line_type{0});
      CHECK_SCAN_WHITESPACE("\t\tthough\tful", column_type{2}, line_type{0});
   }
   SECTION("Scan new lines") {
      CHECK_SCAN_WHITESPACE("\n", column_type{0}, line_type{1});
      CHECK_SCAN_WHITESPACE("\n\n", column_type{0}, line_type{2});
      CHECK_SCAN_WHITESPACE("\n\ncare bear cou\n\tdown", column_type{0}, line_type{2});

      CHECK_SCAN_WHITESPACE("\r\nelectrode diglett \nido\ran mankey", column_type{0}, line_type{1});
      CHECK_SCAN_WHITESPACE("\r\n\r\n", column_type{0}, line_type{2});
      CHECK_SCAN_WHITESPACE("\r\n\n", column_type{0}, line_type{2});
      CHECK_SCAN_WHITESPACE("\n\r\n", column_type{0}, line_type{2});
      CHECK_SCAN_WHITESPACE("\n\r", column_type{0}, line_type{1});
      CHECK_SCAN_WHITESPACE("\n\r\n\rthe\re's a s\nake in my boo\t!", column_type{0}, line_type{2});
   }
   SECTION("Scan carriage returns") {
      CHECK_SCAN_WHITESPACE("\r", column_type{1}, line_type{0});
      CHECK_SCAN_WHITESPACE("\r\r", column_type{2}, line_type{0});
      CHECK_SCAN_WHITESPACE("\r\rregiice \regirock \registeel", column_type{2}, line_type{0});
   }
   SECTION("Scan form feeds") {
      CHECK_SCAN_WHITESPACE("\f", column_type{0}, line_type{1});
      CHECK_SCAN_WHITESPACE("\f\f", column_type{0}, line_type{2});
      CHECK_SCAN_WHITESPACE("\f\f\n", column_type{0}, line_type{3});
      CHECK_SCAN_WHITESPACE("\f\f\r\n", column_type{0}, line_type{3});
      CHECK_SCAN_WHITESPACE("\f\f\n\r", column_type{0}, line_type{3});
      CHECK_SCAN_WHITESPACE("\f\f", column_type{0}, line_type{2});
      CHECK_SCAN_WHITESPACE("\flord \farquad", column_type{0}, line_type{1});
   }

   SECTION("Scan single-line comment") {
      CHECK_SCAN_WHITESPACE("//", column_type{2}, line_type{0});

      constexpr auto hello_world = "// hello world!"sv;
      CHECK_SCAN_WHITESPACE(hello_world, column_type{distance(hello_world)}, line_type{0});

      constexpr auto hello_world_lf = "// hello world\n"sv;
      CHECK_SCAN_WHITESPACE(hello_world_lf, column_type{0}, line_type{1});

      constexpr auto hello_lf_world_crlf = "// hello\n"
                                           "//\tworld\r\n"sv;
      CHECK_SCAN_WHITESPACE(hello_lf_world_crlf, column_type{0}, line_type{2});
   }
   SECTION("Scan multi-line comment") {
      SECTION("Well-formed comments") {
         constexpr auto empty = "/**/"sv;
         CHECK_SCAN_WHITESPACE(empty, column_type{distance(empty)}, line_type{0});
         CHECK_SCAN_WHITESPACE("/**/alakazam", column_type{distance(empty)}, line_type{0});

         constexpr auto single_line = "/* alakazam */"sv;
         CHECK_SCAN_WHITESPACE(single_line, column_type{distance(single_line)}, line_type{0});

         constexpr auto multi_line = "/**\n"
                                    " * \\brief This is a multi-line comment\r\n"
                                    " */"sv;
         CHECK_SCAN_WHITESPACE(multi_line, column_type{3}, line_type{2});

         constexpr auto fake_end = "/*/ hahaha */"sv;
         CHECK_SCAN_WHITESPACE(fake_end, column_type{distance(fake_end)}, line_type{0});
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
         CHECK(result.second);
         CHECK(result.first == source_coordinate{column_type{4}, line_type{2}});
      }
   }
}
