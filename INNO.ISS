;#########################################################################
;# This file was created by the Inno Setup Wizard.  You can safely
;# modify the contents of this file. 
;#
;# The wizard thing was pretty helpful, 
;# it let me get started quickly. It wa easy to go back and read the real 
;# docs later on. Modified  by hand with vim, muquit, Jan-24-1999
;#
;#########################################################################
;  Inno Setup version 1.09
;  Copyright 99 1998 by Jordan Russell
;  jordanr@iname.com
;  http://www.digicron.com/jordanr/
;#########################################################################

[Setup]
AllowNoIcons=0
AlwaysRestart=0
AppCopyright=(c) Muhammad A Muquit (http://www.muquit.com/)
Uninstallable=yes
AlwaysCreateUninstallIcon=yes
AppName=WWW Count 2.5
AppVerName=WWW Count 2.5
BackColor=clBlue
BackSolid=0
UsePreviousAppDir=Yes
DefaultDirName=InetPub\Scripts
DefaultGroupName=WWW Count 2.5
LicenseFile=data\copyright
InfoBeforeFile=data\README.NT
InfoAfterFile=data\README.AFTER.NT

[Files]
Source: "data\README.txt"; DestDir: "{app}\wcount";
Source: "docs\CountNT.html"; DestDir:"{app}";
Source: "data\conf\count.cfg"; DestDir: "{app}\wcount\conf";
Source: "data\data\sample.dat"; DestDir: "{app}\wcount\data";
Source: "data\rgb.txt"; DestDir: "{app}\wcount";
Source: "data\digits\A\yeehaw.gif"; DestDir: "{app}\wcount\digits\A";
Source: "data\digits\A\peng.gif"; DestDir: "{app}\wcount\digits\A";
Source: "data\digits\A\strip.gif"; DestDir: "{app}\wcount\digits\A";
Source: "data\digits\B\strip.gif"; DestDir: "{app}\wcount\digits\B";
Source: "data\digits\C\strip.gif"; DestDir: "{app}\wcount\digits\C";
Source: "data\digits\D\strip.gif"; DestDir: "{app}\wcount\digits\D";
Source: "data\digits\D\lenna.gif"; DestDir: "{app}\wcount\digits\D";
Source: "data\digits\E\strip.gif"; DestDir: "{app}\wcount\digits\E";
Source: "data\digits\E\foo.gif"; DestDir: "{app}\wcount\digits\E";
Source: "data\digits\bang\strip.gif"; DestDir: "{app}\wcount\digits\bang";
Source: "data\digits\cd\strip.gif"; DestDir: "{app}\wcount\digits\cd";
Source: "data\digits\cdr\strip.gif"; DestDir: "{app}\wcount\digits\cdr";
Source: "data\digits\cdd\strip.gif"; DestDir: "{app}\wcount\digits\cdd";
Source: "src\Count.exe"; DestDir: "{app}";
Source: "utils\mkstrip\mkstrip.exe"; DestDir: "{app}";
Source: "utils\extdgts\extdgts.exe"; DestDir: "{app}";
[Dirs]
Name: "{app}\wcount\logs"

