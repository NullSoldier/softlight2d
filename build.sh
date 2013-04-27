#!/bin/bash
mkdir -p build
clang++ src/*.cpp -o build/light \
	-std=c++11 \
	-framework Cocoa \
	-framework IOKit \
	-framework OpenGL \
	-stdlib=libc++ \
	-L lib \
	-l glfw \
	-I include
