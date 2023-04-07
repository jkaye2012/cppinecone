# Installation

Cppinecone currently supports integration into other projects via [CMake](https://cmake.org/).
[Conan](https://conan.io/) integration is planned for the future, but is not yet supported.

## CMake

To integrate Cppinecone into a CMake project, you should use
[FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) to automatically download and link the
desired version of the library.

### Example

The [examples directory](/doxygen/html/CMakeLists_8txt-example.html) contains a complete example of Cppinecone
integration into another project. Most users will want to use a versioned tag to ensure library stability. By default,
tests and the executable harness are not built. There is generally no reason for end-users to desire these targets.

```cmake
cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_BUILD_TYPE Debug)

project(cppinecone_examples)

include(FetchContent)

FetchContent_Declare(libcppinecone
    GIT_REPOSITORY https://github.com/jkaye2012/cppinecone.git
    GIT_TAG v0.1.0)
FetchContent_Populate(libcppinecone)
add_subdirectory(${libcppinecone_SOURCE_DIR})

add_executable(cppinecone_examples_exe main.cpp)
target_include_directories(main PRIVATE ${CPPINECONE_INCLUDE_DIRS})
target_link_libraries(main PRIVATE ${CPPINECONE_LIBRARIES})
```

Cppinecone should now be available via `#include <pinecone/pinecone.hpp>`.

### Dependency configuration

By default, Cppinecone's CURL configuration assumes that OpenSSL should be used for TLS, and that the library is
available for linking when CURL is being built.

[Three third-party libraries](./index.md#dependencies) are vended with Cppinecone for ease of use. The configuration of
these libraries can be controlled by the user by setting their supported CMake variables. For most users, the library
defaults should be reasonable.

* CURL's supported variables are unfortunately not well-documented. You can find them directly in the
  [CMakeLists.txt](https://github.com/curl/curl/blob/master/CMakeLists.txt) file in their main repository
* JSON's CMake support is [well-documented](https://json.nlohmann.me/integration/cmake/)
* Spdlog does not require CMake configuration