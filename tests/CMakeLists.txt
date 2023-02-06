
FetchContent_Declare(Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v${CATCH2_VERSION}
)
FetchContent_MakeAvailable(Catch2)

set(cppinecone_test_files
  "entrypoint.cpp"
  "meta_operations_tests.cpp"
)

add_executable(cppinecone_tests ${cppinecone_test_files})
target_include_directories(cppinecone_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cppinecone_tests PRIVATE Catch2::Catch2WithMain cppinecone)
