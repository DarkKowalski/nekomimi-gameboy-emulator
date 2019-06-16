#!/bin/bash
echo "Gnerate Makefile:"
cmake .
echo "Start Making:"
make
echo "Clean Cache:"
make clean && rm -rfv CMakeFiles
echo "Done."