#!/usr/bin/tclsh
puts "check results with goldens"

for {set i 1} {$i < 7} {incr i} {
    exec ./run_all.pl 0 $i $i $i | grep "total" > results/0$i$i$i.golden
}

for {set i 1} {$i < 7} {incr i} {
    exec ./run_all.pl 1 $i $i $i | grep "total" > results/1$i$i$i.golden
}
#exec rm results/*.result


