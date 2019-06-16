#!/bin/bash
echo "Gnerate Makefile:"
cmake .
echo "Start Making:"
make
echo "Clean Cache:"
rm -rfv CMakeFiles && \
rm -fv Makefile && \
rm -fv cmake_install.cmake &&\
rm -fv CMakeCache.txt
echo "Done."