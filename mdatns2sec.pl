#!/usr/bin/perl

use strict;
use warnings;

open my $fh, '<', "/tmp/tt.txt" or die $!;

my $lasttime_ns=0;
my $duration_s = 0;
my $firsttime_ns = 0;

while (<$fh>) {

my ($time_ns, $type, $id, $counter) = split;
if ($lasttime_ns == 0)
	{
	$lasttime_ns = $time_ns;
	$firsttime_ns = $time_ns;
	} 
	$duration_s = ($time_ns - $lasttime_ns) * 1e-9;
	$lasttime_ns = $time_ns;

	printf("%16d %d %d %d ",$time_ns-$firsttime_ns, $type, $id, $counter);
	printf("    duration: %8d (s)\n", $duration_s)
}

