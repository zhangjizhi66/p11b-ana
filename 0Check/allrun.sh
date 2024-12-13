#!/bin/bash

for file in ../*_*_*.root
do
    temp=${file#*_}
    runnum=${temp%%_*}
    
    ./ana $runnum &
done