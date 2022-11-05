cmake_minimum_required(VERSION 3.9)
set(CMAKE_CXX_STANDARD 17)

project(main)

include(ExternalProject)

# TODO: allow user to configure this
set(CURL_VERSION 7.86.0)
set(CURL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/include)
set(CURL_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/lib/curl/lib/libcurl.a)

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

add_executable(main main.cpp)

include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(main ${CURL_LIBRARIES})
