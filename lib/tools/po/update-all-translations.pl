#!/usr/bin/perl
#
# Update all translations and create a report.
#
use strict;
use Cwd;
use Data::Dumper;
use Getopt::Long;
use Symbol;

my %OPTS;
$OPTS{'VERBOSE'} = 0;
$OPTS{'MAKE_BINARY'} = 0;
$OPTS{'PATTERN'} = '';
$OPTS{'DRY_RUN'} = 0;
$OPTS{'REMOVE_OBSOLETE'} = 0;
chomp(my $MAKE = `(which gmake || which make) 2>/dev/null`);
die "Missing make" unless $MAKE;

GetOptions('make-binary!' => \$OPTS{'MAKE_BINARY'},
	   'pattern=s' => \$OPTS{'PATTERN'},
	   'dry-run!' => \$OPTS{'DRY_RUN'},
	   'verbose|v!' => \$OPTS{'VERBOSE'},
	   'compendium!' => \$OPTS{'COMPENDIUM'},
	   'remove-obsolete!' => \$OPTS{'REMOVE_OBSOLETE'},
	   'po=s' => \$OPTS{'PO'},
	   'permissions!' => \$OPTS{'PERMISSIONS'},
	   'svn-add!' => \$OPTS{'SVN_ADD'});

my @failures = ();
my @warnings = ();
my $basedir = cwd();
$basedir =~ s{(/.*)/(lib|themes|modules|install|upgrade)/.*?$}{$1};

# Find po dirs in modules/* themes/* install upgrade
my @PO_DIRS = glob "$basedir/modules/*/po $basedir/themes/*/po $basedir/[iu][np][sg]*/po";
@PO_DIRS = grep(/$OPTS{PATTERN}/, @PO_DIRS) if $OPTS{'PATTERN'};
@PO_DIRS = grep($_ !~ m{modules/core/po$}, @PO_DIRS) if $OPTS{'COMPENDIUM'};

if ($OPTS{'PERMISSIONS'}) {
  my $poParam = $OPTS{'PO'} ? "$OPTS{PO}.po" : '*.po';
  foreach my $poDir (@PO_DIRS) {
    chmod(0755, $poDir);
    chdir $poDir;
    &my_system("chmod 644 $poParam 2> /dev/null");
  }
  print STDERR "Updated permissions for $poParam in " . scalar(@PO_DIRS) . " directories.\n";
  exit;
}

if ($OPTS{'SVN_ADD'}) {
  my $poParam = $OPTS{'PO'} ? "$OPTS{PO}.po" : '*.po';
  foreach my $poDir (@PO_DIRS) {
    my %svn = ();
    chdir $poDir;
    open(SVN, 'svn status --non-interactive |') or die;
    while (<SVN>) {
      m|^\?      (.*\.po)$| and $svn{$1} = 1;
    }
    close SVN;
    @_ = glob $poParam;
    chdir '..';
    foreach my $poFile (@_) {
      if (exists $svn{$poFile} and $poFile ne 'messages.po') {
	my $code = substr($poFile, 0, -3);
	my $moFile;
	my $localeDir;
	my $moPath;
	if ($poDir =~ m{(upgrade|install)/po$}) {
	  $localeDir = "../locale/$code";
	  $moFile = "gallery2_$1.mo";
	  $moPath = "$localeDir/LC_MESSAGES/$moFile";
	} elsif (-d "locale") {
	  # Remove this when we stop supporting 2.2 style per-plugin locale dirs
	  $localeDir = "locale/$code";
	  $moFile = "*.mo";
	  $moPath = "$localeDir/LC_MESSAGES/$moFile";
	} else {
	  $moFile = "$code.mo";
	  $moPath = "po/$moFile";
	  my_system("svn add $moPath");
	}

	my_system("svn add po/$poFile");
	my_system("svn add $localeDir") if $localeDir;
	my_system("svn propset svn:mime-type application/octet-stream $moPath");
      }
    }
  }
  exit;
}

if ($OPTS{'MAKE_BINARY'}) {
  # Make all .mo files binary in SVN.
  chdir $basedir;
  my @MO_FILES = glob "modules/*/po/*.mo themes/*/po/*.mo locale/*/*/*.mo";
  my_system("svn propset svn:mime-type application/octet-stream " . join(' ', @MO_FILES));

  # Remove this when we stop supporting 2.2 style per-plugin locale dirs
  @MO_FILES = glob "modules/*/locale/*/*/*.mo themes/*/locale/*/*/*.mo";
  if (@MO_FILES) {
    my_system("svn propset svn:mime-type application/octet-stream " . join(' ', @MO_FILES));
  }
  exit;
}

my $TARGET = $OPTS{'REMOVE_OBSOLETE'} ? 'all-remove-obsolete' : 'all';
$TARGET = 'compendium' if $OPTS{'COMPENDIUM'};
foreach my $poDir (@PO_DIRS) {
  (my $printableDir = $poDir) =~ s|$basedir.||;
  print STDERR "$printableDir: ";
  unless ($OPTS{'DRY_RUN'}) {
    if (-f "$poDir/GNUmakefile") {
      chdir $poDir;
      my $poParam = '';
      if (!$OPTS{'PO'} || (-f "$OPTS{PO}.po" && ($poParam = 'PO=' . $OPTS{'PO'}))) {
	my_system("$MAKE $TARGET clean QUIET=1 NOCREATE=1 $poParam 2>&1")
	  and print "FAIL!\n"
	    and push(@failures, $poDir);
      } else {
	print "Missing $OPTS{PO}.po!\n";
	push(@warnings, $poDir);
      }
    } else {
      print "Missing GNUmakefile!\n";
      push(@warnings, $poDir);
    }
  }
}

sub my_system {
  my $cmd = shift;
  if ($OPTS{'VERBOSE'}) {
    print STDERR "System: $cmd\n";
  }
  system($cmd);
}

if (@warnings) {
  print "\n\n";
  print scalar(@warnings) . " warnings\n";
  foreach (@warnings) {
    print "\t$_\n";
  }
}
if (@failures) {
  print "\n\n";
  print scalar(@failures) . " failures\n";
  foreach (@failures) {
    print "\t$_\n";
  }
  exit 1;
}

sub out {
  my ($file, $indent, $msg) = @_;
  print $file " " x ($indent * 4) . $msg . "\n";
}

