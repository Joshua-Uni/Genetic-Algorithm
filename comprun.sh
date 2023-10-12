#!/bin/bash

# exit script at first error
set -u -e

# Compile it and output to executable called 'app'
clang Main.c aux.c -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2 -o run

# runs the app
./run
