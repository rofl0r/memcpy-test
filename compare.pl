#!/usr/bin/env perl
use strict;
use warnings;
use File::Slurp;

my $code1 = $ARGV[0] or die;
my $code2 = $ARGV[1] or die;

my $tmp = "/tmp/memcpy.txt";

print "speed comparison - $code1 vs $code2\n";
print "CC=$ENV{CC} OPTS=$ENV{OPTS}\n";

system("./build.sh $code1 && ./test > $tmp.1") and die;
system("./build.sh $code2 && ./test > $tmp.2") and die;

my @file1 = read_file("$tmp.1") or die;
my @file2 = read_file("$tmp.2") or die;

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
