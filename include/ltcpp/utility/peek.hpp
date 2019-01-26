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
#ifndef LTCPP_UTILITY_PEEK_HPP
#define LTCPP_UTILITY_PEEK_HPP

#include <istream>
#include <optional>

namespace ltcpp {
   namespace detail_peek {
      struct peek_fn {
         template<class CharT, class Traits>
         std::optional<CharT> operator()(std::basic_istream<CharT, Traits>& in) const
         {
            if (in) {
               if (auto c = in.peek(); in.good()) {
                  return static_cast<CharT>(c);
               }
               else {
                  in.clear();
               }
            }

            return {};
         }
      };
   } // namespace detail_peek

   inline constexpr auto peek = detail_peek::peek_fn{};
} // namespace ltcpp

#endif // LTCPP_UTILITY_PEEK_HPP
