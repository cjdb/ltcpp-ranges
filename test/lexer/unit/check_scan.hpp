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
#ifndef TEST_LEXER_CHECK_SCAN_HPP
#define TEST_LEXER_CHECK_SCAN_HPP

#include "ltcpp/lexer/token.hpp"
#include "ltcpp/source_coordinate.hpp"
#include "../../simple_test.hpp"
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>

#define CHECK_SCAN(f, spelling_, expected_kind, expected_spelling_) {                              \
   using ltcpp::source_coordinate;                                                                 \
   auto spelling = std::string(spelling_);                                                         \
   auto in = std::istringstream{spelling};                                                         \
   in.unsetf(std::ios_base::skipws);                                                               \
                                                                                                   \
   auto const result = f(in, source_coordinate{});                                                 \
   CHECK(result.kind() == expected_kind);                                                          \
                                                                                                   \
   auto expected_spelling = std::string_view{expected_spelling_ == "" ? spelling : expected_spelling_}; \
   CHECK(result.spelling() == expected_spelling);                                                  \
                                                                                                   \
   auto const begin = result.cursor_range().begin();                                               \
   auto const end = result.cursor_range().end();                                                   \
   CHECK(begin.line() == end.line());                                                              \
                                                                                                   \
   auto const column_distance =                                                                    \
      static_cast<std::intmax_t>(end.column()) - static_cast<std::intmax_t>(begin.column());       \
   CHECK(column_distance == static_cast<std::intmax_t>(size(expected_spelling)));                  \
}                                                                                                  \


#endif // TEST_LEXER_CHECK_SCAN_HPP
