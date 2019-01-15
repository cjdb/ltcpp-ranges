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
// WITHOUT WARRANTIES OR CONDITIONS OF ANY ltcpp::token_kind, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include <algorithm>
#include <cjdb/cctype/isalpha.hpp>
#include <cjdb/cctype/isdigit.hpp>
#include <gsl/gsl>
#include <iterator>
#include "ltcpp/lexer/token.hpp"
#include <string_view>
#include <unordered_map>

namespace {
   /// \brief
   /// \param lexeme
   /// \returns
   ///
   ltcpp::token_kind deduce_number_kind(std::string_view const lexeme) noexcept
   {
      auto result = ltcpp::token_kind::integral_literal;
      if (auto radix_point = std::count(begin(lexeme), end(lexeme), '.'); radix_point == 1 or
          lexeme.find('E') != std::string_view::npos or lexeme.find('e') != std::string_view::npos) {
         result = ltcpp::token_kind::floating_literal;
      }
      else if (radix_point > 1) {
         return ltcpp::token_kind::too_many_radix_points;
      }

      if (not cjdb::isdigit(lexeme.back()) and lexeme.back() != '.') {
         return ltcpp::token_kind::exponent_lacking_digit;
      }
      return result;
   }

   /// \brief
   /// \param lexeme
   /// \returns
   ///
   ltcpp::token_kind lookup_kind(std::string_view const lexeme) noexcept
   {
      Expects(not empty(lexeme));

      using ltcpp::token_kind;
      static auto lexeme_table = std::unordered_map<std::string_view, token_kind>{
         // symbols
         {"++", token_kind::increment},
         {"--", token_kind::decrement},
         // arithmetic
         {"+", token_kind::plus},
         {"-", token_kind::minus},
         {"*", token_kind::times},
         {"/", token_kind::divide},
         {"%", token_kind::modulo},
         // assignment
         {"=",  token_kind::assign},
         {"+=", token_kind::plus_eq},
         {"-=", token_kind::minus_eq},
         {"*=", token_kind::times_eq},
         {"/=", token_kind::divide_eq},
         {"%=", token_kind::modulo_eq},
         // separators
         {".", token_kind::dot},
         {",", token_kind::comma},
         {";", token_kind::semicolon},
         {"{", token_kind::brace_open},
         {"}", token_kind::brace_close},
         {"(", token_kind::paren_open},
         {")", token_kind::paren_close},
         {"[", token_kind::square_open},
         {"]", token_kind::square_close},
         // comparison operations
         {"==", token_kind::equal_to},
         {"!=", token_kind::not_equal_to},
         {"<",  token_kind::less},
         {"<=", token_kind::less_equal},
         {">=", token_kind::greater_equal},
         {">",  token_kind::greater},
         // logical operators
         {"and", token_kind::and_},
         {"or",  token_kind::or_},
         {"not", token_kind::not_},
         // boolean literals
         {"false", token_kind::boolean_literal},
         {"true",  token_kind::boolean_literal},
         // types
         {"bool",   token_kind::bool_},
         {"char",   token_kind::char_},
         {"double", token_kind::double_},
         {"int",    token_kind::int_},
         {"string", token_kind::string_},
         {"void",   token_kind::void_},
         // keywords
         {"break",    token_kind::break_},
         {"const",    token_kind::const_},
         {"continue", token_kind::continue_},
         {"for",      token_kind::for_},
         {"if",       token_kind::if_},
         {"return",   token_kind::return_},
         {"while",    token_kind::while_},
         // errors
         {"!", token_kind::unknown_token}
      };

      if (auto result = lexeme_table.find(lexeme); result != end(lexeme_table)) {
         return result->second;
      }
      else if (auto const front = lexeme.front(); cjdb::isalpha(front) or front == '_') {
         return ltcpp::token_kind::identifier;
      }
      else if (cjdb::isdigit(front) or front == '.') {
         return deduce_number_kind(lexeme);
      }
      else {
         return ltcpp::token_kind::unknown_token;
      }
   }
} // namespace

namespace ltcpp {
   /// \brief Given a lexeme and a source coordinate, generates a token.
   /// \param lexeme A UTF-8 string that forms a valid sequence of characters.
   /// \param cursor The position in the source file where the lexeme begins.
   /// \param kind
   /// \returns A token, based on the lexeme provided.
   ///
   token make_token(std::string lexeme, source_coordinate const cursor_begin, token_kind const kind)
   noexcept
   {
      auto const cursor_end = source_coordinate{
         source_coordinate::line_type{0}, // lexemes don't span multiple lines
         source_coordinate::column_type{std::distance(begin(lexeme), end(lexeme))}
      };
      return token{
         kind,
         std::move(lexeme),
         cursor_begin,
         source_coordinate::shift(cursor_begin, cursor_end)
      };
   }
   /// \brief Given a lexeme and a source coordinate, generates a token.
   /// \param lexeme A UTF-8 string that forms a valid sequence of characters.
   /// \param cursor The position in the source file where the lexeme begins.
   /// \returns A token, based on the lexeme provided.
   ///
   token make_token(std::string lexeme, source_coordinate const cursor_begin) noexcept
   {
      return make_token(std::move(lexeme), cursor_begin, lookup_kind(lexeme));
   }
} // namespace ltcpp
