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
#ifndef LTCPP_LEXER_LEXER_HPP
#define LTCPP_LEXER_LEXER_HPP

#include <istream>
#include "ltcpp/lexer/token.hpp"
#include "ltcpp/reporter.hpp"
#include "ltcpp/source_coordinate.hpp"

namespace ltcpp {
   /// \brief
   /// \param in
   /// \param report
   /// \param cursor
   /// \returns
   /// \throws std::runtime_error if `in.fail() and not in.eof()`.
   ///
   token
   generate_token(std::istream& in, reporter& report, source_coordinate cursor) noexcept(false);
} // namespace ltcpp

#endif // LTCPP_LEXER_LEXER_HPP
