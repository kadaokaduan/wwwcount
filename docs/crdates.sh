#!/bin/sh
#
# create the date images used in the Count.html
# ma_muquit@fccc.edu
# Oct-19-1997
#
. ../tests/env.dat
P="../src/Count.cgi"
PA="$P -debug"

if [ ! -f $P ]; then
    echo
    echo "$P does not exist"
    echo
fi

ATTR="ft=2&dd=D&srgb=00ff00&prgb=00ffff"

#
# func to set QUERY_STRING
setQueryString ()
{
    QUERY_STRING="lit=$1&$ATTR"
    export QUERY_STRING
}


crGifs ()
{
    setQueryString "$1"
    $PA
}

if [ $# -lt 2 ]; then
    echo
    echo "usage: `basename $0` <date-string> <output gif file>"
    echo
    exit 0
fi

crGifs $1 > $2  2>/dev/null
