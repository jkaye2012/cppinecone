#pragma once

#include <functional>

#include <nlohmann/json.hpp>

#include "pinecone/domain/operation.hpp"
#include "pinecone/domain/operation_type.hpp"
#include "pinecone/types/index_types.hpp"

using json = nlohmann::json;

namespace pinecone::domain
{
template <>
struct operation_args<operation_type::index_list>
    : public list_operation_args<operation_type::index_list> {
  using list_operation_args::list_operation_args;

  using parsed_type = types::indexes;

  static const std::function<result<parsed_type>(std::vector<uint8_t>&)> parser;
};
inline std::function<result<types::indexes>(std::vector<uint8_t>&)> const
    operation_args<operation_type::index_list>::parser =
        [](auto& data) { return parsed_type::build(json::parse(data)); };

template <>
struct operation_args<operation_type::index_describe>
    : public describe_delete_operation_args<operation_type::index_describe> {
  using describe_delete_operation_args::describe_delete_operation_args;

  using parsed_type = types::database;

  static const std::function<result<parsed_type>(std::vector<uint8_t>&)> parser;
};
inline std::function<result<types::database>(std::vector<uint8_t>&)> const
    operation_args<operation_type::index_describe>::parser =
        [](auto& data) { return parsed_type::build(json::parse(data)); };

template <>
struct operation_args<operation_type::index_delete>
    : public describe_delete_operation_args<operation_type::index_delete> {
  using describe_delete_operation_args::describe_delete_operation_args;

  using parsed_type = std::string;

  static const std::function<result<parsed_type>(std::vector<uint8_t>&)> parser;
};
inline std::function<result<std::string>(std::vector<uint8_t>&)> const
    operation_args<operation_type::index_delete>::parser =
        [](auto& data) { return std::string(data.begin(), data.end()); };

template <>
struct operation_args<operation_type::collection_list>
    : public list_operation_args<operation_type::collection_list> {
  using list_operation_args::list_operation_args;

  using parsed_type = types::collections;

  static const std::function<result<parsed_type>(std::vector<uint8_t>&)> parser;
};
inline std::function<result<types::collections>(std::vector<uint8_t>&)> const
    operation_args<operation_type::collection_list>::parser =
        [](auto& data) { return parsed_type::build(json::parse(data)); };

template <>
struct operation_args<operation_type::collection_describe>
    : public describe_delete_operation_args<operation_type::collection_describe> {
  using describe_delete_operation_args::describe_delete_operation_args;

  using parsed_type = types::collection;

  static const std::function<result<parsed_type>(std::vector<uint8_t>&)> parser;
};
inline std::function<result<types::collection>(std::vector<uint8_t>&)> const
    operation_args<operation_type::collection_describe>::parser =
        [](auto& data) { return parsed_type::build(json::parse(data)); };

template <>
struct operation_args<operation_type::collection_delete>
    : public describe_delete_operation_args<operation_type::collection_delete> {
  using describe_delete_operation_args::describe_delete_operation_args;

  using parsed_type = std::string;

  static const std::function<result<parsed_type>(std::vector<uint8_t>&)> parser;
};
inline std::function<result<std::string>(std::vector<uint8_t>&)> const
    operation_args<operation_type::collection_delete>::parser =
        [](auto& data) { return std::string(data.begin(), data.end()); };

template <>
struct operation_args<operation_type::index_configure>
    : public patch_operation_args<operation_type::index_configure, types::index_configuration> {
  using patch_operation_args::patch_operation_args;

  using parsed_type = std::string;

  static const std::function<result<parsed_type>(std::vector<uint8_t>&)> parser;
};
inline std::function<result<std::string>(std::vector<uint8_t>&)> const
    operation_args<operation_type::index_configure>::parser =
        [](auto& data) { return std::string(data.begin(), data.end()); };
}  // namespace pinecone::domain