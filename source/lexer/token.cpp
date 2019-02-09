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
#include <cjdb/cctype/isalpha.hpp>
#include <cjdb/cctype/isdigit.hpp>
#include <gsl/gsl>
#include "ltcpp/lexer/token.hpp"
#include <range/v3/algorithm/count.hpp>
#include <range/v3/distance.hpp>
#include <string_view>
#include <unordered_map>

namespace {
   using namespace ranges;

   /// \brief
   /// \param lexeme
   /// \returns
   ///
   ltcpp::token_kind deduce_number_kind(std::string_view const lexeme) noexcept
   {
      auto result = ltcpp::token_kind::integral_literal;
      auto const has_exponent = [](auto const l) noexcept {
         return l.find('E') != std::string_view::npos or l.find('e') != std::string_view::npos;
      };
      if (auto radix_point = count(lexeme, '.'); radix_point == 1 or has_exponent(lexeme)) {
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
         // arithmetic
         {"+", token_kind::plus},
         {"-", token_kind::minus},
         {"*", token_kind::times},
         {"/", token_kind::divide},
         {"%", token_kind::modulo},
         // assignment
         {"<-", token_kind::assign},
         // separators
         {".", token_kind::dot},
         {",", token_kind::comma},
         {":", token_kind::colon},
         {";", token_kind::semicolon},
         {"{", token_kind::brace_open},
         {"}", token_kind::brace_close},
         {"(", token_kind::paren_open},
         {")", token_kind::paren_close},
         {"[", token_kind::square_open},
         {"]", token_kind::square_close},
         {"->", token_kind::arrow},
         // comparison operations
         {"=", token_kind::equal_to},
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
         {"bool",    token_kind::bool_},
         {"char8",   token_kind::char8},
         {"float16", token_kind::float16},
         {"float32", token_kind::float32},
         {"float64", token_kind::float64},
         {"int8",    token_kind::int8},
         {"int16",   token_kind::int16},
         {"int32",   token_kind::int32},
         {"int64",   token_kind::int64},
         {"void",    token_kind::void_},
         // keywords
         {"assert",   token_kind::assert_},
         {"break",    token_kind::break_},
         {"continue", token_kind::continue_},
         {"for",      token_kind::for_},
         {"fun",      token_kind::fun_},
         {"if",       token_kind::if_},
         {"import",   token_kind::import_},
         {"let",      token_kind::let_},
         {"module",   token_kind::module_},
         {"mutable",  token_kind::mutable_},
         {"readable", token_kind::readable_},
         {"ref",      token_kind::ref_},
         {"return",   token_kind::return_},
         {"while",    token_kind::while_},
         {"writable", token_kind::writable_},
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
         source_coordinate::column_type{ranges::distance(lexeme)}
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
