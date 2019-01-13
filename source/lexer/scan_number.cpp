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
#include <cctype>
#include <cjdb/cctype/isdigit.hpp>
#include <istream>
#include "ltcpp/lexer/detail/scan_number.hpp"
#include "ltcpp/lexer/token.hpp"
#include "ltcpp/consume_istream_while.hpp"
#include <string>

namespace {
   /// \brief
   /// \param in
   /// \param cursor The position in the source file where the identifier begins.
   /// \returns
   ///
   std::string scan_floating_exponent(std::istream& in) noexcept(false)
   {
      if (auto c = in.peek(); std::toupper(c) == 'E') {
         auto exponent = std::string{static_cast<char>(in.get())};

         // an exponent may have one '+' or '-' after the 'E'
         if (c = in.peek(); c == '+' or c == '-') {
            exponent += static_cast<char>(in.get());
         }

         exponent += ltcpp::consume_istream_while(in, cjdb::isdigit);
         return exponent;
      }

      return {};
   }

   /// \brief
   /// \param in
   /// \param cursor The position in the source file where the identifier begins.
   /// \returns
   ///
   ltcpp::token scan_floating(std::istream& in, std::string&& number,
      ltcpp::source_coordinate const cursor) noexcept(false)
   {
      constexpr auto is_floating_point_token = [](auto const x) constexpr noexcept {
         return cjdb::isdigit(x) or x == '.';
      };
      number += ltcpp::consume_istream_while(in, is_floating_point_token);
      number += ::scan_floating_exponent(in);
      return ::ltcpp::make_token(std::move(number), cursor);
   }
} // namespace

namespace ltcpp::detail_lexer {
   /// \brief
   /// \param in
   /// \param cursor The position in the source file where the identifier begins.
   /// \returns
   ///
   token scan_number(std::istream& in, source_coordinate const cursor) noexcept
   {
      auto number = ltcpp::consume_istream_while(in, cjdb::isdigit);
      if (auto peek = in.peek(); peek == '.' or peek == 'e' or peek == 'E') {
         return ::scan_floating(in, std::move(number), cursor);
      }
      else {
         return ::ltcpp::make_token(std::move(number), cursor);
      }
   }

   /// \brief Checks if a '.' forms a floating-point number or is indeed a dot.
   ///
   token possibly_float(std::istream& in, source_coordinate const cursor) noexcept
   {
      auto dot = static_cast<char>(in.get());
      if (in and std::isdigit(in.peek())) {
         return ::scan_floating(in, std::string{dot}, cursor);
      }
      else {
         return ::ltcpp::make_token(std::string{dot}, cursor);
      }
   }
} // namespace ltcpp::detail_lexer
