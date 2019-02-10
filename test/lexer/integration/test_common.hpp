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
#ifndef TEST_LEXER_LEXER_TEST_COMMON_HPP
#define TEST_LEXER_LEXER_TEST_COMMON_HPP

#include "../../simple_test.hpp"
#include <cassert>
#include <ios>
#include <iostream>
#include <iterator>
#include "ltcpp/lexer/lexer.hpp"
#include "ltcpp/lexer/token.hpp"
#include "ltcpp/reporter.hpp"
#include "ltcpp/source_coordinate.hpp"
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#define CHECK_EQUAL(actual, expected) {                                                                   \
   if (size(actual) != size(expected)) {                                                                  \
      std::cerr << "Range size mismatch in CHECK_EQUAL at line " << __LINE__ << "\n"                      \
         "\n"                                                                                             \
         "actual == " << size(actual) << "\n"                                                             \
         "contents:\n";                                                                                   \
         std::copy(begin(actual), end(actual), std::ostream_iterator<ltcpp::token>{std::cerr, "\n"});     \
         std::cerr << "\n"                                                                                \
            "expected == " << size(expected) << "\n"                                                      \
            "contents:\n";                                                                                \
         std::copy(begin(expected), end(expected), std::ostream_iterator<ltcpp::token>{std::cerr, "\n"}); \
         assert(size(actual) == size(expected));                                                          \
   }                                                                                                      \
   for (auto i = 0U; i < size(actual); ++i) {                                                             \
      CHECK(actual[i] == expected[i]);                                                                    \
   }                                                                                                      \
}                                                                                                         \


inline std::vector<ltcpp::token> generate_tokens(std::string const& source, ltcpp::reporter& report)
{
   auto in = std::istringstream{source};
   in.unsetf(std::ios_base::skipws);

   auto tokens = std::vector<ltcpp::token>{};
   auto lexer = ltcpp::lexer_view{in, report};
   for (auto token : lexer) {
      tokens.push_back(std::move(token));
   }
   return tokens;
}

#endif // TEST_LEXER_LEXER_TEST_COMMON_HPP
