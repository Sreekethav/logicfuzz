#!/bin/bash -eu

cd $SRC/test-project

# ---- Compile the library source files ----
$CC $CFLAGS -Iinclude -c src/intlist.c -o intlist.o
$CC $CFLAGS -Iinclude -c src/stats.c   -o stats.o

# ---- Build static library ----
ar rcs libtest.a intlist.o stats.o

# ---- Compile and link the fuzzer ----
# LogicFuzz places the LLM-generated fuzz target at fuzzer.c.
# Strip any markdown fences the LLM may have emitted, then compile
# as C++ so FuzzedDataProvider and other C++ utilities work correctly.
# The public headers already have extern "C" wrappers, so C++ inclusion
# of intlist.h / stats.h works without any extra compatibility header.
sed -i '/^```/d' $SRC/test-project/fuzzer.c
cp $SRC/test-project/fuzzer.c $WORK/fuzzer.cpp

$CXX $CXXFLAGS -Iinclude \
    -c $WORK/fuzzer.cpp -o $WORK/fuzzer.o

$CXX $CXXFLAGS $LIB_FUZZING_ENGINE \
    $WORK/fuzzer.o libtest.a \
    -o $OUT/test_project_fuzzer

# ---- Copy source for coverage reporting ----
mkdir -p $OUT/src $OUT/include
cp src/*.c    $OUT/src/
cp include/*.h $OUT/include/
