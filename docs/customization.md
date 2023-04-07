# API customization

## Policies

TODO: links

While Cppinecone has sensible defaults that should be appropriate for most use cases, we recognize that some users may
have differing needs or technical constraints. The API supports compile-time customization through policies that
allow control over the following behaviors:

* [Failure handling](): for users who would prefer exceptions to the `result` API
* [Allocation](): for users who require fine-grained control over memory allocation strategies

### Example

## Logging

Cppinecone uses [spdlog](https://github.com/gabime/spdlog) for logging. By default, no logger will be configured, so it
is completely up to the user to configure logging if they wish to capture Cppinecone's diagnostic output. Note that logs
are **not** considered a public API surface, meaning that the logs written by Cppinecone should never be relied upon for
any automated purpose. Written logs may change with any release, including patch releases.

See [the logging file](/doxygen/html/logging_8hpp.html) for full details.

Note that logging is currently not particularly useful. It should be improved in the near future.

### Example

```c++
#include <pinecone/pinecone.hpp>
#include <pinecone/util/logging.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

static inline void example() noexcept
{
    pinecone::util::setup_logger([](auto const& name) {
        return spdlog::stdout_color_mt(name);
    });
    pinecone::synchronous_client client = // ...
}
```