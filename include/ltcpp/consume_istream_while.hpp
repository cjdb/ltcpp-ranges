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
#include <istream>
#include <range/v3/istream_range.hpp>
#include <range/v3/view/take_while.hpp>
#include <range/v3/to_container.hpp>
#include <string>
#include <utility>

namespace ltcpp {
   namespace detail_consume_istream_while_fn {
      struct consume_istream_while_fn {
         template<class F>
         std::string operator()(std::istream& in, F pred) const
         {
            auto sequence = ranges::istream_range<char>(in)
                          | ranges::view::take_while(std::move(pred))
                          | ranges::to<std::string>();
            if (in or in.eof()) {
               auto const was_eof = in.eof();
               in.unget(); // take_while consumes an extra character
               if (was_eof) {
                  in.clear();
               }
            }
            return sequence;
         }
      };
   } // namespace detail_consume_istream_while_fn

   inline constexpr auto consume_istream_while =
      detail_consume_istream_while_fn::consume_istream_while_fn{};
} // namespace ltcpp
