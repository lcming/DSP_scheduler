#!/usr/bin/perl

@irs = <ir/*.ir>;
if(scalar@ARGV < 4)
{
    die "usage: #of ADD MUL SHI archi (0:scalar, 1:vliw, 2:cascaded)";
}

foreach my$ir (@irs)
{
    system("./scheduler $ir @ARGV");
}
