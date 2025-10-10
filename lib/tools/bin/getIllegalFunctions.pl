#!/usr/local/bin/perl
#
use strict;

my $versionXmlUrl = 'http://cvs.php.net/co.php/phpdoc/xsl/version.xml?r=1.16&p=1';
my $versionXmlFile = '/tmp/version.xml';

unless (-f $versionXmlFile) {
  system("wget -O $versionXmlFile $versionXmlUrl") and die "unable to wget $versionXmlUrl";
}

open(FD, "<$versionXmlFile") || die;

print "<?php\n";
print "\$illegalFunctions = array();\n";
while (<FD>) {
  # only get functions
  next unless /\<function/;

  # only get ones that apply to PHP 4
  next unless /PHP 4/;

  # Decode entities
  s/&gt;/>/g;
  s/&lt;/</g;

  my ($function, $comparison) = (/<function name=['\"](.*?)['\"].*PHP 4(.*?)[,\"]/);
  $comparison =~ s/\s//g;

  # No comparison means it works on all PHP 4
  next unless $comparison;

  # Weird cases
  next if ($comparison =~ /<=?4\.0\.0/);
  next if ($comparison =~ /<=?4\.2\.3/);
  next if ($comparison =~ /<=?4\.0\.4/);
  next if ($comparison =~ /4\.0\.[346]only/);

  # Anything > or >= 4.0* is fine
  next if ($comparison =~ />=?4\.0/);

  # Anything >= 4.1.0 is fine
  next if ($comparison =~ />=4\.1\.0/);

  printf('%-70s // %s', "\$illegalFunctions[] = '$function';", $comparison);
  print "\n";
}

close(FD);

print "?>\n";
