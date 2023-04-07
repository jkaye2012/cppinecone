# Introduction

Cppinecone provides a simple API for C++ applications to interact with the [Pinecone vector database](https://pinecone.io).

To learn about the details of the Pinecone API, please see their [reference
documentation](https://docs.pinecone.io/reference/).

While I recommend starting with the [quick start guide](./quickstart.md), you may also wish to take a look at the
[client](/cppinecone/doxygen/html/structpinecone_1_1pinecone__client.html), which serves as the entry-point to the API and is the
primary public interface for users of Cppinecone.

## Disclaimer

Cppinecone is in a relatively early state of development. The client-facing API should remain relatively stable (at
least as stable as the Pinecone API itself), but bugs are certainly possible. The client has not been highly optimized
and most API operations will heap allocate - this allocation is not strictly required and could eventually be controlled
by the user if the needs arises.

## Design decisions

Cppinecone makes a small number of design decisions that users may wish to be aware of:

* The client will never throw exceptions
* Fallible operations will _always_ return a
  [result](/cppinecone/doxygen/html/structpinecone_1_1util_1_1result.html)

These decisions can be modified through the use of [API customizations](./customization.md).

## Known issues

* Rarely, the Pinecone API will return 50X errors for operations run against an index/collection too soon after its
  initialization is complete. This is a problem with the Pinecone API itself, and not something that Cppinecone can
  control
* Some Pinecone API operations seem to return undocumented data with their results. Cppinecone provides support only for
  data explicitly documented in Pinecone's API reference
* Cppinecone has been tested only on Debian-based Linux distributions using the Clang compiler. Other platforms and
  compilers should be supported, but this support has not been verified. If you find compatibility issues, please create
  an issue.

## Dependencies

Cppinecone aims to minimize dependencies, both those required by users and those used to construct the client API.
Currently, Cppinecone carries the following required dependencies:

* Required at run-time
    * [CURL](https://curl.se/docs/) for http communication
* Used within the client implementation
    * [nlohmann/json](https://github.com/nlohmann/json) for JSON serde
    * [spdlog](https://github.com/gabime/spdlog) for logging
    * [Catch2](https://github.com/catchorg/Catch2) for unit testing

More information about these dependencies and what they mean for users can be found in the [installation documentation](./installation.md).
