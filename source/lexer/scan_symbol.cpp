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
#include "ltcpp/lexer/detail/scan_symbol.hpp"
#include "ltcpp/lexer/token.hpp"
#include <istream>
#include <string>

#include "ltcpp/utility/peek.hpp"

namespace {
   using ltcpp::token, ltcpp::source_coordinate, ltcpp::make_token, ltcpp::peek;

   /// \brief Checks if a character is the beginning of a multi-character token.
   ///
   token possibly_token(std::istream& in, char const current, source_coordinate const cursor,
      std::string const& candidates) noexcept(false)
   {
      if (auto next = peek(in); next and candidates.find(*next) != std::string::npos) {
         in.get(*next);
         return make_token(std::string{current, *next}, cursor);
      }
      else {
         return make_token(std::string{current}, cursor);
      }
   }
} // namespace

namespace ltcpp::detail_lexer {
   /// \brief
   ///
   token scan_symbol(std::istream& in, source_coordinate const cursor) noexcept(false)
   {
      auto current = '\0';
      in.get(current);
      switch (current) {
      case '-':
         return ::possibly_token(in, current, cursor, {">"});
      case '<':
         return ::possibly_token(in, current, cursor, {'=', '-'});
      case '>': [[fallthrough]];
      case '!':
         return ::possibly_token(in, current, cursor, {'='});
      default:
         return ::ltcpp::make_token(std::string{current}, cursor);
      }
   }
} // namespace ltcpp::detail_lexer
