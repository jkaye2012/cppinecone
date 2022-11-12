#pragma once

#include <map>
#include <sstream>
#include <string>
#include <string_view>

#include "pinecone/domain/operation_type.hpp"
#include "pinecone/types/api_metadata.hpp"

namespace pinecone::net
{
struct url_builder {
  explicit url_builder(std::string environment) noexcept : _environment(std::move(environment))
  {
    std::ostringstream oss;
    oss << "https://controller." << _environment << ".pinecone.io";
    _controller_prefix = oss.str();
  }

  auto set_metadata(types::api_metadata metadata) noexcept { _metadata = std::move(metadata); }

  [[nodiscard]] auto prefix() const noexcept -> std::string_view { return _controller_prefix; }

  template <domain::operation_type op>
  [[nodiscard]] auto build() const noexcept -> std::string
  {
    if constexpr (domain::op_api_type(op) == domain::api_type::controller) {
      return _controller_prefix + domain::op_url_fragment(op);
    }
  }

  template <domain::operation_type op>
  [[nodiscard]] auto build(std::string_view index_name) const noexcept -> std::string
  {
    if constexpr (domain::op_api_type(op) == domain::api_type::service) {
      std::ostringstream oss;
      oss << "https://" << index_name << "-" << _metadata.project_name() << ".svc." << _environment
          << ".pinecone.io" << domain::op_url_fragment(op);
      return oss.str();
    }
  }

 private:
  std::string _environment;
  std::string _controller_prefix;
  types::api_metadata _metadata;
};

}  // namespace pinecone::net
