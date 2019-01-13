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
build_library("${prefix}" lexer range-v3)
build_library("${prefix}" scan_whitespace range-v3)
build_library("${prefix}" scan_string_literal range-v3)
build_library("${prefix}" scan_symbol range-v3)
build_library("${prefix}" scan_identifier range-v3)
build_library("${prefix}" scan_number range-v3)
build_library("${prefix}" token range-v3)
