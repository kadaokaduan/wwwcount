#!/bin/sh
#
# it's not for you
# muquit, nov-24-1997
#

if [ $# -lt 1 ]; then
    echo "
        sorry this program is not for you
    "
    exit 0
fi

VER=`cat VERSION`
DVER=`cat VERSION|sed 's/\./_/g'`

INSTALL_PROG="./install-sh"

HTML_DIR="$HOME/public_html"
#HTML_DIR="$HOME/foo"
#DOC_DIR="$HTML_DIR/Count2.5"
DOC_DIR="$HTML_DIR"

ARC_FILE="wwwcount$VER.tar.gz"
ARC_DIR="$DOC_DIR/src/wwwcount$VER"

main="Count.html"
ex="Count$DVER-ex.html"
faq="Count_FAQ.html"
changelog="Count_Changelog.html"
download="Count_download.html"
blues="blues.gif"

####nt="CountNT.html"

FDIR="Count$VER"


HOST=`hostname`
if [ .$HOST = .chaos ]; then
    ./mkarc.sh -gzip
    if [ -f $ARC_FILE ]; then
        prog="$INSTALL_PROG -c -m 644 $ARC_FILE $ARC_DIR/$ARC_FILE"
        echo "$prog"
        $prog
    else
        echo "File $ARC_FILE does not exist"
        exit 0
    fi
    prog="$INSTALL_PROG -c -m 644 docs/$main $DOC_DIR/$main"
    echo $prog
    $prog

    prog="$INSTALL_PROG -c -m 644 docs/$ex $DOC_DIR/$ex"
    echo $prog
    $prog

    prog="$INSTALL_PROG -c -m 644 docs/$faq $DOC_DIR/$faq"
    echo $prog
    $prog

    prog="$INSTALL_PROG -c -m 644 docs/$download $DOC_DIR/$download"
    echo $prog
    $prog

    prog="$INSTALL_PROG -c -m 644 docs/$changelog $DOC_DIR/$changelog"
    echo $prog
    $prog

    prog="$INSTALL_PROG -c -m 644 docs/$blues $DOC_DIR/$blues"
    echo $prog
    $prog

###    prog="$INSTALL_PROG -c -m 644 docs/$nt $DOC_DIR/$nt"
###    echo $prog
###    $prog

    prog="cp -r docs/$FDIR $DOC_DIR"
    echo $prog
    $prog
else
    echo "wrong host"
    exit 0
fi
