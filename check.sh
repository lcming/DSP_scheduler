#!/bin/bash
echo "check results"

for (( i=1; i<=6; i++))
do
    echo "scalar $i $i $i"
    ./run_all.pl 0 $i $i $i | grep "total" > results/0$i$i$i.result
    diff results/0$i$i$i.result results/0$i$i$i.golden 
done

for (( i=1; i<=6; i++))
do
    echo "vliw $i $i $i"
    ./run_all.pl 1 $i $i $i | grep "total" > results/1$i$i$i.result
    diff results/1$i$i$i.result results/1$i$i$i.golden
done

rm results/*.result



