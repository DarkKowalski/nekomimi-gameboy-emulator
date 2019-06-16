#!/bin/bash
cat ./runtime-manual
echo ""
if [ x$1 != x ]
then
./run-emulator "$1"
else
./run-emulator
fi