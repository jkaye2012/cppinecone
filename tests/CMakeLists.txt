
FetchContent_Declare(Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v${CPPINECONE_CATCH2_VERSION}
)
FetchContent_MakeAvailable(Catch2)

set(cppinecone_test_files
  "entrypoint.cpp"
  "vector_operations_tests.cpp"
  "collection_operations_tests.cpp"
  "index_operations_tests.cpp"
  "meta_operations_tests.cpp"
)

add_executable(cppinecone_tests ${cppinecone_test_files})
target_include_directories(cppinecone_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CPPINECONE_INCLUDE_DIRS})
target_link_libraries(cppinecone_tests PRIVATE Catch2::Catch2WithMain ${CPPINECONE_LIBRARIES})
