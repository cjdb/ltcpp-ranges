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

# \brief Provides build settings common for all targets.
# \param target The target to apply the target to.
#
function(build_impl target libraries)
   target_compile_options("${target}" PRIVATE
      $<$<CXX_COMPILER_ID:MSVC>:
         /W4 /WX
         /w14242
         /w14254
         /w14263
         /w14265
         /w14287
         /we4289
         /w14296
         /w14311
         /w14545
         /w14546
         /w14547
         /w14549
         /w14555
         /w14619
         /w14640
         /w14826
         /w14905
         /w14906
         /w14928
         /permissive->
      $<$<CXX_COMPILER_ID:GNU>:
         -Wall -Wextra -Werror
         -Wcast-align
         -Wconversion
         -Wduplicated-cond
         -Wduplicated-branches
         -Wdouble-promotion
         -Wlogical-op
         -Wnon-virtual-dtor
         -Wnull-dereference
         -Wold-style-cast
         -Woverloaded-virtual
         -Wpedantic
         -Wshadow
         -Wsign-conversion
         -Wsign-promo
         -Wmisleading-indentation
         -Wunused
         -Wuseless-cast
         -Wformat=2
         -Wodr
         -Wno-attributes>
      $<$<CXX_COMPILER_ID:Clang>:
         -Weverything -Werror
         -Wno-c++98-compat
         -Wno-c++98-compat-pedantic
         -Wno-padded>)

   target_compile_options("${target}" PRIVATE
      $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
         $<$<CONFIG:Debug>:
            -fstack-protector-all>
         -fdiagnostics-color=always
         -fconcepts>)

   target_include_directories("${target}" PUBLIC "${PROJECT_SOURCE_DIR}/include")

   foreach(i ${libraries})
      message(STATUS "Linking ${i} with ${target}")
      target_link_libraries("${target}" PRIVATE ${i})
   endforeach()
   add_compile_options(-DRANGES_DEEP_STL_INTEGRATION=1)
endfunction()

# \brief Produces a target name for compiling a translation unit.
# \param prefix A string that prefixes the filename that will be removed from its path. Everything
#               that prefixes the prefix will _also_ be removed.
# \param file The name of the source file.
# \returns A sans-prefix path that is dot separated.
#
function(name_target prefix file)
   get_filename_component(sublibrary "${file}.cpp" ABSOLUTE)
   string(REGEX REPLACE ".cpp$" "" sublibrary "${sublibrary}")
   string(REPLACE "/" "." sublibrary "${sublibrary}")
   string(REGEX REPLACE "^.*${prefix}[.]" "" sublibrary "${sublibrary}")
   set(target "${sublibrary}" PARENT_SCOPE)
endfunction()

# \brief Builds an executable.
# \param prefix A string that prefixes the filename that will be removed from its path. Everything
#               that prefixes the prefix will _also_ be removed.
# \param file The name of the source file.
#
function(build_executable prefix file)
   name_target("${prefix}" "${file}")
   add_executable("${target}" "${file}.cpp")

   set(libraries "")
   if(${ARGC} GREATER 2)
      set(libraries ${ARGV})
      list(SUBLIST libraries 2 ${ARGC} libraries)
   endif()
   build_impl("${target}" "${libraries}")
endfunction()

# \brief Builds a library.
# \param prefix A string that prefixes the filename that will be removed from its path. Everything
#               that prefixes the prefix will _also_ be removed.
# \param file The name of the source file.
#
function(build_library prefix file)
   name_target("${prefix}" "${file}")
   add_library("${target}" "${file}.cpp")

   set(libraries "")
   if(${ARGC} GREATER 2)
      set(libraries ${ARGV})
      list(SUBLIST libraries 2 ${ARGC} libraries)
   endif()
   build_impl("${target}" "${libraries}")
endfunction()

set(enable_catch2 On)
set(disable_catch2 Off)

# \brief Builds a test executable and creates a test target (for CTest).
# \param prefix A string that prefixes the filename that will be removed from its path. Everything
#               that prefixes the prefix will _also_ be removed.
# \param file The name of the source file.
# \param catch2_enabled A boolean to determine if Catch2 should be linked or not.
#
function(build_test prefix file catch2_enabled)
   name_target("${prefix}" "${file}")
   name_target("${prefix}" "${file}")
   add_executable("${target}" "${file}.cpp")

   set(libraries "")
   if(${ARGC} GREATER 3)
      set(libraries ${ARGV})
      list(SUBLIST libraries 3 ${ARGC} libraries)
   endif()
   build_impl("${target}" "${libraries}")

   if(catch2_enabled)
      target_link_libraries("${target}" PRIVATE "ltcpp.test.catch2_main")
   endif()

   add_test("test.${target}" "${target}")
endfunction()

# \brief Builds a benchmark executable and creates a test target (for CTest).
# \param prefix A string that prefixes the filename that will be removed from its path. Everything
#               that prefixes the prefix will _also_ be removed.
# \param file The name of the source file.
# \param catch2_enabled A boolean to determine if Catch2 should be linked or not.
#
function(build_benchmark prefix file)
   build_executable(${ARGV})
   name_target("${prefix}" "${file}")
   target_link_libraries("${target}" PRIVATE benchmark::benchmark)
   add_test("benchmark.${target}" "${target}")
endfunction()
