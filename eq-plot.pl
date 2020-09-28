#!/usr/bin/perl
# Prototype code in perl.
# It's old and not functional.  The working code is in .c (main.c right now)
use strict; use warnings;
use 5.023;

my $dfn = "data-samples/sample-test.txt";
open(D, "<", $dfn) || die "cant open data file: $dfn: $!";
my @terms;
my %points;
my $verbose=2;
say "Reading data:";
while (<D>) {
	chomp;
	my ($x, $y) = split /\t/;
	printf "  x:%5.2f       y:%5.2f\n", $x, $y if $verbose>1;
	$points{$x} = $y;
}
say "Sorted:";
my @sorx = (sort {$a <=> $b} keys %points);
my @sory = map {$points{$_}} @sorx;
for my $i (0 .. $#sorx) {
	printf "  x:%5.2f       y:%5.2f\n", $sorx[$i], $sory[$i] if $verbose>0;
}
my $curx = 0;

sub sminmax {
	#printf "[%4s <= %4s <= %4s]\n", $_[0], $_[1], $_[2];
}
sub sminmaxx {
	my ($s, $e, $x, $sorxar) = @_;
	printf "[%4s <= %4s <= %4s] = ",
		$$sorxar[$s], $x, $$sorxar[$e];
}
sub sminmaxy {
	my ($s, $e, $x, $sorxar, $soryar) = @_;
	# ((x-xs)/(xe-xs)) * (ye-ys) + ys
	my $y = (($x-$$sorxar[$s]) / ($$sorxar[$e]-$$sorxar[$s])) *
		($$soryar[$e]-$$soryar[$s]) + $$soryar[$s];
	print "$y\n";
	return $y;
}

for (my $x = $sorx[0]; $x<$sorx[-1]; $x += .5) {
	my ($s, $e);
	($s, $e) = (0,1);
	if ($x >= $sorx[$s] && $x <= $sorx[$e]) {
		sminmaxx($s, $e, $x, \@sorx);
		sminmaxy($s, $e, $x, \@sorx, \@sory);
	} else {
		($s, $e) = (1,2);
		if ($x >= $sorx[$s] && $x <= $sorx[$e]) {
			sminmaxx($s, $e, $x, \@sorx);
			sminmaxy($s, $e, $x, \@sorx, \@sory);
		} else {
			($s, $e) = (2,3);
			if ($x >= $sorx[$s] && $x <= $sorx[$e]) {
				sminmaxx($s, $e, $x, \@sorx);
				sminmaxy($s, $e, $x, \@sorx, \@sory);
			} else {
				($s, $e) = (3,4);
				if ($x >= $sorx[$s] && $x <= $sorx[$e]) {
					sminmaxx($s, $e, $x, \@sorx);
					sminmaxy($s, $e, $x, \@sorx, \@sory);
				}
			}
		}
	}
}
exit;

	#if ($sory[$i] == 0) { say "Term$i: Zero. Skipping"; }

for my $i (0 .. $#sorx) {
	#my $denom = join("*", map {"\$x-($_)"} @sorx);
	my $isolator = join("*",
		map {"(x$_-x)"} grep {$_!=$i} (0..$#sorx));
		#map {"$sorx[$_]-x"} grep {$_!=$i} (0..$#sorx));
	#my $numer = "$sory[$i] * $isolator";
	my $numer = "y$i * $isolator";
	#my $denom = "$sorx[$i] - x + $isolator";
	my $denom = "x$i - x + $isolator";
	#my $term = "$sory[$i] * $numer / $denom";
	my $term = "$numer / ($denom)";
	say "Term$i:  $term";
	push(@terms, $term);
}
# plot (-1 / (1-((x-(-3))*(x-(5))))) + (1 / (1-((x-(-3))*(x-(5)))))
# plot (-1 / (1-((x-(-3))*(x-(1))*(x-(2))*(x-(5))*(x-(6))))) + (1 / (1-((x-(-3))*(x-(1))*(x-(2))*(x-(5))*(x-(6)))))


print join("; ", map {"x$_=$sorx[$_]; y$_=$sory[$_]"} (0..$#sorx));
print ";\n";
say "plot ", join(" + ", map {"($_)"} @terms);
# yterm
# numerator should be 1 at x=xterm
# denominator should be 1 at x=xterm
#  (OR numerator and denominator should be 0 at x=xterm)
# numerator should be 0 at x=xother_term
# The following experiments were attempts at figuring out a way
# to generate an equation with one term for each value (a sort of curve fit, without
# any iteration and with fixed points exactly-specified).
# I gave up on this idea anyway, because the interpolation would result in
# mouse move values that were unreasonable for the user with accessibility issues.
# Broken test equations follow:
	#               (1-(x-x1)) * (1-(x-x1)(x-x2)(x-x3))                    (x-x1) * (x-x3)
	#   y1 * ------------------------------------------ +  y2 * ------------------------------- + ....
	#                (1-(x-x1))                                    (1-((x-x1) * (x-x2) * (x-x3))


	#               (x-x2) * (x-x3)                    (x-x1) * (x-x3)
	#   y1 * ------------------------------- +  y2 * ------------------------------- + ....
	#          (1-(x-x1)) * (x-x2) * (x-x3)           (1-((x-x1) * (x-x2) * (x-x3))


# -3	-1
# 1	0
# 2	0
# 5	1
# 6	0
	#
