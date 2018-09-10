#ifndef COUNT_CONFIG_H
#define COUNT_CONFIG_H 1

/*
** This file is automatically generated by the Configure script 
** Count-config
** Counter Version: 2.4 by ma_muquit@fccc.edu
** created on: Sat Nov 22 00:24:26 EST 1997
**
**  If you edit this file, you better make sure you know what are 
**  you doing. Whatever you do, never put absolute path here, because
**  the relative paths are converted to absolute path in the program.
*/
#define CONFIG_DIR  "WWW_ROOT:[WCOUNT.CONF]"
#define CONFIG_FILE "count.cfg"
#define DATA_DIR    "WWW_ROOT:[WCOUNT.DATA]"
#define DIGIT_DIR   "WWW_ROOT:[WCOUNT.DIGITS."
#define LOG_DIR     "WWW_ROOT:[WCOUNT.LOG]"
#define LOG_FILE    "Count24.log"

/*****************************************************************************
* Defaults parameters not supplied in calling.
* Change em here if you insist...
*****************************************************************************/
/*
** Max digitis in output 
*/
#define DEFAULT_MAXDIGITS           6

/*
** left padding with zeros, 0 indicates no left padding
*/
#define DEFAULT_LEFTPAD             True

/*
** Default frame thickness, 0 indicates no frame
*/
#define DEFAULT_FRAME_THICKNESS     4

/*
** default frame color is DarkGreen 0x006400, 0;100;10 is the RGB triplets
*/
#define DEFAULT_FRAME_RED           0
#define DEFAULT_FRAME_GREEN         100
#define DEFAULT_FRAME_BLUE          0

/*
** use this ft if we get frgb only
*/
#define FRGB_IMPLIED_FT             4

/*
** default transpareny
*/
#define DEFAULT_TRANSPARENCY        False

/*
** default transparent color is black
*/
#define DEFAULT_TRANSPARENT_RED     0
#define DEFAULT_TRANSPARENT_GREEN   0
#define DEFAULT_TRANSPARENT_BLUE    0

/*
** default opaque color. Do not change this.
*/
#define DEFAULT_OPAQUE_RED          0
#define DEFAULT_OPAQUE_GREEN        255
#define DEFAULT_OPAQUE_BLUE         0

/*
** default pen color
*/
#define DEFAULT_PEN_RED             255
#define DEFAULT_PEN_GREEN           119
#define DEFAULT_PEN_BLUE            0

/*
** use this tr, if we get trgb only
*/
#define TRGB_IMPLIED_TR             1

/*
** default initial counter value
*/
#define DEFAULT_STARTCOUNT          1

/*
** show count or not
*/
#define DEFAULT_SHOWCOUNT           1

/*
** this df means, use rand 
*/
#define DF_FOR_RANDOM               "RANDOM"

/*
** make rand the default too
*/
#define DEFAULT_DATAFILE            "RANDOM"

/*
** default digit style, my green LED, digits/A
*/
#define DEFAULT_DIGITS_STYLE        "A"

/*
** & or | separate params
*/
#define PARAM_DELIMITERS            "|&"

/*
** This specifies a file listing RGB values mapped to names.
** If not defined, no attempt to use RGB names is made.  If you are not sure,   
** just leave this be--it is okay if the file does not exist.
**
** When RGB_MAPPING_ISERROR is set to 1, an error is returned when RGB name
** value is used, but it shows the corresponding RGB triplet.  This is done
** so RGB names can be looked up, but yet not allow users to make constant
** use of this very ineffiecient lookup.
*/
#define RGB_MAPPING_DICT            "WWW_ROOT:[WCOUNT]rgb.txt"
/*
** End of parameter defaults
*/
#endif /* COUNT_CONFIG_H*/
