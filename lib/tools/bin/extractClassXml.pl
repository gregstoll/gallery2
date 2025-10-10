#!/usr/local/bin/perl
#
use strict;
use File::Basename;
use Getopt::Long;

my $DTD;
my $OUTFILE;
my $STUB_OK = 0;
my $QUIET = 0;
my $OUT_DIR;

GetOptions("dtd:s" => \$DTD,
	   "out:s" => \$OUTFILE,
	   "out-dir:s" => \$OUT_DIR,
	   "stub-ok+" => \$STUB_OK,
	   "quiet!" => \$QUIET);

foreach my $file (@ARGV) {
  my $tagCount = 0;
  my $base = basename($file);
  $base =~ s/\..*?$//;
  my $xml = $OUTFILE || "$OUT_DIR/$base.xml";
  my $schemaName = undef;

  open(IFD, "<$file") || die;
  open(OFD, ">$xml") || die;
  print OFD "<!DOCTYPE class SYSTEM \"$DTD\">\n";
  print OFD "<class>\n";
  while (<IFD>) {
    if (s/.*\@g2\s*//) {
      $tagCount++;
      print OFD $_;

      # NOTE!  Keep this in sync with the similar block in generate-entities.php
      # and generate-maps.php
      if (m|<class-name>(.*)</class-name>|) {
	($schemaName = $1) =~ s/^Gallery//;
	# Shorten some table names to fit Oracle's 30 char name limit..
	$schemaName =~ s/Preferences/Prefs/;
	$schemaName =~ s/Toolkit/Tk/;
	$schemaName =~ s/TkOperation/TkOperatn/;
      }

      if (m|<schema>|) {
	print OFD "<schema-name>$schemaName</schema-name>\n";
      }
    }
  }
  close(IFD);
  print OFD "</class>\n";
  close(OFD);

  # It's gotta have more than the class-name, schema-version tags.
  #
  if ($tagCount == 0 || ($tagCount <= 2 && !$STUB_OK)) {
    print STDERR "No tags detected\n" unless ($QUIET);
    unlink($xml);
  }
}

exit 0;
