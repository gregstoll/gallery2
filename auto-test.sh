#!/usr/bin/env bash

set -a
set -u

SCRIPT_PATH=$(dirname $0)

function slugify() {
  iconv -t ascii//TRANSLIT \
  | tr -d "'" \
  | sed -E 's/[^a-zA-Z0-9]+/-/g' \
  | sed -E 's/^-+|-+$//g' \
  | tr "[:upper:]" "[:lower:]"
}

function run_test() {
  URL="$SERVER_URL/lib/tools/phpunit/index.php?filter=$1"

  BASENAME=$(echo "$1" | slugify)
  curl -# \
    "$URL" \
    -w"time_total=%{time_total}\n" \
    -b "$SCRIPT_PATH/tmp-cookies.txt" \
    -H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8' \
    -H 'Cache-Control: no-cache' \
    -H 'Connection: keep-alive' \
    -H 'Pragma: no-cache' \
    -H 'Sec-GPC: 1' \
    -H 'Referer: http://host.docker.internal:10000/lib/tools/phpunit/index.php?filter=%5EAlbumTest%24' \
    -H 'Upgrade-Insecure-Requests: 1' \
    -H 'User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36' \
    --compressed \
    --insecure \
    | tee $BASENAME.out \
    | grep updateStats \
    | sed -E "s/(.*getElementById\(')([^']*)('.*)/\2/"

  tail -1 $BASENAME.out
}

function do_login() {

  USERNAME=$1
  PASSWORD=$2
  G2_AUTH_TOKEN=$(get_authToken)

  curl -XPOST \
    -b $SCRIPT_PATH/tmp-cookies.txt \
    -c $SCRIPT_PATH/tmp-cookies.txt \
    "$SERVER_URL/main.php" \
    -H 'Cache-Control: no-cache' \
    -H 'Connection: keep-alive' \
    -H 'Content-Type: application/x-www-form-urlencoded' \
    -H "Origin: $SERVER_URL" \
    -H 'Pragma: no-cache' \
    --data-raw 'g2_return=%2Fmain.php%3F&g2_formUrl=%2Fmain.php%3Fg2_view%3Dcore.UserAdmin%26g2_subView%3Dcore.UserLogin&g2_controller=core.UserLogin&g2_form%5BformName%5D=UserLogin&g2_form%5Baction%5D%5Blogin%5D=Login' \
    --data "g2_authToken=${G2_AUTH_TOKEN}" \
    --data "g2_form%5Busername%5D=$USERNAME" \
    --data "g2_form%5Bpassword%5D=$PASSWORD" \
    --compressed \
    --insecure
}

function get_authToken() {
  curl "$SERVER_URL/main.php?g2_view=core.UserAdmin&g2_subView=core.UserLogin" \
    -H 'Cache-Control: no-cache' \
    -H 'Connection: keep-alive' \
    -H 'Pragma: no-cache' \
    --compressed \
    --insecure |
    grep "g2_authToken" |
    sed -E 's/(.*value=")([^"]*)(".*)/\2/' |
    head -1
}

function do_securitycheck() {

  PASSWORD=$1

  curl \
    -b $SCRIPT_PATH/tmp-cookies.txt \
    -c $SCRIPT_PATH/tmp-cookies.txt \
    "$SERVER_URL/lib/tools/phpunit/index.php" \
    -H 'Cache-Control: no-cache' \
    -H 'Connection: keep-alive' \
    -H 'Content-Type: application/x-www-form-urlencoded' \
    -H 'Pragma: no-cache' \
    --data "password=$PASSWORD" \
    --compressed \
    --insecure

}

function get_setupsid() {
  curl \
    -b $SCRIPT_PATH/tmp-cookies.txt \
    -c $SCRIPT_PATH/tmp-cookies.txt \
    "$SERVER_URL/lib/tools/phpunit/index.php" \
    -H 'Cache-Control: no-cache' \
    -H 'Connection: keep-alive' \
    -H 'Pragma: no-cache' \
    --compressed \
    --insecure
}

SERVER_URL=$1
SCOPE=$2
USERNAME=${USERNAME}
PASSWORD=${PASSWORD}

get_setupsid
do_login $USERNAME $PASSWORD
do_securitycheck $PASSWORD
run_test "$SCOPE"
