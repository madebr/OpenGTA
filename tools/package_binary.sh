#!/bin/bash

DLL_NAMES_W32=*.dll #"SDL.dll"

function binary() {
  local ex_name=$1
  if [ "$HOST" == "WIN32" ]; then
    echo $ex_name".exe"
    echo $DLL_NAMES_W32
  else
    echo $ex_name
  fi
}

function host_txt() {
  if [ "$HOST" == "WIN32" ]; then
    unix2dos $@
  fi
  echo "$@"
}

function list_files() {
  binary viewer
  echo licenses/*
  host_txt license.txt readme.txt cvs_changelog.txt
  host_txt doc/using_mods.txt
}

R_DATE=$(cat ogta_version)
if [ "$HOST" == "WIN32" ]; then
  ZIP_FILE_NAME=ogta_sneak-preview-${R_DATE}_win32
  
  list_files | xargs zip $ZIP_FILE_NAME
else
  TAR_FILE_NAME=ogta_sneak-preview-${R_DATE}_linux.tar.bz2

  list_files | xargs tar jvcf $TAR_FILE_NAME
fi
