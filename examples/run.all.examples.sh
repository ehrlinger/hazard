#!/bin/sh
#
# Run all these examples sequentially.
HZEXAMPLES=`pwd`
export HZEXAMPLES

for fl in `ls *.sas`
do
  echo "Editing $fl ..."
  sas9.1 $fl
  base=`basename $fl .sas`
  if [ $# -eq 1 ]; then
    mv $base.log $base.$1.log
    mv $base.lst $base.$1.lst
  fi
done

exit 0

