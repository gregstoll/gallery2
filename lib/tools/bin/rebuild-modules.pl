#!/usr/bin/perl
use strict;
chomp(my $CURDIR = `pwd`);
chomp(my $MAKE = `(which gmake || which make) 2>/dev/null`);

if (!$MAKE) {
  die "Unable to locate 'make' or 'gmake'";
}

my @MAKEFILES = <modules/*/classes/GNUmakefile>;
foreach my $makefile (@MAKEFILES) {
  (my $module = $makefile) =~ s|(modules/.*?)/.*|$1|;
  print STDERR "Building $module\n";
  chdir("$CURDIR/$module/classes") || die;
  system("$MAKE -s clean && $MAKE -s && $MAKE -s clean") and die;
}
