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
#ifndef LTCPP_LEXER_TOKEN_HPP
#define LTCPP_LEXER_TOKEN_HPP

#include "ltcpp/source_coordinate.hpp"
#include <ostream>
#include <string>
#include <tuple>
#include <utility>

namespace ltcpp {
   enum class token_kind {
      // symbols
      increment,
      decrement,
      // arithmetic
      plus,
      minus,
      times,
      divide,
      modulo,
      // assignment
      assign,
      plus_eq,
      minus_eq,
      times_eq,
      divide_eq,
      modulo_eq,
      // separators
      dot,
      comma,
      semicolon,
      brace_open,
      brace_close,
      paren_open,
      paren_close,
      square_open,
      square_close,
      // comparison operations
      equal_to,
      not_equal_to,
      less,
      less_equal,
      greater_equal,
      greater,
      // logical operators
      and_,
      or_,
      not_,
      // literals
      integral_literal,
      boolean_literal,
      floating_literal,
      string_literal,
      // type specifiers
      bool_,
      char_,
      double_,
      int_,
      string_,
      void_,
      // keywords
      break_,
      const_,
      continue_,
      for_,
      if_,
      return_,
      while_,
      // other
      identifier,
      eof,
      // errors
      unknown_token,
      unterminated_string_literal,
      unterminated_comment,
      invalid_escape_sequence,
      too_many_radix_points,
      exponent_lacking_digit
   };

   struct [[nodiscard]] source_coordinate_range {
      source_coordinate begin;
      source_coordinate end;
   };

   class [[nodiscard]] token {
   public:
      token(token_kind const kind, std::string spelling, source_coordinate begin,
         source_coordinate end)
         : kind_{kind}
         , spelling_{std::move(spelling)}
         , position_{begin, end}
      {}

      token_kind kind() const noexcept
      { return kind_; }

      std::string_view spelling() const noexcept
      { return spelling_; }

      source_coordinate_range position() const noexcept
      { return position_; }

      friend bool operator==(token const& a, token const& b) noexcept
      {
         return std::tie(a.kind_, a.spelling_, a.position_.begin, a.position_.end)
             == std::tie(b.kind_, b.spelling_, b.position_.begin, b.position_.end);
      }

      friend std::ostream& operator<<(std::ostream& o, token const& t) noexcept
      {
         return o << '[' << static_cast<int>(t.kind()) << ", \"" << t.spelling() << "\", " <<
            t.position().begin << ".." << t.position().end << ']';
      }
   private:
      token_kind kind_;
      std::string spelling_;
      source_coordinate_range position_;
   };

   token make_token(std::string lexeme, source_coordinate cursor_begin) noexcept;
   token make_token(std::string lexeme, source_coordinate cursor_begin, token_kind kind) noexcept;
} // namespace ltcpp

#endif // LTCPP_LEXER_TOKEN_HPP
