#!/bin/bash

CFLAGS="-Wall -Wextra -Wdouble-promotion -Wno-unused -Wno-unused-parameter -Wno-missing-braces"
DEBUG_FLAGS="-DBUILD_DEBUG -g3"

RELEASE_FLAGS_ARM64="-O3 -flto -march=native"
RELEASE_FLAGS_X64="-O3 -flto -march=native"

FUZZER_FLAGS="-fsanitize=fuzzer,address,undefined"
TEST_FLAGS="-fsanitize=address,undefined"
PROFILER_FLAGS="-std=c++20"

clang++ $TEST_FLAGS $DEBUG_FLAGS $CFLAGS -I./ test.cpp -o test
clang++ $FUZZER_FLAGS $DEBUG_FLAGS $CFLAGS -I./ fuzzer_test.cpp -o fuzzer_test
clang++ $RELEASE_FLAGS_ARM64 $PROFILER_FLAGS -I./ profile_test.cpp -o profile_test
clang++ $DEBUG_FLAGS $TEST_FLAGS -I./ asan_test.cpp -o asan_test
