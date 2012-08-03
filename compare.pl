#!/usr/bin/env perl
use strict;
use warnings;
use File::Slurp;

my @file1 = read_file($ARGV[0]) or die;
my @file2 = read_file($ARGV[1]) or die;

sub getvals {
	my $x = shift;
	my @a = split /\t/, $x;
	return $a[0], $a[1];
}


for(@file1) {
	my ($size, $val) = getvals($_);
	my $s = shift(@file2);
	my ($size2, $val2) = getvals($s);
	exit 1 if($size != $size2);
	chomp($val);
	chomp($val2);
	print "size: $size\t$val\t$val2\n";
}
