#!/bin/sh
dir=$1
key=$2
crx=$3

cmd.exe /c "start /wait chrome --pack-extension=$dir --pack-extension-key=$key --enable-experimental-extension-apis \
    --no-message-box" &&
  cp $dir.crx $crx
