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
#include <cctype>
#include <istream>
#include "cjdb/cctype/isalnum.hpp"
#include "ltcpp/lexer/detail/scan_identifier.hpp"
#include "ltcpp/lexer/token.hpp"
#include "ltcpp/source_coordinate.hpp"
#include "ltcpp/consume_istream_while.hpp"
#include <utility>

namespace ltcpp::detail_lexer {
   /// \brief
   /// \param in
   /// \param cursor The position in the source file where the identifier begins.
   /// \returns
   ///
   token scan_identifier(std::istream& in, source_coordinate const cursor) noexcept
   {
      constexpr auto is_identifier_char = [](char const x) constexpr noexcept {
         return cjdb::isalnum(x) or x == '_';
      };
      auto identifier = ltcpp::consume_istream_while(in, is_identifier_char);
      return ltcpp::make_token(std::move(identifier), cursor);
   }
} // namespace ltcpp::detail_lexer
