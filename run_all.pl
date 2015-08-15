#!/usr/bin/perl

@irs = <ir/*.ir>;
if(scalar@ARGV < 3)
{
    die "usage: #of ADD MUL SHI";
}

foreach my$ir (@irs)
{
    system("./scheduler $ir @ARGV");
}
