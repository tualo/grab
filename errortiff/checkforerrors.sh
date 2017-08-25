#!/bin/bash

#x=$(find /imagedata/ -cmin -5 -name "*.bin")
# find /imagedata/ -cmin -5 -name "*.bin" -exec process {} \;

sleep(5)
if [[ $# -eq 0 ]] ; then
  echo "ok, we need a filename"
  exit
fi

body=$(cat errormail.txt)
if [ -f $1 ]; then
  /root/errortiff/errortiff -f "$f" -o "$f.jpg"
  echo $body | mail -A "$f.jpg" -s "THB: Bildfehler an Maschine 310" "thomas.hoffmann@tualo.de"
  mv "$f" "$f.xbin"
fi
