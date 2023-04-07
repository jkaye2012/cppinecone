# Architecture

The most important tenet of Cppinecone's architecture is that using the library must be simple for end users. Users
generally do not care about details of how the client works internally, nor should the library prescribe specific
behaviors or methodologies on our users whenever possible. With this in mind, the most important concept in the
architecture is:

* The public client API

The client API is in turn driven by a small number of "top-level" concepts that allow for easy implementation of
communication with Pinecone's REST API:

* URL construction
* HTTP communication

Finally, we must encode the data structures and communication protocols for each operation that the Pinecone API
supports. This is provided by:

* Individual API operations

Each of these concepts is briefly explained here. The purpose of this document is to provide a high-level overview of
_why_ the code is structured in this way; it does not attempt to explain _how_ each individual component works or
why low-level implementation details were chosen.

## The public client API

The [public client API](/doxygen/html/structpinecone_1_1pinecone__client.html) is meant to be the primary (and, for now, only) entrypoint into Cppinecone. Users should not be
required to use anything other than a client to access basic API functionality. Similarly, customizations applied to the
API should be applied to the client instance so that instantiation remains simple and confined to a single concept
within the code base.

## URL construction

Perhaps surprisingly, URL construction for Pinecone's REST APIs is non-trivial. Different API operation require
significantly different URL fragments, and many operations require metadata that must be retrieved from the API itself.
These operation-level details are abstracted from client APIs and HTTP communication by the concept of opaque URL
construction for each individual operation. This functionality is provided by the
[url_builder](/doxygen/html/structpinecone_1_1net_1_1url__builder.html).

## HTTP communication

One of the simplest parts of Cppinecone, libcurl is used to perform the actual HTTP requests to Pinecone's REST API. The
[http_client](/doxygen/html/structpinecone_1_1net_1_1http__client_3_01threading__mode_1_1sync_01_4.html)
concept exists to abstract away the low-level details of CURL's APIs from the rest of Cppinecone's implementation. This
also provides a convenient location for unified serde operations and consistent error handling across all API operations.

## Individual API operations

Each API operation is implemented independently, but in terms of shared concepts that remove the need for
boilerplate/repetition. The definitions for these concepts are [stored together](/doxygen/html/operation__type_8hpp.html).

Generally speaking, the steps required to implement a new API operation are:

1. Add the operation definition as a supported `operation_type`
2. Map the new `operation_type` to an [API type](/doxygen/html/operation__type_8hpp.html#a5a1a5d9d2152ec2530acf8e4cbc00859)
3. Map the new `operation_type` to a [url fragment](/doxygen/html/operation__type_8hpp.html#a2b396a3a5f137adfe14202d0c0c0b76e)
4. Map the new `operation_type` to an [HTTP method](/doxygen/html/operation__type_8hpp.html#ac1271542a4aec7ece1b57b14bf12c475)
5. Define any [custom C++ types](/doxygen/html/index__types_8hpp.html) necessary to represent the operation request/response
6. [Wire up the operation](/doxygen/html/index__operations_8hpp.html)
7. Expose the operation via a new function on the public client

---

Cppinecone buckets API operations into three different categories:

* [Metadata operations](/doxygen/html/meta__operations_8hpp.html): provide information about the Pinecone API itself
* [Index operations](/doxygen/html/index__operations_8hpp.html): CRUD operations on Pinecone indices and collections
* [Vector operations](/doxygen/html/vector__operations_8hpp.html): CRUD operations on individual vectors within indices
  and collections

The relationship between the API operation categories and the remainder of Cppinecone can be visualized with the file
include diagram available within [the generated documentation](http://localhost:8000/doxygen/html/operation_8hpp.html).