cmake_minimum_required(VERSION 3.20)

set(CPPINECONE_BUILD_TESTS "off" CACHE BOOL "Whether to build tests")
set(CPPINECONE_CURL_VERSION 7.86.0 CACHE STRING "libcurl version")
set(CPPINECONE_JSON_VERSION 3.11.2 CACHE STRING "json version")
set(CPPINECONE_CATCH2_VERSION 3.1.1 CACHE STRING "catch2 version")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

project(cppinecone)

include(FetchContent)

FetchContent_Declare(curl
  URL https://curl.se/download/curl-${CPPINECONE_CURL_VERSION}.tar.gz
)
FetchContent_MakeAvailable(curl)

FetchContent_Declare(json
  URL https://github.com/nlohmann/json/releases/download/v${CPPINECONE_JSON_VERSION}/json.tar.xz
)
FetchContent_MakeAvailable(json)

set(includes
  ${nlohmann_json_SOURCE_DIR}/include
  ${curl_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )
set(CPPINECONE_INCLUDE_DIRS ${includes} CACHE STRING "Include directories required to build cppinecone")

add_library(cppinecone STATIC
  src/pinecone.cpp
  src/domain/index_operations.cpp
  src/domain/meta_operations.cpp
  src/domain/method.cpp
  src/domain/operation.cpp
  src/domain/operation_type.cpp
  src/domain/vector_operations.cpp
  src/net/arguments.cpp
  src/net/http_client.cpp
  src/net/url_builder.cpp
  src/types/accepted.cpp
  src/types/api_metadata.cpp
  src/types/error.cpp
  src/types/index_types.cpp
  src/types/parser.cpp
  src/types/vector_metadata.cpp
  src/types/vector_types.cpp
  src/util/curl_result.cpp
  src/util/result.cpp
  src/util/visit.cpp
)

target_include_directories(cppinecone SYSTEM BEFORE PUBLIC ${CPPINECONE_INCLUDE_DIRS})
target_link_libraries(cppinecone PRIVATE libcurl nlohmann_json::nlohmann_json)

if(${CPPINECONE_BUILD_TESTS})
  message("-- Tests and examples will be built")
  add_executable(main main.cpp)
  target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
  target_link_libraries(main PRIVATE cppinecone)

  add_subdirectory(tests)
endif()
