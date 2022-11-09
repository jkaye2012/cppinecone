#pragma once

#include <map>
#include <sstream>
#include <string>
#include <string_view>

#include "pinecone/domain/operation_type.hpp"

namespace pinecone::net
{
struct url_builder {
  explicit url_builder(std::string_view environment) noexcept
  {
    std::ostringstream oss;
    oss << "https://controller." << environment << ".pinecone.io";
    _prefix = oss.str();
  }

  [[nodiscard]] auto prefix() const noexcept -> std::string_view { return _prefix; }

  [[nodiscard]] auto build(domain::operation_type op) noexcept -> std::string const&
  {
    if (auto url = _urls.find(op); url != _urls.end()) {
      return url->second;
    }

    auto [url, _] = _urls.emplace(op, _prefix + domain::op_url_fragment(op));
    return url->second;
  }

 private:
  std::string _prefix;
  std::map<domain::operation_type, std::string> _urls;
};

}  // namespace pinecone::net
