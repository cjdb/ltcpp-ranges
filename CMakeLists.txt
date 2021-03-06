#
#  Copyright Christopher Di Bella
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
cmake_minimum_required(VERSION 3.12)
project(ltcpp-ranges CXX)

add_library(ltcpp-ranges INTERFACE)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

option(CJDB_ENABLE_CLANG_TIDY "" OFF)

include("${CMAKE_BINARY_DIR}/conan_paths.cmake" OPTIONAL)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(MicrosoftGSL REQUIRED)
find_package(Sanitizer COMPONENTS address undefined REQUIRED)
find_package(range-v3 REQUIRED)

if (CJDB_ENABLE_CLANG_TIDY)
   find_package(Clang REQUIRED)
   set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
   set(CMAKE_CXX_CLANG_TIDY clang-tidy-7 -p=${CMAKE_BINARY_DIR})
endif()

add_compile_options(
   -Wall
   -Wextra
   -Wno-attributes
   -Wsign-promo
   -Woverloaded-virtual
   -Wnon-virtual-dtor
   -Wodr
   -Werror)

enable_testing()
include(CTest)
include(build)

include_directories(../../../ranges/include)
include_directories(../../../expected)

set(prefix "ltcpp-ranges")
add_subdirectory(source)
add_subdirectory(test)
