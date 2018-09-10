#!/bin/sh
#
# updates the doc tree with the latest examples
#
# it's not for your use
# muquit, nov-28-1997
#

VER=`cat ../VERSION`
DVER=`cat ../VERSION|sed 's/\./_/g'`

##PROG="./testcount-sh"
##if [ -f $PROG ]; then
##   echo "$PROG"
##    $PROG
##else
##   echo "NO $PROG exists, exiting"
##    exit 0
##fi

EXF="Count$DVER-ex.html"
DOCDIR="../docs"
CEXD="Count$VER/eximages"
EXD="$DOCDIR/Count$VER"
RM="/bin/rm -rf"
MV="/bin/cp -r"

if [ -d $CEXD ]; then
    if [ -d $EXD/eximages ]; then
        echo "$RM $EXD/eximages"
        $RM $EXD/eximages
        echo "$MV $CEXD $EXD"
        $MV $CEXD $EXD
    fi
else
    echo "directory $CEXD does not exist, exiting"
    exit 0
fi

if [ -f $EXF ]; then
    echo "$MV $EXF $DOCDIR"
    $MV $EXF $DOCDIR
else
    echo "No $EXF file exists"
    exit 0
fi

