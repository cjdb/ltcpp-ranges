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
#ifndef LTCPP_LEXER_SOURCE_COORDINATE_RANGE_HPP
#define LTCPP_LEXER_SOURCE_COORDINATE_RANGE_HPP

#include "ltcpp/source_coordinate.hpp"

#include <tuple>

namespace ltcpp {
   class [[nodiscard]] source_coordinate_range {
   public:
      explicit constexpr source_coordinate_range(source_coordinate begin, source_coordinate end)
         : begin_(begin)
         , end_(end)
      {}

      constexpr source_coordinate begin() const noexcept
      { return begin_; }

      constexpr source_coordinate end() const noexcept
      { return end_; }

      template<class CharT, class Traits>
      friend std::basic_ostream<CharT, Traits>
      operator<<(std::basic_ostream<CharT, Traits>& o, source_coordinate_range const& r)
      {
         return o << "from " << r.begin() << " to " << r.end();
      }

      friend constexpr bool
      operator==(source_coordinate_range const& x, source_coordinate_range const& y) noexcept
      { return std::tie(x.begin_, x.end_) == std::tie(y.begin_, y.end_); }

      friend constexpr bool
      operator!=(source_coordinate_range const& x, source_coordinate_range const& y) noexcept
      { return not (x == y); }
   protected:
      constexpr void begin(source_coordinate x) noexcept
      { begin_ = x; }

      constexpr void end(source_coordinate x) noexcept
      { end_ = x; }
   private:
      source_coordinate begin_;
      source_coordinate end_;
   };
} // namespace ltcpp

#endif // LTCPP_LEXER_SOURCE_COORDINATE_RANGE_HPP
