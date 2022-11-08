#!/usr/bin/env bash

set -a

GALLERY2_BASE=/gallery2-data
GALLERY2_SRC_HOME=/app

for i in es_AR fr pt_BR de; do
  LANGUAGE_DIR="$GALLERY2_BASE/locale/$i/LC_MESSAGES"
  mkdir -p $LANGUAGE_DIR
  cp $GALLERY2_SRC_HOME/modules/core/po/$i.mo $LANGUAGE_DIR/modules_core.mo
done
