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

   template<class CharT, class Traits>
   std::basic_ostream<CharT, Traits>&
   operator<<(std::basic_ostream<CharT, Traits>& o, token_kind const kind) noexcept
   {
      switch (kind) {
      case token_kind::increment:
         return o << "++";
      case token_kind::decrement:
         return o << "--";
      case token_kind::plus:
         return o << "+";
      case token_kind::minus:
         return o << "-";
      case token_kind::times:
         return o << "*";
      case token_kind::divide:
         return o << "/";
      case token_kind::modulo:
         return o << "%";
      case token_kind::assign:
         return o << "=";
      case token_kind::plus_eq:
         return o << "+=";
      case token_kind::minus_eq:
         return o << "-=";
      case token_kind::times_eq:
         return o << "*=";
      case token_kind::divide_eq:
         return o << "/=";
      case token_kind::modulo_eq:
         return o << "%=";
      case token_kind::dot:
         return o << ".";
      case token_kind::comma:
         return o << ",";
      case token_kind::semicolon:
         return o << ";";
      case token_kind::brace_open:
         return o << "{";
      case token_kind::brace_close:
         return o << "}";
      case token_kind::paren_open:
         return o << "(";
      case token_kind::paren_close:
         return o << ")";
      case token_kind::square_open:
         return o << "[";
      case token_kind::square_close:
         return o << "]";
      case token_kind::equal_to:
         return o << "==";
      case token_kind::not_equal_to:
         return o << "!=";
      case token_kind::less:
         return o << "<";
      case token_kind::less_equal:
         return o << "<=";
      case token_kind::greater_equal:
         return o << ">=";
      case token_kind::greater:
         return o << ">";
      case token_kind::and_:
         return o << "and";
      case token_kind::or_:
         return o << "or";
      case token_kind::not_:
         return o << "not";
      case token_kind::integral_literal:
         return o << "integral literal";
      case token_kind::boolean_literal:
         return o << "boolean literal";
      case token_kind::floating_literal:
         return o << "floating-point literal";
      case token_kind::string_literal:
         return o << "string literal";
      case token_kind::bool_:
         return o << "bool";
      case token_kind::char_:
         return o << "char";
      case token_kind::double_:
         return o << "double";
      case token_kind::int_:
         return o << "int";
      case token_kind::string_:
         return o << "string";
      case token_kind::void_:
         return o << "void";
      case token_kind::break_:
         return o << "break";
      case token_kind::const_:
         return o << "const";
      case token_kind::continue_:
         return o << "continue";
      case token_kind::for_:
         return o << "for";
      case token_kind::if_:
         return o << "if";
      case token_kind::return_:
         return o << "return";
      case token_kind::while_:
         return o << "while";
      case token_kind::identifier:
         return o << "identifer";
      case token_kind::eof:
         return o << "(end-of-file)";
      case token_kind::unknown_token:
         return o << "(unknown-token error)";
      case token_kind::unterminated_string_literal:
         return o << "(unterminated-string-literal error)";
      case token_kind::unterminated_comment:
         return o << "(unterminated-comment error)";
      case token_kind::invalid_escape_sequence:
         return o << "(invalid-escape-sequence error)";
      case token_kind::too_many_radix_points:
         return o << "(too-many-radix-points error)";
      case token_kind::exponent_lacking_digit:
         return o << "(exponent-lacking-digit error)";
      default:
         return o << "(unknown-token-kind error)";
      }
   }

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
