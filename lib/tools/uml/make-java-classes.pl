#!/usr/local/bin/perl
use strict;
use File::Basename;

my $GALLERYDIR = "../../..";
my $TOOLDIR = "..";
my $TMPDIR = "tmp";
my $OUTPUTDIR = "tmp";
my $SAXON_JAR = $ENV{'SAXON_JAR'} ? $ENV{'SAXON_JAR'} : "/usr/local/share/java/classes/saxon.jar";
my $SAXON = "java -jar $SAXON_JAR";
my $XSLFILE = "JavaClasses.xsl";

foreach my $module (<$GALLERYDIR/modules/*>) {
  &generate(basename($module));
}

sub generate {
  my $module = shift;
  mkdir($TMPDIR) unless -d $TMPDIR;
  mkdir("$TMPDIR/$module") unless -d "$TMPDIR/$module";

  foreach my $classFile (<$GALLERYDIR/modules/$module/classes/*.class>) {
    (my $base = basename($classFile)) =~ s/\..*?$//;
    my $xmlFile = "$TMPDIR/$module/$base.xml";

    if (! -f $xmlFile || ((stat($classFile))[9] > (stat($xmlFile))[9])) {
      system("perl $TOOLDIR/bin/extractClassXml.pl " .
	     "--dtd=../../../dtd/GalleryClass2.0.dtd " .
	     "--stub-ok " .
	     "--out $xmlFile $classFile " .
	     "--quiet");
      if (-z $xmlFile) {
	unlink($xmlFile);
      }
    }

    if (-f $xmlFile) {
      my $javaFile = "$TMPDIR/$module/$base.java";
      if (! -f $javaFile || ((stat($xmlFile))[9] > (stat($javaFile))[9])) {
	system("$SAXON $xmlFile $XSLFILE | perl -pe 's/\@\@package\@\@/$module/' > $javaFile");

	open(FD, "<$javaFile") || die;
	chomp(my @lines = <FD>);
	close(FD);

	unshift(@lines, "package $module;");
	for (my $i = 0; $i < @lines; $i++) {
	  $lines[$i] =~ s/extends GalleryPersistent//;

	  # HACK: make all non module classes "extends"
	  # clauses extend from core.Xxx
	  if ($module ne "core") {
	    $lines[$i] =~ s/extends (\S+)/extends core.$1/;
	  }
	}

	open(FD, ">$javaFile") || die;
	print FD join("\n", @lines);
	close(FD);
      }
    }
  }
}

__END__


TOOLDIR ?= $(GALLERYDIR)/lib/tools
XSLFILE ?= $(TOOLDIR)/uml/JavaClasses.xsl
G2_TMPDIR ?= tmp

include $(TOOLDIR)/GNUmakefile.inc

CLASSFILES = $(wildcard $(CLASSDIR)/*.class)
XMLFILES = $(patsubst $(CLASSDIR)/%.class,%.xml,$(CLASSFILES))
JAVAFILES = $(patsubst %.xml,%.java,$(XMLFILES))

java: $(G2_TMPDIR) $(JAVAFILES)

$(G2_TMPDIR):
	mkdir $(G2_TMPDIR)

%.java: $(G2_TMPDIR)/%.xml $(XSLFILE)
	if [ -f $< ]; then $(SAXON) $< $(XSLFILE) \
		| perl -pe 's/\@\@package\@\@/$(PACKAGE)/' \
		> $@; fi

$(G2_TMPDIR)/%.xml: $(CLASSDIR)/%.class
	perl $(TOOLDIR)/bin/extractClassXml.pl \
		--dtd=../$(TOOLDIR)/dtd/GalleryClass2.0.dtd \
		--stub-ok \
		--out $@ \
		$^ 
	if [ -f $@ ]; then $(VALIDATOR) $@; fi

clean:
	rm -rf $(G2_TMPDIR)

scrub: clean
	rm -f *.java

# Gmake will automatically delete $(G2_TMPDIR)/*.xml files after creating .java files
# because it thinks that they're intermediate files.  But, we want to save
# them (for now), so mark them as PRECIOUS.
#
.PRECIOUS: $(G2_TMPDIR)/%.xml
