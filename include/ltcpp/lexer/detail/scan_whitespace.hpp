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
#ifndef LTCPP_LEXER_DETAIL_SCAN_WHITESPACE_HPP
#define LTCPP_LEXER_DETAIL_SCAN_WHITESPACE_HPP

#include "ltcpp/lexer/token.hpp"
#include "ltcpp/source_coordinate.hpp"
#include "ltcpp/source_coordinate_range.hpp"
#include <istream>
#include <utility>

namespace ltcpp::detail_lexer {
   class [[nodiscard]] unterminated_comment_error : public source_coordinate_range {
   public:
      using source_coordinate_range::source_coordinate_range;
      using source_coordinate_range::begin;
      using source_coordinate_range::end;

      friend constexpr bool
      operator==(unterminated_comment_error const& x, unterminated_comment_error const& y) noexcept
      {
         return static_cast<source_coordinate_range const&>(x)
             == static_cast<source_coordinate_range const&>(y);
      }

      friend constexpr bool
      operator!=(unterminated_comment_error const& x, unterminated_comment_error const& y) noexcept
      { return not (x == y); }
   };

   tl::expected<source_coordinate, unterminated_comment_error>
   scan_whitespace_like(std::istream& in, source_coordinate cursor) noexcept;
} // namespace ltcpp::detail_lexer

#endif // LTCPP_LEXER_DETAIL_SCAN_WHITESPACE_HPP
