# Metadata Query API

## Formal specification

[Metadata filtering](https://www.pinecone.io/docs/metadata-filtering) is one of the more complicated aspects of
Pinecone's REST API. This section describes the approach taken by Cppinecone more formally for those who are interested
in the inner workings of the API.

The following [EBNF](https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form) grammar defines the structure
enforced by Cppinecone's API constructs:

```ebnf
eq = "$eq";
ne = "$ne";
gt = "$gt";
gte = "$gte";
lt = "$lt";
lte = "$lte";
in = "$in";
nin = "$nin";
and = "$and";
or = "$or";

bin_op = eq | ne | gt | gte | lt | lte;
var_op = in | nin;
com_op = and | or;

str = ?quoted Pinecone string?;
num = ?Pinecone number?;
bool = true | false;

meta_key = str;
meta_val = num | str | bool;
meta_arr = "[", meta_val, {", ", meta_val}, "]";

bin_pred = "{", meta_key, ": {", bin_op, ": ", meta_val, "}}";
var_pred = "{", meta_key, ": {", var_op, ": ", meta_arr, "}}";
com_pred = "{", com_op, ": ", com_arr, "}";

com_arr = "[", pred, {", ", pred} "]";
pred = bin_pred | var_pred | com_pred;

filter = '{ "filter": ', pred, '}';
```

Note that this grammar does not exactly match the filters supported by Pinecone's API natively! While Pinecone
technically does provide other mechanisms to express these filters, the grammar used by Cppinecone is comprehensive
enough to encode any filter expression supported by Pinecone while also maintaining a level of simplicity that makes the
API much easier to work with than it would be otherwise.

From the grammar, it's apparent that the necessary rules are simpler than they appear. The API supports only three types of
operations:

1. Binary predicates: `eq`, `ne`, `gt`, `gte`, `lt`, and `lte`
2. N-ary predicates: `in` and `nin`
3. Combination predicates: `and` and `or`

Binary and N-ary predicates can be thought of as atoms, meaning that they cannot be decomposed into smaller operations,
while Combination predicates are instead productions, meaning that they are always composed of two other predicates. We can
therefore describe metadata filters using [prefix notation](https://en.wikipedia.org/wiki/Polish_notation) so that
operator precedence is handled automatically by filter construction.

This insight is the core of Cppinecone's metadata filtering API. Filters are constructed as an N-ary tree with a single
root where all leaves form atomic operations. A depth-first traversal of the tree thus automatically constructs the
prefix notation. This construction also allows the API to easily leverage lazy evaluation, meaning that each filter
constructed by a user requires only a single allocation step.

## Example

We demonstrate application of both simple and compound predicate filters. This example should be able to be compiled and
run assuming that Cppinecone has been [installed](./installation.md). The example assumes that the provided API key has
access to an index named `squad` created from the `SQuAD` public collection.

```c++
#include <cassert>
#include <cstdint>
#include <iostream>
#include <variant>

#include <pinecone/pinecone.hpp>
#include <pinecone/types/filters.hpp>
#include <pinecone/types/vector_types.hpp>

static inline void run_metadata_example(pinecone::synchronous_client const& client) noexcept
{
  auto comic_filter = pinecone::types::filters::eq("title", "Marvel_Comics");
  auto result =
      client.query("squad", pinecone::types::query_builder(comic_filter, 1000, "9833").build());
  assert(result.is_successful());
  std::cout << "Marvel comic results: " << result->query_matches().size() << std::endl;

  auto ps_filter = pinecone::types::filters::eq("title", "PlayStation_3");
  auto combined_filter = pinecone::types::filters::or_(comic_filter, ps_filter);
  auto combined_result =
      client.query("squad", pinecone::types::query_builder(combined_filter, 1000, "9833").build());
  assert(combined_result.is_successful());
  std::cout << "Marvel comic or playstation results: " << combined_result->query_matches().size()
            << std::endl;
}

int main(int argc, char** argv)
{
  assert(argc == 3);
  auto client = std::get<0>(pinecone::synchronous_client::build({argv[1], argv[2]}));
  run_metadata_example(client);
  return 0;
}
```