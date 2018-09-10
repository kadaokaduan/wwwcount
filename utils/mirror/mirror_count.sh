#!/bin/sh
###
### mirror the WWW Homepage Access counter pages.
### The official Counter homepage is at URL:
###     http://www.fccc.edu/users/muquit/Count.html
###     http://muquit.com/muquit/software/Count/Count.html
### muquit, Feb-28-1999, sunday, first cut
### muquit@muquit.com, Feb-13-2000
#      Updated to mirror from muquit.com
###
### The script can be used from command line or as a cron job. To setup a
### cron job to check the coutner pages at 12 midnight US EST, put the
### following line in the crontab file.
###
### 0 0 * * * /usr/local/etc/Counter/utils/mirror_count.sh
###
### To edit crontab file, just type: crontab -e
### you'll be in your editor (usually vi by default). Add the line at the
### end of the file. save and quit. :wq
### To check the cron entry, type; crontab -l
###

##----------------- editable variables---------starts-------------------

# full path of wget program
wget="/usr/local/bin/wget"

# the counter URL
counter_url="http://muquit.com/muquit/software/Count/Count.html"


# The directory where the counter pages will be downloaded. 
# it must be a web accessible directory
dir="/usr/local/apache/htdocs/Count"

# the path of the log file to keep the wget log information.
log="/usr/local/apache/htdocs/Count_mirror.log"

##----------------- editable variables---------ends-------------------

if [ ! -x $wget ]; then
    echo "The program $wget does not exist, exiting."
    exit 1
fi

$wget -nH -r -np -N  -R index.html -L -X cgi-bin \
    -P $dir -o $log \
    --cut-dirs=3    \
    $counter_url > /dev/null 2>&1

###
### make sure the directory is readable by the web server. edit and 
### uncomment the following lines if needed. 
###
# /bin/chown -R nobody $dir
# /bin/chgrp -R nobody $dir
# /bin/chmod -R 755 $dir


#############################################################################
###    Explanation of the wget options used: 
###
###       -nH             don't create host directories
###       -r              recursive web-suck
###       -np             don't ascend to the parent directory
###       -N              don't retrieve files if older than local
###       -R index.html   don't bring index.html
###       -L              follow relative links only
###       -X cg-bin       exclude cgi-bin directoyr
###       -P $dir         save files to this directory 
###       -o $log         wget log will be written to this file
###       --cut-dirs=2    ignore 2 remote directory components (/users/muquit)
###       $counter_url    The URL to mirror
#############################################################################
