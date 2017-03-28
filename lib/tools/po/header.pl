#!/usr/bin/perl
# Ensure correct header (Id tag, GPL header, Project-Id-Version)
# Usage: perl -pi{ext} header.pl {po_file}
# messages.po should exist in current dir

if ($. == 1) {
    $_ = "# \$Id\$\n" unless /^# \$Id/;
} elsif (not $x) {
    $x = 1 if /^(?:([^#])|#.*USA)/;
    undef $_ unless $1;
    print `sed -n "2,/USA/ p" messages.po` if $x;
} elsif (s/^("Project-Id-Version:).*$/$1 Gallery: /) {
    chomp;
    if ($f = -f '../module.inc' ? '../module.inc' : (-f '../theme.inc' ? '../theme.inc' : '')) {
      $m = `perl -naF\\' -e 'do { print \$F[1]; exit } if /setName/' $f`;
      $m .= ' Theme' if ($f eq '../theme.inc');
      $v = `perl -naF\\' -e 'do { print \$F[1]; exit } if /setVersion/' $f`;
    } else {
      $m = (($m = `pwd`) =~ m|install.po|) ? 'Installer' : 'Upgrader';
      $f = '../../modules/core/module.inc';
      $v = `perl -naF\\' -e 'do { print \$F[1]; exit } if /setGalleryVersion/' $f`;
      $v =~ s/-.*$//;
    }
    $_ .= "$m $v\\n\"\n";
} else {
    s/^("Language-Team:.*?<)LL\@li.org(.*)$/${1}gallery-translations\@lists.sourceforge.net$2/;
    s/^("Content-Type:.*?=)CHARSET(.*)$/${1}UTF-8$2/;
}

