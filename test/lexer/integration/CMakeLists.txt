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
build_test(
   "${prefix}"
   line1-column1
   # PRIVATE_LIBRARIES
      source.lexer.lexer
      source.lexer.scan_identifier
      source.lexer.scan_number
      source.lexer.scan_string_literal
      source.lexer.scan_symbol
      source.lexer.scan_whitespace
      source.lexer.token)
build_test(
   "${prefix}"
   line2-column1
   # PRIVATE_LIBRARIES
      source.lexer.lexer
      source.lexer.scan_identifier
      source.lexer.scan_number
      source.lexer.scan_string_literal
      source.lexer.scan_symbol
      source.lexer.scan_whitespace
      source.lexer.token)
build_test(
   "${prefix}"
   radix-point-error
   # PRIVATE_LIBRARIES
      source.lexer.lexer
      source.lexer.scan_identifier
      source.lexer.scan_number
      source.lexer.scan_string_literal
      source.lexer.scan_symbol
      source.lexer.scan_whitespace
      source.lexer.token)
build_test(
   "${prefix}"
   missing-exponent-errors
   # PRIVATE_LIBRARIES
      source.lexer.lexer
      source.lexer.scan_identifier
      source.lexer.scan_number
      source.lexer.scan_string_literal
      source.lexer.scan_symbol
      source.lexer.scan_whitespace
      source.lexer.token)
build_test(
   "${prefix}"
   unterminated-comment
   # PRIVATE_LIBRARIES
      source.lexer.lexer
      source.lexer.scan_identifier
      source.lexer.scan_number
      source.lexer.scan_string_literal
      source.lexer.scan_symbol
      source.lexer.scan_whitespace
      source.lexer.token)
build_test(
   "${prefix}"
   unterminated-string
   # PRIVATE_LIBRARIES
      source.lexer.lexer
      source.lexer.scan_identifier
      source.lexer.scan_number
      source.lexer.scan_string_literal
      source.lexer.scan_symbol
      source.lexer.scan_whitespace
      source.lexer.token)
