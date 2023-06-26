#!/usr/bin/env bash

set -u
set -a

SOURCE_VOLUME=$1
TARGET_VOLUME=$2

docker container run --rm -it -v $SOURCE_VOLUME:/from -v $TARGET_VOLUME:/to alpine ash -c "cd /from ; cp -av . /to"
