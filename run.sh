#!/bin/bash

make
input=`ls ./OS_PJ1_Test`
for cur in $input;
do
    echo $cur >> output
    ./a.out < ./OS_PJ1_Test/$cur >> output
done
