#!/bin/bash

./run_build.sh

pids=()
./build/tests/cppinecone_tests "[meta]" &
pids+=($!)
./build/tests/cppinecone_tests "[index]" &
pids+=($!)
./build/tests/cppinecone_tests "[collection]" &
pids+=($!)
./build/tests/cppinecone_tests "[vector][basic]" &
pids+=($!)
./build/tests/cppinecone_tests "[vector][metadata]" &
pids+=($!)

result=0
for pid in "${pids[@]}"; do
    wait "$pid"
    result=$((result + $?))
done
exit $result
