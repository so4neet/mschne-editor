#!/bin/bash

cTarget="editor.out"
cObjs=$(find . -type f -name "*.c")
cFlags="-I./src"
lFlags="-lm -lraylib"

clang $cObjs $cFlags -o build/$cTarget $lFlags
./build/editor.out