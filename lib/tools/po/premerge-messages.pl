#!/usr/bin/perl
#
# Merge the creation date and charset from a def.po file with a ref.pot file
# so that msgmerge does not complain.
#
# This is a brutish hack.
#
use strict;

my $start = '^"Project-Id-Version';
my $end = '^\s*$';
if ($ARGV[0] == '-2') {
  shift; # Keep portion of xx.po header in newly created xx_YY.po
  $start = '^"POT-Creation-Date';
  $end = '^"MIME-Version';
}
my $def_po = shift;
my $ref_pot = shift;

my @header;
my $saving = 0;
open(FD, "<$def_po") or exit;
while (<FD>) {
  chomp;
  if (/$start/) {
    $saving = 1;
  }

  if (/$end/) {
    $saving = 0;
  }

  if ($saving) {
    push(@header, "$_\n");
  }
}
close(FD);

my @lines;
my $replacing = 0;
open(FD, "<$ref_pot") || die;
while (<FD>) {
  if (/$start/) {
    push(@lines, @header);
    $replacing = 1;
  }

  if (/$end/) {
    $replacing = 0;
  }

  unless ($replacing) {
    push(@lines, $_);
  }
}
close(FD);

print @lines;
