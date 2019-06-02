#!/bin/bash

ATTEMPTS=100
EXE=LEDemo-Release.exe

if [ ! -f "$EXE" ]; then
    echo -e "$EXE not found!"
    exit -1
fi

if [ ! -z $1 ]; then
    ATTEMPTS=$1
fi

echo -e "\nStarting Test using $EXE ($ATTEMPTS runs):\n"
for (( i=1; i<=$ATTEMPTS; i++ ))
do
    echo -e "\e[1ARun #$i"
    ./"$EXE" --terminate-on-ready > test_many_runs_output.txt 2>&1
done

echo -e "Test Complete"
