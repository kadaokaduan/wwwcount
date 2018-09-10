#!/bin/sh
#
# create a gzip compressed tar archive or a zip compressed archive of
# the www homepage access counter source distribution.
# This program is for my personal use only, so I'll use sh functions. All
# machines I use have real shell.
# nov-09-1997
# 

ver=`cat ./VERSION`
pwd=`pwd`
pwd_head=`basename $pwd`

#
# first check if we'r at the corrent directory
#
if [ $pwd_head != wwwcount$ver ]; then
    echo "You are in wrong base directory"
    echo "Expected: wwwcount$ver"
    echo "Found:    $pwd_head"
    echo "exiting ..."
    exit 1
fi

if [ $# -lt 1 ]; then
    echo
    echo "usage: `basename $0` [options]"
    echo "Where options are:"
    echo "  -gzip   create gzip compressed tar archive of the source"
    echo "  -zip    create zip archive of the source"
    echo "  -zipnt  create the zip archive of binaries for NT"
    echo 
    exit 1
fi

rm -f *.zip

#############################################################
# funciton  for gzip+tar
#############################################################
create_gzip_tar ()
{
    arc_hist=$1
    arc_filename="$pwd_head.tar"

#
# remove the tar file if any in the current working directoyr
#
    if [ -f ./$arc_filename.gz ]; then
        echo "removing $arc_filename.gz"
        rm -f ./$arc_filename.gz
    fi
    
    if [ -f ./wwwcount.zip ]; then
        echo "removing ./wwwcount.zip"
        rm -f ./wwwcount.zip
    fi

    if [ -f ./$arc_hist ]; then
        echo "removing $arc_hist"
        rm -f ./$arc_hist
    fi
    echo "creating fresh $arc_hist"
    date > /tmp/$arc_hist.$$
    gzip -V 2>> /tmp/$arc_hist.$$
    uname -a  >> /tmp/$arc_hist.$$

    mv /tmp/$arc_hist.$$ $pwd/$arc_hist

    echo "running make pristine"
    make pristine > /dev/null 2>&1
#
# go up
#

    cd ..
    tar -cf /tmp/$arc_filename.$$ ./$pwd_head
    if [ $? -eq 0 ]; then
        gzip -f -v -9 /tmp/$arc_filename.$$
        mv /tmp/$arc_filename.$$.gz ./$pwd_head/$arc_filename.gz
        cd $pwd
        ls -l $arc_filename.gz
    fi

}


#####################################################
# create the zip archive of the source distribution
#####################################################
create_zip ()
{
    arc_hist=$1
    arc_filename="wwwcount"

#
# remove the tar file if any in the current working directoyr
#

    zarc_filename="$arc_filename.zip"
    if [ -f ./$zarc_filename ]; then
        echo "removing $zarc_filename"
        rm -f ./$zarc_filename
    fi

    if [ -f ./$pwd_head.tar.gz ]; then
        echo "removing ./$pwd_head.tar.gz"
        rm -f ./$pwd_head.tar.gz
    fi


    if [ -f ./$arc_hist ]; then
        echo "removing $arc_hist"
        rm -f ./$arc_hist
    fi
    echo "creating fresh $arc_hist"
    date > /tmp/$arc_hist.$$
    uname -a  >> /tmp/$arc_hist.$$

    mv /tmp/$arc_hist.$$ $pwd/$arc_hist

    echo "running make pristine"
    make pristine > /dev/null 2>&1
#
# go up
#

    cd ..
    zip -q -r /tmp/$arc_filename ./$pwd_head
    mv /tmp/$zarc_filename $pwd
    cd $pwd
    ls -l $zarc_filename

}

#############################################################
# function to create zip archive for the binaries for NT
#############################################################
# 
# make a directory called wwwcount2_5
#
TDIR="./wwwcount2_5"

if [ -d $TDIR ]; then
cat<<EOD
    Will not overwrite the directory $TDIR. Remove it manually first
    if you like to create a new archive.
EOD
    exit 0
fi

create_bin_zip_nt()
{
CPROG="src/Count.exe"
MKSTRIP="utils/mkstrip/mkstrip.exe"
EXTDGTS="utils/extdgts/extdgts.exe"


    if [ ! -f $CPROG ]; then
        echo "Program $CPROG does not exist!"
        exit 0
    fi

    if [ ! -d  $TDIR ]; then
        mkdir $TDIR
    fi
cat<<EOR>$TDIR/README

Count 2.5

This is the README file for the binary distribution of the WWW Homepage
Access Counter for MS Windows NT.

The official Counter main page is at:
    http://www.fccc.edu/users/muquit/Count.html
Page for NT binary distribution is at:
    http://www.fccc.edu/users/muquit/CountNT.html

The official page contains the latest information.

INSTALL
-------
    * Read the documentation of the counter at the counter main page at the
      URL: http://www.fccc.edu/users/muquit/Count.html

    * edit wcount/count.cfg file. 

    * copy Count.exe to the cg-bin or the directory where your web server is
      configured to keep the CGI programs.

    * copy the directory wcount to the same cgi-bin directory where Count.exe
      is copied.

    * Read the counter documentation and look at the example page.
      
--
Muhammad A Muquit
Nov-16-1997, Count 2.4
Jan-10-1998, updated for Count 2.5


EOR

    cp $CPROG $TDIR
    cp $MKSTRIP $TDIR
    cp $EXTDGTS $TDIR

    cp VERSION $TDIR
    cp docs/CountNT.html $TDIR
    cp -r data $TDIR/wcount
}

##
# check command line and call the appropiate routine
##
if [ $1 = "-gzip" ]; then
    arc_hist="gzip.arc"
    create_gzip_tar $arc_hist
elif [ $1 = "-zip" ]; then
    arc_hist="zip.arc"
    create_zip $arc_hist
elif [ $1 = "-zipnt" ]; then
    create_bin_zip_nt
else
    echo "Unknown option \"$1\""
    exit 0
fi


