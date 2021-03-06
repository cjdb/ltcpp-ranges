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
#include "ltcpp/lexer/detail/scan_number.hpp"
#include "ltcpp/lexer/token.hpp"

#include "./check_scan.hpp"
#include "../../simple_test.hpp"
#include <string>
#include <string_view>

int main()
{
   // Check numbers can be correctly scanned in
   using ltcpp::token_kind;
   using ltcpp::detail_lexer::scan_number;
   using namespace std::string_literals;
   using namespace std::string_view_literals;

   { // Check integers
      CHECK_SCAN(scan_number, "1", token_kind::integral_literal, ""sv);
      CHECK_SCAN(scan_number, "20", token_kind::integral_literal, ""sv);
      CHECK_SCAN(scan_number, "98765432100123456789", token_kind::integral_literal, ""sv);
   }

   { // Check floating-point numbers
      CHECK_SCAN(scan_number, "1.", token_kind::floating_literal, ""sv);
      CHECK_SCAN(scan_number, "1.2", token_kind::floating_literal, ""sv);
      CHECK_SCAN(scan_number, "100e3", token_kind::floating_literal, ""sv);
      CHECK_SCAN(scan_number, "100E3", token_kind::floating_literal, ""sv);
      CHECK_SCAN(scan_number, "72e+4", token_kind::floating_literal, ""sv);
      CHECK_SCAN(scan_number, "72E+4", token_kind::floating_literal, ""sv);
      CHECK_SCAN(scan_number, "2e-6", token_kind::floating_literal, ""sv);
      CHECK_SCAN(scan_number, "2E-6", token_kind::floating_literal, ""sv);
      CHECK_SCAN(scan_number, "2.e-6", token_kind::floating_literal, ""sv);
      CHECK_SCAN(scan_number, "2.E-6", token_kind::floating_literal, ""sv);
      CHECK_SCAN(scan_number, "48.65e-9", token_kind::floating_literal, ""sv);
      CHECK_SCAN(scan_number, "48.65E+9", token_kind::floating_literal, ""sv);
      CHECK_SCAN(scan_number, ".65", token_kind::floating_literal, ""sv);
      CHECK_SCAN(scan_number, ".65e-0", token_kind::floating_literal, ""sv);
      CHECK_SCAN(scan_number, ".65e+8", token_kind::floating_literal, ""sv);
   }

   { // Check floating-point errors
      CHECK_SCAN(scan_number, "1.2.3", token_kind::too_many_radix_points, ""sv);
      CHECK_SCAN(scan_number, "1.e", token_kind::exponent_lacking_digit, ""sv);
      CHECK_SCAN(scan_number, "756E", token_kind::exponent_lacking_digit, ""sv);
   }

   return ::test_result();
}
