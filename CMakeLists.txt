cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

project(main)

include(ExternalProject)
include(FetchContent)

# TODO: allow user to configure these
set(CURL_VERSION 7.86.0)
set(CURL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/include)
set(CURL_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/lib/libcurl.a)
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

FetchContent_Declare(json
  URL https://github.com/nlohmann/json/releases/download/v${JSON_VERSION}/json.tar.xz
)
FetchContent_MakeAvailable(json)

add_executable(main main.cpp)

include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(main PRIVATE ${CURL_LIBRARIES})
target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json)

# TODO: make building tests optional?
add_subdirectory(tests)
