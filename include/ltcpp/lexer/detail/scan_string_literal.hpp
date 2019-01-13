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
#ifndef LTCPP_LEXER_DETAIL_SCAN_STRING_LITERAL_HPP
#define LTCPP_LEXER_DETAIL_SCAN_STRING_LITERAL_HPP

#include "ltcpp/lexer/token.hpp"
#include "ltcpp/source_coordinate.hpp"
#include <istream>

namespace ltcpp::detail_lexer {
   token scan_string_literal(std::istream& in, source_coordinate cursor) noexcept;
} // namespace ltcpp::detail_lexer

#endif // LTCPP_LEXER_DETAIL_SCAN_STRING_LITERAL_HPP
