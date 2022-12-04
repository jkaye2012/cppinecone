cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

project(main)

include(ExternalProject)
include(FetchContent)

if(NOT DEFINED CURL_VERSION)
  set(CURL_VERSION 7.86.0)
endif()
if(NOT DEFINED JSON_VERSION)
  set(JSON_VERSION 3.11.2)
endif()
if(NOT DEFINED CATCH2_VERSION)
  set(CATCH2_VERSION 3.1.1)
endif()

if(NOT DEFINED CURL_SSL_BACKEND)
  set(CURL_SSL_BACKEND "openssl")
endif()
string(APPEND CURL_SSL_OPT "--with-" ${CURL_SSL_BACKEND})

ExternalProject_Add(
  curl
  URL https://curl.se/download/curl-${CURL_VERSION}.tar.gz
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/curl/
  CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/deps/curl/configure --prefix=${CMAKE_CURRENT_SOURCE_DIR}/lib/curl ${CURL_SSL_OPT}
  PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/lib/curl
  BUILD_COMMAND make
  BUILD_IN_SOURCE 0
)
add_library(libcurl SHARED IMPORTED)
include_directories(BEFORE SYSTEM ${CMAKE_SOURCE_DIR}/deps/curl/include)
set_target_properties(libcurl PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/lib/libcurl.so)

FetchContent_Declare(json
  URL https://github.com/nlohmann/json/releases/download/v${JSON_VERSION}/json.tar.xz
)
FetchContent_MakeAvailable(json)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

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

target_link_libraries(cppinecone PUBLIC libcurl nlohmann_json::nlohmann_json)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE cppinecone)

add_subdirectory(tests)
