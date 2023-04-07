cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_BUILD_TYPE Debug)

project(cppinecone_examples)

include(FetchContent)

FetchContent_Declare(libcppinecone
    GIT_REPOSITORY ${CMAKE_SOURCE_DIR}/..
    GIT_TAG main)
FetchContent_Populate(libcppinecone)
add_subdirectory(${libcppinecone_SOURCE_DIR})

add_executable(cppinecone_examples_exe main.cpp)
target_include_directories(cppinecone_examples_exe PRIVATE ${CPPINECONE_INCLUDE_DIRS})
target_link_libraries(cppinecone_examples_exe PRIVATE ${CPPINECONE_LIBRARIES})