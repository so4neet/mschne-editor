#!/bin/bash

debug=0
run=0

# Get Launch Flags

while getopts dr flag
do
        case "${flag}" in
                d) debug=1;;
                r) run=1;;
        esac
done

echo "Building MSCHNE Editor"
if [ "$debug" -eq 1 ]; then
        echo "Debug Build Enabled"
fi
if [ "$run" -eq 1 ]; then
        echo "Running after build"
fi

