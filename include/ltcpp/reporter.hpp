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
#ifndef LTCPP_REPORTER_HPP
#define LTCPP_REPORTER_HPP

#include "ltcpp/source_coordinate.hpp"
#include "ltcpp/source_coordinate_range.hpp"
#include <ostream>
#include <memory>

namespace ltcpp {
   enum class pass { lexical, syntax, semantic, optimisation, code_generation };
   inline std::ostream& operator<<(std::ostream& o, pass const tag)
   {
      switch (tag) {
      case pass::lexical:
         return o << "lexical";
      case pass::syntax:
         return o << "syntax";
      case pass::semantic:
         return o << "semantic";
      case pass::optimisation:
         return o << "optimisation";
      case pass::code_generation:
         return o << "code generation";
      default:
         return o << "(unhandled compiler pass)";
      }
   }

   class reporter {
   public:
      reporter(std::ostream& o) noexcept
         : out_{std::addressof(o)}
      {}

      template<class... Args>
      // requires (StreamInsertable<Args> and ...)
      void error(pass const tag, source_coordinate const cursor, Args&&... args) noexcept
      { report_impl(tag, "error", errors_, cursor, std::forward<Args>(args)...); }

      template<class... Args>
      // requires (StreamInsertable<Args> and ...)
      void error(pass const tag, source_coordinate_range const range, Args&&... args) noexcept
      { report_impl(tag, "error", errors_, range, std::forward<Args>(args)...); }

      template<class... Args>
      // requires (StreamInsertable<Args> and ...)
      void warning(pass const tag, source_coordinate const cursor, Args&&... args) noexcept
      { report_impl(tag, "warning", warnings_, cursor, std::forward<Args>(args)...); }

      template<class... Args>
      // requires (StreamInsertable<Args> and ...)
      void warning(pass const tag, source_coordinate_range const range, Args&&... args) noexcept
      { report_impl(tag, "warning", warnings_, range, std::forward<Args>(args)...); }

      std::intmax_t errors() const noexcept
      { return errors_; }

      std::intmax_t warnings() const noexcept
      { return warnings_; }
   private:
      std::ostream* out_;
      std::intmax_t errors_ = 0;
      std::intmax_t warnings_ = 0;
      bool warnings_as_errors = true;

      template<class... Args>
      void report_impl(pass const tag, std::string_view const report_type, std::intmax_t& count,
         source_coordinate const cursor, Args&&... args) noexcept
      {
         ++count;
         *out_ << tag << ' ' << report_type << " at " << cursor << ": ";
         (*out_ << ... << std::forward<Args>(args));
         *out_ << '\n';
      }

      template<class... Args>
      void report_impl(pass const tag, std::string_view const report_type, std::intmax_t& count,
         source_coordinate_range const range, Args&&... args) noexcept
      {
         ++count;
         *out_ << tag << ' ' << report_type << " from " << range.begin << " to " << range.end << ": ";
         (*out_ << ... << std::forward<Args>(args));
         *out_ << '\n';
      }
   };
} // namespace ltcpp

#endif // LTCPP_REPORTER_HPP
