# Quick start guide

One of Cppinecone's primary design goals is to be as simple as possible for basic usage.
There are only two requirements to interact with the Pinecone API:

* You must know the environment (region) that you wish to interact with
* You must have access to a Pinecone API key with appropriate permissions

This information is required by Pinecone itself - Cppinecone doesn't carry any of its
own requirements.

## Download and install

Cppinecone must be integrated into your application before it can be used. There are multiple
methods available; for a full listing, please see the [installation documentation](./installation.md).

The simplest installation method is Cmake's [FetchContent]():

TODO: broken links and complete this section

## Create a client

All API access is performed through a `pinecone_client` instance. Currently, only a synchronous
client is supported (though there are plans for an asynchronous option in the future).

```cpp
#include <pinecone/pinecone.hpp>

auto client = std::move(std::get<pinecone::synchronous_client>(pinecone::synchronous_client::build({"us-west1-gcp", "api_key_goes_here"})));
```

Note that it is technically possible for client creation to fail, so the `build` function
returns Cppinecone's `result` type. While this is true, client construction failure is possible
only due to a system configuration problem (usually, a curl version mismatch), so this possibility
can be safely ignored in many situations. If you're in control of the systems that your code will
be run on, you can safely use the `result` directly without unwrapping it.

## Run a collection operation

Let's use the client to find the collections available to us:

```cpp
auto collections = client.list_collections();
if(collections) {
  std::cout << "Number of collections: " << collections->names().size() << std::endl; // Requires <iostream>
} else {
  std::cerr << "Failed to list collections: " << collections.to_string() << std::endl;
}
```

Most Cppinecone operations are as simple as this: create a client once somewhere within your application, then
run all future operations using the single client instance. Multiple client instances are required only
for concurrent requests, or when access to multiple environments/API keys are necessary.

The synchronous client is **not** thread-safe; if multiple threads require Pinecone access, either create a client per
thread or ensure that the client is protected by a [mutex](https://en.cppreference.com/w/cpp/thread/mutex).
Once the asynchronous client is released, this restriction will be lifted.

Same as the client construction, all API operations by default return a [result](/doxygen/html/structpinecone_1_1util_1_1result.html). `result` models the possibility
of failure; as all API operations require network access, they are all fallible. `result` is implicitly
`nodiscard`, meaning that your compiler will warn you if you accidentally forget to ensure that an operation
was successful.

Note that even though Cppinecone itself does not throw exceptions by default, it _cannot_ be compiled with
`no-exceptions` due to carried dependencies that are incompatible with that compilation option.

If you'd prefer exceptions to the `result` construction, please see the [API customization](./customization.md)
documentation for the other available failure handling policies.

## Run a vector operation

Vector operations are very similar to collection operations. All vector operations require the index name to be
provided:

```cpp
auto stats = client.describe_index_stats("squad");
if(stats) {
  std::cout << "Number of namespaces: " << stats->namespaces().size() << std::endl;
} else {
  std::cerr << "Failed to describe stats: " << stats.to_string() << std::endl;
}
```

## Next steps

This is all you need to know to use Cppinecone! For more information about the available operations and other
in-depth details, check out the rest of the documentation:

* [API reference](./api_reference.md)
* [Metadata query API](./metadata_query_api.md)
* [API customization](./customization.md)

## Complete code example

This snippet can be compiled and run directly.

```cpp
#include <iostream>
#include <string>
#include <variant>

#include <pinecone/pinecone.hpp>

int main(int argc, char** argv) {
  if(argc != 2) {
    std::cerr << "Please provide Pinecone environment and API key as parameters" << std::endl;
    return 1;
  }

  auto client_build = pinecone::synchronous_client::build({argv[1], argv[2]});
  if(!std::holds_alternative<pinecone::synchronous_client>(client_build)) {
    std::cerr << "Failed to create client: " << std::get<std::string>(client_build) << std::endl;
    return 2;
  }
  auto client = std::move(std::get<pinecone::synchronous_client>(client_build));

  auto collections = client.list_collections();
  if(collections) {
    std::cout << "Number of collections: " << collections->names().size() << std::endl;
  } else {
    std::cerr << "Failed to list collections: " << collections.to_string() << std::endl;
  }

  auto stats = client.describe_index_stats("squad");
  if(stats) {
    std::cout << "Number of namespaces: " << stats->namespaces().size() << std::endl;
  } else {
    std::cerr << "Failed to describe stats: " << stats.to_string() << std::endl;
  }

  return 0;
}
```
