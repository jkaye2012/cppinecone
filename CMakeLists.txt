cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
# TODO: make configurable
set(CMAKE_BUILD_TYPE Debug)
if($CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "$CMAKE_CXX_FLAGS -D_GLIBCXX_DEBUG")
endif()

project(main)

include(ExternalProject)
include(FetchContent)

# TODO: allow user to configure these
set(CURL_VERSION 7.86.0)
set(JSON_VERSION 3.11.2)
set(CATCH2_VERSION 3.1.1)

# TODO: openssl should be configurable/optional
ExternalProject_Add(
  curl
  URL https://curl.se/download/curl-${CURL_VERSION}.tar.gz
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/curl/
  CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/deps/curl/configure --prefix=${CMAKE_CURRENT_SOURCE_DIR}/lib/curl --with-openssl
  PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/lib/curl
  BUILD_COMMAND make
  BUILD_IN_SOURCE 1
)
find_package(CURL REQUIRED)

FetchContent_Declare(json
  URL https://github.com/nlohmann/json/releases/download/v${JSON_VERSION}/json.tar.xz
)
FetchContent_MakeAvailable(json)

include_directories(
  ${CURL_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
 )

add_library(cppinecone STATIC
  src/pinecone.cpp
  src/result.cpp
  src/domain/curl_result.cpp
  src/domain/index_operations.cpp
  src/domain/method.cpp
  src/domain/operation.cpp
  src/domain/operation_type.cpp
  src/types/index_types.cpp
  src/util/visit.cpp
  src/net/http_client.cpp
  src/net/url_builder.cpp
)

target_link_libraries(cppinecone PUBLIC ${CURL_LIBRARIES} nlohmann_json::nlohmann_json)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE cppinecone)

# TODO: make building tests optional?
add_subdirectory(tests)
