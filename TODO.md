# To do list

- [ ] CMake dependency management should be improved, at least modularized
- [ ] Document global initialization for curl
- [ ] Document architecture, namespace deps, etc
- [ ] Consider noexcept interaction with allocations
- [ ] Add policy to public API for possibility of throwing
- [ ] Complete implementation of fetch operation
- [ ] Consider nlohmann patch for std::optional handling
- [ ] Send issues to Pinecone about API documentation problems
  - Collections describe doesn't send size when status is not ready (also dimension field undocumented)
  - Can't create an index from a collection (collection not found)
