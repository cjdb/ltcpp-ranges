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

#include <cassert>
#include <istream>
#include "ltcpp/lexer/token.hpp"
#include "ltcpp/reporter.hpp"
#include "ltcpp/source_coordinate.hpp"
#include <optional>

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
   public:
      class sentinel {}; // can't use ranges::default_sentinel_t as it poisons benchmarks
      class [[nodiscard]] iterator {
      public:
         using difference_type = std::intmax_t;

         iterator() = default;

         explicit iterator(std::istream* in, reporter* report) noexcept
            : in_{in}
            , report_{report}
            , cache_{ltcpp::generate_token(*in_, *report_, source_coordinate{})}
         { assert(cache_); }

         token& operator*() noexcept
         {
            assert(cache_);
            return *cache_;
         }

         token* operator->() noexcept
         {
            assert(cache_);
            return std::addressof(*cache_);
         }

         iterator& operator++() noexcept(false)
         {
            if (cache_->kind() != token_kind::eof) {
               cache_ = ltcpp::generate_token(*in_, *report_, cache_->cursor_range().end());
               assert(cache_);
            }
            else {
               eof_seen_ = true;
            }
            return *this;
         }

         void operator++(int) noexcept(false)
         { operator++(); }

         friend bool operator==(iterator const& i, sentinel) noexcept
         { return not i.in_->good() and i.eof_seen_; }

         friend bool operator==(sentinel s, iterator const& i) noexcept
         { return i == s; }

         friend bool operator!=(iterator const& i, sentinel s) noexcept
         { return not (i == s); }

         friend bool operator!=(sentinel s, iterator const& i) noexcept
         { return not (s == i); }
      private:
         std::istream* in_;
         reporter* report_;
         std::optional<token> cache_;
         bool eof_seen_ = false;
      };

      lexer_view() = default;

      explicit lexer_view(std::istream& in, reporter& report) noexcept
         : in_{std::addressof(in)}
         , report_{std::addressof(report)}
      { in.unsetf(std::ios_base::skipws); }

      iterator begin() noexcept
      { return iterator{in_, report_}; }

      sentinel end() const noexcept
      { return {}; }
   private:
      std::istream* in_ = nullptr;
      reporter* report_ = nullptr;
   };
} // namespace ltcpp

#endif // LTCPP_LEXER_LEXER_HPP
