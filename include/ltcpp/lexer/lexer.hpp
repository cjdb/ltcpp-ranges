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
#ifndef LTCPP_LEXER_LEXER_HPP
#define LTCPP_LEXER_LEXER_HPP

#include <istream>
#include "ltcpp/lexer/token.hpp"
#include "ltcpp/reporter.hpp"
#include "ltcpp/source_coordinate.hpp"

namespace ltcpp {
   /// \brief
   /// \param in
   /// \param report
   /// \param cursor
   /// \returns
   /// \throws std::runtime_error if `in.fail() and not in.eof()`.
   ///
   token
   generate_token(std::istream& in, reporter& report, source_coordinate cursor) noexcept(false);

   class lexer_view {
      class default_sentinel_t {}; // public-facing, can't use ranges...
      class [[nodiscard]] lexer_iterator {
      public:
         using difference_type = std::intmax_t;

         lexer_iterator() = default;

         explicit lexer_iterator(std::istream* in, reporter* report) noexcept
            : in_{in}
            , report_{report}
         {}

         token operator*() noexcept(false)
         {
            auto result = ltcpp::generate_token(*in_, *report_, cursor_);
            cursor_ = result.cursor_range().end;
            return result;
         }

         lexer_iterator& operator++() noexcept
         { return *this; }

         lexer_iterator& operator++(int) noexcept
         { return *this; }

         friend bool operator==(lexer_iterator const& i, default_sentinel_t) noexcept
         { return not i.in_->good(); }

         friend bool operator==(default_sentinel_t s, lexer_iterator const& i) noexcept
         { return i == s; }

         friend bool operator!=(lexer_iterator const& i, default_sentinel_t s) noexcept
         { return not (i == s); }

         friend bool operator!=(default_sentinel_t s, lexer_iterator const& i) noexcept
         { return not (s == i); }
      private:
         std::istream* in_;
         reporter* report_;
         source_coordinate cursor_;
      };
   public:
      lexer_view() = default;

      explicit lexer_view(std::istream& in, reporter& report) noexcept
         : in_{std::addressof(in)}
         , report_{std::addressof(report)}
      { in.unsetf(std::ios_base::skipws); }

      lexer_iterator begin() noexcept
      { return lexer_iterator{in_, report_}; }

      default_sentinel_t end() noexcept
      { return {}; }
   private:
      std::istream* in_ = nullptr;
      reporter* report_ = nullptr;
   };
} // namespace ltcpp

#endif // LTCPP_LEXER_LEXER_HPP
