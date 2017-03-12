#!/usr/bin/perl

use strict;
use warnings;

my $lasttime_ns=0;
my $duration_s = 0;
my $firsttime_ns = 0;
my $SumOfAll=0;
my $countSyncs=0;
my $grepline="";

my $numArgs = @ARGV;

if ($numArgs!=3)
{
  printf("Expected 3 arguments, got %d\n Usage: \n getperiods <filename> <ChSync> <mode>\n",$numArgs);
  printf("mode  0: verbose \n");
  printf("      1: only result in (s)\n");
  printf("      2: only result in (ms)\n");
  printf("      3: only result in (ns)\n");
  exit(1);
}

my ($ArgFile, $ArgID, $ArgMode) = @ARGV;

my $BeVerbose=0;

if ($ArgMode==0) {
  $BeVerbose=1;
}

open my $fh, '<', $ArgFile or die $!;

my $IsFirstRun=1;
my $AvgSync_s=0;

while (<$fh>) {

  my ($time_ns, $type, $id, $counter) = split;
  if ($id==$ArgID)
  {
    if ($IsFirstRun)
    {
      $lasttime_ns = $time_ns;
      $firsttime_ns = $time_ns;
      $IsFirstRun=0;
    } 
    else {
      $duration_s = ($time_ns - $lasttime_ns) * 1e-9;
      $lasttime_ns = $time_ns;
      $SumOfAll += $duration_s;
      $countSyncs +=1;
      $AvgSync_s = $SumOfAll/$countSyncs;

      if ($BeVerbose){
        printf("%16d %d %d %d ",$time_ns-$firsttime_ns, $type, $id, $counter);
        printf("    duration: %8f (s)", $duration_s);
        printf("  Avg: %f (s)\n", $AvgSync_s);
      }
    }
  }
}

if ($countSyncs > 0){
  if ($BeVerbose){
    printf("\nAvg: %f (s)\n", $AvgSync_s);
    printf("0.5 tau = 0.5 * %d (ns) = \n %d \n", $AvgSync_s*1e9, 0.5*$AvgSync_s*1e9);
    } else {


if ($ArgMode==1){printf("%5.6f %s\n", $AvgSync_s, $ArgFile);} # (s)
if ($ArgMode==2){printf("%5.3f %s\n", $AvgSync_s*1e3, $ArgFile);} # (ms)
if ($ArgMode==3){printf("%d %s\n", $AvgSync_s*1e9, $ArgFile);} # (ns)
	    
    }
  }

