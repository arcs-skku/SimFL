#!/bin/bash
CURDIR=`pwd`
SIMFL=$(readlink -f $(dirname ${BASH_SOURCE[0]}))
cd $SIMFL/lib

for f in *; do
	libname=`echo $f | awk -F'.' '{print $1}'`
	if [ -L $f ] && [ "$libname" == "libSimFL" ]; then
		rm -f $f
	elif [ ! -L $f ] && [ -e $f ]; then
		FullVersion=`echo $f | awk -F'.' '{printf "%s.%s.%s", $3, $4, $5}'`
		MajorVersion=`echo $FullVersion | awk -F'.' '{print $1}'`
	fi
done

ln -s libSimFL.so.$FullVersion libSimFL.so.$MajorVersion
ln -s libSimFL.so.$MajorVersion libSimFL.so

cd $CURDIR


export SIMFL
export LD_LIBRARY_PATH=$SIMFL/lib:$LD_LIBRARY_PATH
echo "SIMFL           : $SIMFL"
echo "LD_LIBRARY_PATH : $LD_LIBRARY_PATH"

