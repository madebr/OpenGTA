#!/bin/bash

TMP_FILE=/tmp/picasa_lst.$$

function fetch() {
  wget -O $TMP_FILE 'http://picasaweb.google.com/data/feed/base/user/under.northern.sky/albumid/5015283825893823185?kind=photo&alt=rss&hl=en_US'
}

function extract() {
  local _file=$1
  echo 'cat /rss/channel/item[*]/enclosure|/rss/channel/item[*]/link' \
    | xmllint --shell $_file | grep http | sed \
      -e 's|^.*url="||' -e 's|".*$||' -e 's|<\/*link>||g'
}

fetch
extract $TMP_FILE | {
  while read a; do
    basename $a
    read b
    echo $b
    echo
  done
}
rm -f $TMP_FILE
