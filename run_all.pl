#!/usr/bin/perl

@irs = <ir/*.ir>;
if(scalar@ARGV < 1)
{
    die "usage: archi (0:scalar, 1:vliw, 2:cascaded)";
}
if($ARGV[0] == 2)   #cascade
{
    if(scalar@ARGV < 2)
    {
        die "usage: archi, cascade config file"; 
    }
}
else
{
    if(scalar@ARGV < 4)
    {
        die "usage: archi , #of ADD MUL SHI (0:scalar, 1:vliw, 2:cascaded)";
    }
}

foreach my$ir (@irs)
{
    system("./scheduler $ir @ARGV");
}
