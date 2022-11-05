cmake_minimum_required(VERSION 3.9)
project(main)

find_package(CURL REQUIRED)

add_executable(main main.cpp)

include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(main ${CURL_LIBRARIES})
