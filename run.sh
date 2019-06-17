#!/bin/bash
cat ./runtime-manual
echo ""
if [ x$1 != x ]
then
./run-emulator "$@"
else
./run-emulator
fi
