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
#ifndef LTCPP_IRREGULAR_TRANSFORM_HPP
#define LTCPP_IRREGULAR_TRANSFORM_HPP

#include <range/v3/view/for_each.hpp>
#include <range/v3/view/single.hpp>
#include <utility>

namespace ltcpp {
   inline auto irregular_transform = [](auto f){
      return ranges::view::for_each([f=std::move(f)](auto&& r){
         return ranges::view::single(f(std::forward<decltype(r)>(r)));
      });
   };
} // namespace ltcpp
#endif // LTCPP_IRREGULAR_TRANSFORM_HPP
