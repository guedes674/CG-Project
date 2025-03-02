#!/bin/bash

make clean
clear
make

# Run generator
./generator box 2 3 box_2_3.3d
if [ $? -eq 0 ]; then
    # Run engine only if generator succeeds
    ./engine configs/test_1_4.xml
else
    echo "Generator failed. Not running engine."
fi