# API customization

TODO: links

While Cppinecone has sensible defaults that should be appropriate for most use-cases, we recognize that some users may
have differing needs or technical constraints. The API supports compile-time customization through [policies]() that
allow control over the following behaviors:

* [Failure handling](): for users who would prefer exceptions to the `result` API
* [Logging](): for users who require detailed introspection into the inner workings of the API
* [Allocation](): for users who require fine-grained control over memory allocation strategies
