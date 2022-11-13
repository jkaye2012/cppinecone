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

From the grammar, we can see that the necessary rules are simpler than they appear. The API supports only three types of
operations:

1. Binary predicates: `eq`, `ne`, `gt`, `gte`, `lt`, and `lte`
2. N-ary predicates: `in` and `nin`
3. Combination predicates: `and` and `or`

Binary and N-ary predicates can be thought of as atoms, meaning that they cannot be decomposed into smaller operations,
while Combination predicates are instead productions, meaning that they are always composed of constituent atoms. We can
therefore describe metadata filters using [prefix notation](https://en.wikipedia.org/wiki/Polish_notation) so that
operator precedence is handled automatically by filter construction.

This insight is the core of Cppinecone's metadata filtering API. Filters are constructed as an N-ary tree with a single
root where all leaves form atomic operations. A depth-first traversal of the tree thus automatically constructs the
prefix notation. This construction also allows the API to easily leverage lazy evaluation, meaning that each filter
constructed by a user requires only a single allocation step.
