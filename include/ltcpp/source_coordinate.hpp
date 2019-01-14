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
#ifndef LTCPP_LEXER_SOURCE_COORDINATE_HPP
#define LTCPP_LEXER_SOURCE_COORDINATE_HPP

#include <cstdint>
#include <cjdb/utility/strong_value.hpp>
#include <ostream>
#include <tuple>

namespace ltcpp {
   class source_coordinate {
      struct column_tag {};
      struct line_tag {};
   public:
      using value_type = std::intmax_t;
      using column_type = cjdb::strong_value<value_type, column_tag>;
      using line_type = cjdb::strong_value<value_type, line_tag>;

      /// \brief Initialises the column and lines so that they are equivalent to the value 1.
      ///
      constexpr source_coordinate() = default;

      /// \brief Initialises the column value with the column parameter and initialises the line
      ///        value with the line parameter.
      ///
      constexpr explicit source_coordinate(line_type const line, column_type const column) noexcept
         : line_{line}
         , column_{column}
      {}

      /// \brief Returns the column value.
      ///
      constexpr column_type column() const noexcept
      { return column_; }

      /// \brief Returns the line value.
      ///
      constexpr line_type line() const noexcept
      { return line_; }

      /// \brief Checks that the column and line values of x are the same as the column and line
      ///        values of y.
      ///
      constexpr friend bool operator==(source_coordinate const x, source_coordinate const y) noexcept
      { return std::tie(x.column_, x.line_) == std::tie(y.column_, y.line_); }

      /// \brief Checks that the column and line values of x are not the same as the column and line
      ///        values of y.
      ///
      constexpr friend bool operator!=(source_coordinate const x, source_coordinate const y) noexcept
      { return not (x == y); }

      /// \brief Writes the value of a column object to an ostream object.
      ///
      /// The result is equivalent to writing an integer with the same value to the ostream object.
      /// \returns o
      ///
      friend std::ostream& operator<<(std::ostream& o, column_type const x)
      { return o << static_cast<std::intmax_t>(x); }

      /// \brief Writes the value of a line object to an ostream object.
      ///
      /// The result is equivalent to writing an integer with the same value to the ostream object.
      /// \returns o
      ///
      friend std::ostream& operator<<(std::ostream& o, line_type const x)
      { return o << static_cast<std::intmax_t>(x); }

      /// \brief Writes the value of a source_coordinate object to an ostream object.
      ///
      /// The format is "{<line>:<column>}"
      /// \returns o
      ///
      friend std::ostream& operator<<(std::ostream& o, source_coordinate const x)
      { return o << '{' << x.line() << ":" << x.column() << '}'; }

      /// \brief Moves the x by:
      ///       1. adding y.line() to x.line(), and
      ///       2. (a) adding y.column() to x.column() if y.line() == 0, or
      ///          (b) assigning y.column() to x.column() otherwise
      /// \returns x
      ///
      static constexpr
      source_coordinate shift(source_coordinate const x, source_coordinate const y) noexcept
      {
         return source_coordinate{
            line_type{x.line() + y.line()},
            column_type{
                 [&x, &y]{
                    if (y.line() == line_type{0})
                       return x.column() + y.column();
                    else if (y.column() > column_type{0})
                       return y.column();
                    else
                       return column_type{1};
                 }()
            }
         };
      }
   private:
      line_type line_{1};
      column_type column_{1};
   };
} // namespace ltcpp

#endif // LTCPP_LEXER_SOURCE_COORDINATE_HPP
