#!/bin/sh
##
## make a binary distribution
## muquit, May-01-1999

HOMEPAGE="http://www.fccc.edu/users/muquit/Count.html"

COUNT_BIN="./bin/Count.cgi"
EXTDGTS_BIN="./bin/extdgts"
MKSTRIP_BIN="./bin/mkstrip"

CP="/bin/cp"
RM="/bin/rm"
MKDIR="/bin/mkdir"
LS="/bin/ls"

OS=`uname`
REL=`uname -r`


DIR="./wwwcnt25bin-$OS"
README="$DIR/README"

DATA="./data"
DOCS="./docs"
CFG="./count.cfg"
VERSION="./VERSION"

INST_PROG="./Count-install"
TMPL="./Config.tmpl"

TOBINDIR="$DIR/bin"



if [ ! -f $COUNT_BIN ]; then
    echo "Binary $COUNT_BIN does not exist .. aborting"
    exit 
fi

if [ ! -f $EXTDGTS_BIN ]; then
    echo "Binary $EXTDGTS_BIN does not exist .. aborting"
    exit 
fi

if [ ! -f $MKSTRIP_BIN ]; then
    echo "Binary $MKSTRIP_BIN does not exist .. aborting"
    exit 
fi

if [ ! -f $TMPL ]; then
    echo "File $TMPL does not exist .. aborting"
    exit 
fi
. $TMPL


if [ -d $DIR ]; then
    echo "removing directory $DIR"
    $RM -rf $DIR    
    $RM -rf $DIR.tar.gz
fi
echo "
============================================================================
making binary distribution for $OS $REL
============================================================================
"
echo "making directory $DIR"
echo "$MKDIR $DIR"
$MKDIR $DIR

echo "$MKDIR $TOBINDIR"
$MKDIR $TOBINDIR

echo "$CP $VERSION $DIR"
$CP $VERSION $DIR

echo "$CP $COUNT_BIN $TOBINDIR"
$CP $COUNT_BIN $TOBINDIR

echo "$CP $EXTDGTS_BIN $TOBINDIR"
$CP $EXTDGTS_BIN $TOBINDIR

echo "$CP $MKSTRIP_BIN $TOBINDIR"
$CP $MKSTRIP_BIN $TOBINDIR

echo "$CP $INST_PROG $DIR"
$CP $INST_PROG $DIR

echo "$CP $TMPL $DIR"
$CP $TMPL $DIR

echo "$CP $CFG $DIR"
$CP $CFG $DIR

echo "$CP -r $DATA $DIR"
$CP -r $DATA $DIR

echo "$RM -f $DIR/README*"
$RM -f $DIR/README*

echo "$CP -r $DOCS $DIR"
$CP -r $DOCS $DIR

echo ""
echo "creating README"
cat<<EOR>$README

This is a binary release compiled on $OS $REL
The program is compiled with the following directory structure:

      $BASE_DIR          <- Base Directory
EOR

cat<<'EOX'>>$README
          |-- Log                     <- Log Directory
          |   `-- CountX_X.log        <- Log file
          |-- conf                    <- Conf Directory
          |   `-- count.cfg           <- Configuration file
          |-- data                    <- Data Directory
          |   |-- sample.dat          <- data files
          |   |-- joe.dat
          |   |-- harry.dat
          |   `--
          |
          |-- digits                  <- Digit Directory
          |   |-- A
          |   |   `-- strip.gif       
          |   |-- B
          |   |   `-- strip.gif
          |   |-- C                   Digit sub-directories and
          |   |   `-- strip.gif       image strip of different styles
          |   |-- D
          |   |   |-- lenna.gif
          |   |   `-- strip.gif
          |   |-- E
          |   |   `-- strip.gif
          |   |-- cd
          |   |   `-- strip.gif
          |   |-- cdd
          |   |   `-- strip.gif
          |   `-- cdr
          |       `-- strip.gif
          `-- rgb.txt                 <- color name database

EOX

cat<<EOX2>>$README
For more information please read the Counter documentation at the URL:
  $HOMEPAGE


Enjoy!

EOX2

echo "tar -cf $DIR.tar $DIR"
tar -cf $DIR.tar $DIR

echo "gzip -v -9 $DIR.tar"
gzip -v -9 $DIR.tar

$LS -l $DIR.tar.gz
