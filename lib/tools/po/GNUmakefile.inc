#
# Gallery .mo files makefile
#
# This makefile is based on the one provided with the Horde project
# http://www.horde.org/.  As such, it inherits the license from the
# original version.  You can find that license here:
#
# http://cvs.horde.org/co.php/horde/COPYING?r=2.1
#
# I'm not exactly sure what the license restrictions are in this case,
# but I want to give full credit to the original authors:
#
# Copyright 2000-2002 Joris Braakman <jbraakman@yahoo.com>
# Copyright 2001-2002 Chuck Hagenbuch <chuck@horde.org>
# Copyright 2001-2002 Jan Schneider <jan@horde.org>
#
# $Id: GNUmakefile.inc 18114 2008-10-04 15:39:28Z mindless $
#
MODULE_DIR = $(shell dirname "`pwd`")
TYPE_DIR = $(shell dirname "$(MODULE_DIR)")
APPLICATION ?= $(shell basename "$(TYPE_DIR)")_$(shell basename "$(MODULE_DIR)")
PHP ?= php

# Let PHP parse the config.php file to get the g2data directory.
G2DATA = $(shell echo '<?php include("$(GALLERYDIR)/lib/support/GallerySetupUtilities.class"); $$gallery = new GallerySetupUtilitiesStub(); @include("$(GALLERYDIR)/config.php"); print $$gallery->getConfig("data.gallery.base"); ?>' | $(PHP) -q 2>&1)

ifdef QUIET
    MSGFMT_FLAGS =
else
    MSGFMT_FLAGS = -v
endif

MSGFMT = msgfmt $(MSGFMT_FLAGS) -c -o
MSGFMTSOL = msgfmt $(MSGFMT_FLAGS) -o

PO ?= *
LANGPO = $(shell echo $@ | perl -ne 'chomp; print if (s/^(..)_..\.po/$$1.po/ and -s)')
COMPENDIUM = $(shell echo $@ | perl -ne 'chomp; print "-C $$_" if (s/^(..)_..\.po/$$1.po/ and -s)')

all: install

all-remove-obsolete: update remove-obsolete install

messages.po: alwaysbuild
	@$(PHP) -f "$(TOOLDIR)"/po/extract.php "$(MODULE_DIR)" > strings.raw.tmp
	@# Don't extract right to strings.raw because extract.php tries to preserve
	@# some old data from strings.raw and the > redirect blanks it out.
	@mv strings.raw.tmp strings.raw
	@xgettext --keyword=_ -C --no-location --add-comments=HINT: \
		  --msgid-bugs-address=gallery-translations@lists.sourceforge.net strings.raw
	@sed '1,4 d' < messages.po > messages.tmp
	@cat "$(TOOLDIR)"/po/header.txt messages.tmp > messages.po
	@$(RM) messages.tmp

%.po: messages.po
	@perl "$(TOOLDIR)"/po/premerge-messages.pl $@ messages.po > messages.tmp
	@# ignore errors from msgmerge, we'll catch them on the msgfmt command:
	@-if [ -s messages.tmp ]; then \
	    msgmerge --quiet --width=95 $(COMPENDIUM) $@ messages.tmp > $@.tmp; \
	    if [ -s $@.tmp ]; then mv $@.tmp $@; else $(RM) $@.tmp; fi; \
	    perl -pi "$(TOOLDIR)"/po/header.pl $@; \
	  elif [ -z "$(NOCREATE)" ]; then \
	    if [ -n "$(LANGPO)" ]; then \
	      perl "$(TOOLDIR)"/po/premerge-messages.pl -2 messages.po $(LANGPO) > $@; \
	    else cp messages.po $@; fi; \
	    perl -pi "$(TOOLDIR)"/po/header.pl $@; \
	  fi
	@$(RM) messages.tmp

alwaysbuild:

clean:
	@$(RM) messages.po

update: $(PO).po

check-g2data:
	@if ! test -d '$(G2DATA)'; then \
	    echo "************************************************************"; \
	    echo "WARNING: We detected the following as your g2data dir but"; \
	    echo "this directory does not appear to exist.  We will be unable"; \
	    echo "to copy new .mo files into the g2data/locale directory until"; \
	    echo "this is fixed."; \
	    echo ""; \
	    echo '$(G2DATA)'; \
	    echo ""; \
	    echo "Alternatively, point your browser at gallery2/lib/support, "; \
	    echo "click the \"Filesystem Permissions\" link and then click "; \
	    echo "the 'Make the locale folder read/write' link"; \
	    echo "************************************************************"; \
	fi

install: check-g2data update
	@if ! test -z "$(DEBUG)"; then set -x; fi; \
	if test "$(OSTYPE)" = "solaris"; then \
	    echo "You'll probably get some warnings on Solaris. This is normal."; \
	    MSGFMT_CMD="$(MSGFMTSOL)"; \
	else \
	    MSGFMT_CMD="$(MSGFMT)"; \
	fi; \
	FAIL=0; \
	COPY_FAIL=0; \
	echo "Updating $(PO)"; \
	for LOCALE in `ls $(PO).po | grep -v messages.po | sed 's/\.[^.]*$$//g'`; do \
	    INSTALL_FILE=""; \
	    if test -z "$${QUIET}"; then echo $${LOCALE}; fi; \
	    if test -d ../locale; then \
		if ! "$(TOOLDIR)"/po/shtool mkdir -p ../locale/$${LOCALE}/LC_MESSAGES; then \
		    echo "Could not create directory: ../locale/$${LOCALE}/LC_MESSAGES"; \
		    exit 1; \
		fi; \
		MO_FILE=../locale/$${LOCALE}/LC_MESSAGES/$(APPLICATION).mo; \
	    elif test "$(APPLICATION)" = "gallery2_install" \
		   || test "$(APPLICATION)" = "gallery2_upgrade";  then \
		if ! $(TOOLDIR)/po/shtool mkdir -p ../../locale/$${LOCALE}/LC_MESSAGES; then \
		    echo "Could not create directory: ../../locale/$${LOCALE}/LC_MESSAGES"; \
		    exit 1; \
		fi; \
		MO_FILE=../../locale/$${LOCALE}/LC_MESSAGES/$(APPLICATION).mo; \
	    else \
		MO_FILE=$${LOCALE}.mo; \
		if test -d '$(G2DATA)'; then \
		    INSTALL_DIR='$(G2DATA)'/locale/$${LOCALE}/LC_MESSAGES; \
		    INSTALL_FILE="$${INSTALL_DIR}/$(APPLICATION).mo"; \
		fi; \
	    fi; \
	    $(PHP) -f $(TOOLDIR)/po/trim-po.php `pwd`/$${LOCALE}.po > tmp.po; \
	    if $${MSGFMT_CMD} $${MO_FILE} tmp.po; then \
		if test -n "$${QUIET}"; then \
		    echo -n '.'; \
		fi; \
		if test -n "$${INSTALL_FILE}"; then \
		    if ! test -d "$${INSTALL_DIR}"; then \
			echo ">> Creating $${INSTALL_DIR}"; \
			if ! $(TOOLDIR)/po/shtool mkdir -p $${INSTALL_DIR}; then \
			    echo "** Unable to create $${INSTALL_DIR}!"; \
			    COPY_FAIL=1; \
			fi \
		    fi; \
		    if test -d "$${INSTALL_DIR}"; then \
			if cp $${MO_FILE} $${INSTALL_FILE}; then \
			    chmod 777 $${INSTALL_FILE} 2>/dev/null ; \
			else \
			    echo "** Unable to install $${INSTALL_FILE}!"; \
			    COPY_FAIL=1; \
			fi; \
		    fi \
		fi \
	    else \
		echo -n "Fail: $${LOCALE}"; \
		FAIL=1; \
	    fi; \
	    $(RM) tmp.po; \
	    if test -z "$${QUIET}"; then echo; fi; \
	done; \
	if test $${COPY_FAIL} -gt 0; then \
	    echo "************************************************************"; \
	    echo "WARNING: Failed to copy some .mo files to the g2data/locale"; \
	    echo "directory! Try changing the permissions on your g2data dir: "; \
	    echo ""; \
	    echo 'chmod -R 777 $(G2DATA)/locale'; \
	    echo ""; \
	    echo "Alternatively, point your browser at gallery2/lib/support, "; \
	    echo "click the \"Filesystem Permissions\" link and then click "; \
	    echo "the 'Make the locale folder read/write' link"; \
	    echo "************************************************************"; \
	fi; \
	if test $${FAIL} -gt 0; then exit 1; fi;
	@echo '';

compendium: messages.po
	@perl -pi -e 's|^("Content-Type: text/plain; charset)=CHARSET|$$1=UTF-8|' messages.po
	@for POFILE in `ls $(PO).po | grep -v messages.po`; do \
	    echo -n "$${POFILE} "; \
	    COMPENDIUM=; \
	    for SRC in `ls $(TOOLDIR)/../../*/*/po/$${POFILE} $(TOOLDIR)/../../*/po/$${POFILE}`; \
	    do \
		COMPENDIUM="$${COMPENDIUM} -C $${SRC}"; \
	    done; \
	    msgmerge --width=95 -N $${COMPENDIUM} \
		     --suffix= --update $${POFILE} messages.po; \
	    if [ `svn diff $${POFILE} | wc -l` -lt 16 ]; then \
		svn revert --quiet $${POFILE}; \
	    fi; \
	done;

create-from-core:
	@for POFILE in `ls $(TOOLDIR)/../../modules/core/po/$(PO).po | grep -v messages.po`; do \
	    PO=`basename $${POFILE}`; \
	    if [ ! -f $${PO} ]; then \
		perl -p -e '$$x++ if /^msgid/; exit if $$x eq 2' $${POFILE} > $${PO}; \
	    fi; \
	done;

remove-obsolete:
	@for LOCALE in `ls $(PO).po | grep -v messages.po | sed 's/\.[^.]*$$//g'`; do \
		msgattrib --no-obsolete --width=95 --output-file=$${LOCALE}.po $${LOCALE}; \
	done;
