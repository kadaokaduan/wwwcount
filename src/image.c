/* routines call imaging routines in combine library for the countner */
#include "cdebug.h"
#include "combine.h"
#include "gdfonts.h"
#include "count.h"

#ifdef SYS_WIN32
#include "configNT.h"
#elif defined( __VMS )
#include "configVMS.h"
#else
#include "config.h"
#endif

#ifdef __VMS
#define GIF_PATH_FMT "%s%s]%s" /* Already have ] at end of _DIR macros */
#else
#define GIF_PATH_FMT "%s/%s/%s"
#endif

static Image *makeCountdownImage _Declare ((Image *,DigitInfo *));

static char *ImagePrefix[]=
{
    "zero", "one", "two", "three", "four", "five", 
    "six", "seven", "eight", "nine",
};

/*
**  StringImage()
**  creates an image of a string and writes it with MIME header to
**  the browser
**
**  Parameters:
**  char    *str    the string
**
**  Return Values:
**      none
**
**  Limitations and Comments:
**      It writes the MIME header before the image as specified in 
**      CGI 1.1 spec. Before writing the header it also sends the headers
**      to hint the browser to not cache the image.
**
**  Development History:
**      who                  when           why
 *      muquit@semcor.com   09-Sep-95   first cut
*/


void StringImage(str)
char
    *str;
{
    Image
        *fimage,
        *image;

    int
        str_length;

    FrameInfo
        frame_info;
    SFontInfo
        font_info;

    /*
    ** if you do not want to show any error message to the browser,
    ** just return and the visitor will see a broken image
    */

    if (getcfgShowErrmsg() == False)
        return;

    str_length=(int) strlen(str);
    image=CreateBaseImage(gdFontSmall->w*str_length+10,
        gdFontSmall->h+10,0,0,0,DirectClass);

    if (image == (Image *) NULL)
    {
        Warning("Unable to create base image for string");
        return;
    }
    
    font_info.do_bg=True;
    font_info.bgr=0;
    font_info.bgg=0;
    font_info.bgb=0;

    font_info.fgr=0;
    font_info.fgg=255;
    font_info.fgb=0;

    ImageString(image,gdFontSmall,5,5,str,&font_info);
    GetFrameInfo(image->columns,image->rows,&frame_info);
    fimage=FrameImage(image,&frame_info);
    if (fimage != (Image *) NULL)
    {
        DestroyAnyImageStruct(&image);
        image=fimage;
    }

    setCache(0);
    setExpire(0);
    PrintHeader();
    WriteGIFImage(image,(char *) NULL);
    
}
/*
** mogrify image if needed
*/
void MogrifyImage(image,digit_info,frame_info)
Image
    **image;
DigitInfo
    *digit_info;
FrameInfo
    *frame_info;
{
    Image
        *framed_image,
        *rotated_image;

    if (*image != (Image *) NULL)
    {
        if (digit_info->negate == True)
            NegateImage(*image);
            
        if (digit_info->Frame == True) 
        {
            RGB 
               color,
               matte_color;
            
            matte_color.red=frame_info->matte_color.red;
            matte_color.green=frame_info->matte_color.green;
            matte_color.blue=frame_info->matte_color.blue;

            frame_info->height=frame_info->width;
            frame_info->outer_bevel=(frame_info->width >> 2)+1;
            frame_info->inner_bevel=frame_info->outer_bevel;
            frame_info->x=frame_info->width;
            frame_info->y=frame_info->height;
            frame_info->width=(*image)->columns+(frame_info->width << 1);
            frame_info->height=(*image)->rows+(frame_info->height << 1);
            frame_info->matte_color=matte_color; 
        
            XModulate(&color,matte_color.red,matte_color.green,
                matte_color.blue, HighlightModulate);
            frame_info->highlight_color.red=color.red;
            frame_info->highlight_color.green=color.green;
            frame_info->highlight_color.blue=color.blue;

            XModulate(&color,matte_color.red,matte_color.green,
                matte_color.blue, ShadowModulate);
            frame_info->shadow_color.red=color.red;
            frame_info->shadow_color.green=color.green;
            frame_info->shadow_color.green=color.green;

            framed_image=FrameImage(*image,frame_info);
            if (framed_image != (Image *) NULL)
            {
                DestroyAnyImageStruct (image);
                framed_image->class=DirectClass;
                *image=framed_image;
            }
        }    
        commentImage(*image,IMAGE_COMMENT);
        
#if 0
        (*image)->comments = (char *) malloc (1024*sizeof(char));
        if ((*image)->comments != (char *) NULL)
        {
            /*
            (void) sprintf((*image)->comments,"\n%s %s \n%s %s\n%s %s\n%s\n%s",
                "Count.cgi", Version, "By", Author, "URL:", Url,
                "Built:",GetTime());
            */
            (void) strcpy((*image)->comments,IMAGE_COMMENT);
        }
        
#endif /* if 0 */

        if (digit_info->replace_color == True)
        {
            OpaqueImage(*image,
                digit_info->opaque_red,
                digit_info->opaque_green,
                digit_info->opaque_blue,
                digit_info->pen_red,
                digit_info->pen_green,
                digit_info->pen_blue);

        }

        if (digit_info->alpha == True)
        {
             AlphaImage(*image,digit_info->alpha_red,digit_info->alpha_green,
                digit_info->alpha_blue);
        }
         if (digit_info->rotate == True)
         {
            rotated_image=RotateImage(*image,digit_info->rotate_degrees);
            if (rotated_image != (Image *) NULL)
            {
                DestroyAnyImageStruct (image);
                *image=rotated_image;
            }
            else
            {
                Warning("Could not rotate");
                StringImage("Could not rotate");
                exit(0);
            }
         }
    }
}

Image *CombineImages(digitbuf,digit_info)
char
    *digitbuf;
DigitInfo
    *digit_info;
{
    register char
        *p;

    char
        *token,
        tmpbuf[MaxTextLength];

    int
        i,
        n,
        max_idx=0,
        nsegment,
        segment,
        rc=0;

    unsigned int
        bwidth,
        bheight,
        gbW,
        gbH,
        base_width,
        base_height;

    Image
        *strip_image,
        *sub_image,
        *base_image;


    int
        *seg_array=(int *) NULL;

    RectangleInfo
        rinfo;


    segment=0;
    bwidth=0;
    bheight=0;
    gbW=0;
    gbH=0;
    base_width=0;
    base_height=0;
    strip_image=(Image *) NULL;


    /*
    ** if comma is requesed, commaize (my new invented word!) the
    ** digit. also it's meaningness to left pad while commizing,
    ** therefore, don't left pad with zeros as well
    */
    if ((digit_info->display_type == (unsigned int) SHOW_COUNTER) &&
        (digit_info->comma == True))
    {
        Commaize(digitbuf);
        Debug2("after commaze=%s",digitbuf,0);
    }
    /*
    ** if image strip is not used, we'll handle it as usual
    */
if (digit_info->use_strip == False)
{
    for (p=digitbuf; *p != '\0'; p++)
    {
        *tmpbuf='\0';
        if(isdigit(*p))
        {
            (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                g_sZ_ibase,digit_info->ddhead,ImagePrefix[(int)*p-'0']);
        }
        else
        {
            if (*p == ':')
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"colon");
            if ((*p == 'A') || (*p == 'a'))
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"am");
            if ((*p == 'P') || (*p == 'p'))
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"pm");
            if (*p == ',')
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"comma");
            
            if (*p == '-')
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"dash");

            if (*p == 'D')
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"d");

            if (*p == 'H')
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"h");

            if (*p == 'M')
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"m");

            if (*p == 'S')
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"s");
        }
        rc=GetGIFsize(tmpbuf,&gbW,&gbH);
        if (rc)
        {
            StringImage("Unable to determine digit image size!");
            exit(1);
        }
        bwidth += gbW;
        if (gbH > bheight)
            bheight=gbH;
    }
}
else
{
    /*
    ** strip is in use. First of all, we will determine the with of the
    ** base image. The segments in the strip may be of varaible widths,
    ** we'll handle it
    */
    (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                          g_sZ_ibase,digit_info->ddhead,"strip");
    strip_image=ReadImage(tmpbuf);
    if (strip_image == (Image *) NULL)
    {
        StringImage("Unable to read strip image!");
        exit(1);
    }
    if (strip_image->comments != (char *) NULL)
    {
        rc=sscanf(strip_image->comments,"%d",&nsegment);
        if (rc != 1)
        {
            StringImage("No string segment info found in GIF comment ext.!");
            exit(1);
        }

        max_idx=nsegment+2;
        Debug2("max_idx=%d",max_idx,0);
        seg_array=(int *) malloc((max_idx)*sizeof(int));
        if (seg_array == (int *) NULL)
        {
            StringImage("Memory Allocation Failed for seg_array!");
            exit(1);   
        }
        p=strip_image->comments;
        i=0;
        while ((token=mystrtok(p,":")) != (char *) NULL)
        {
            p=(char *) NULL;
            if (token != (char *) NULL)
            {
                seg_array[i]=atoi(token);
            }
            i++;
        }
        /*
        ** the info about strip in the comment extension of the GIF image
        ** does not match. we'll abort
        ** btw, if the strip is created with my mkstrip program, the info
        ** should be correct. possibly someone is using a strip made by
        ** some other program. Tough luck!
        */
        if (i != max_idx)
        {
            StringImage("Strip info mismatch in GIF comment ext.!");
            exit(1);
        }

        for (p=digitbuf; *p != '\0'; p++)
        {
            if (isdigit(*p))
            {
                n=(*p-'0');
                bwidth += (seg_array[n+2]-seg_array[n+1]);
            }
            else
            {
                if (*p == ':')
                {
                    assertSegarray(12,max_idx,*p);
                    rinfo.x=seg_array[11];
                    bwidth += (seg_array[12]-seg_array[11]);
                }

                if ((*p == 'A') || (*p == 'a'))
                {
                    assertSegarray(13,max_idx,*p);
                    rinfo.x=seg_array[12];
                    bwidth += (seg_array[13]-seg_array[12]);
                }
                if ((*p == 'P') || (*p == 'p'))
                {
                    assertSegarray(14,max_idx,*p);
                    rinfo.x=seg_array[13];
                    bwidth += (seg_array[14]-seg_array[13]);
                }
                if (*p == ',')
                {
                    assertSegarray(15,max_idx,*p);
                    rinfo.x=seg_array[14];
                    bwidth += (seg_array[15]-seg_array[14]);
                }
                if (*p == '-')
                {
                    assertSegarray(16,max_idx,*p);
                    rinfo.x=seg_array[15];
                    bwidth += (seg_array[16]-seg_array[15]);
                }
                if (*p == 'D')
                {
                    assertSegarray(17,max_idx,*p);
                    rinfo.x=seg_array[16];
                    bwidth += (seg_array[17]-seg_array[16]);
                }
                if (*p == 'H')
                {
                    assertSegarray(18,max_idx,*p);
                    rinfo.x=seg_array[17];
                    bwidth += (seg_array[18]-seg_array[17]);
                }
                if (*p == 'M')
                {
                    assertSegarray(19,max_idx,*p);
                    rinfo.x=seg_array[18];
                    bwidth += (seg_array[19]-seg_array[18]);
                }
                if (*p == 'S')
                {
                    assertSegarray(20,max_idx,*p);
                    rinfo.x=seg_array[19];
                    bwidth += (seg_array[20]-seg_array[19]);
                }
            }
        }
        bheight=strip_image->rows;
    }
    else
    {
        StringImage("No info about strip found in GIF comment ext.!");
        exit(1);
    }
}
    base_image= CreateBaseImage (bwidth,bheight,0,0,0,DirectClass);

    if (base_image == (Image *) NULL)
    {
        StringImage("Unable to create base image!");
        exit(1);
    }

if (digit_info->use_strip == False)
{
    for (p=digitbuf; *p != '\0'; p++)
    {
        *tmpbuf='\0';
        if(isdigit(*p))
            (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                g_sZ_ibase,digit_info->ddhead,ImagePrefix[(int)*p-'0']);
        else
        {
            if (*p == ':')
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"colon");
            if ((*p == 'A') || (*p == 'a'))
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"am");
            if ((*p == 'P') || (*p == 'p'))
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"pm");
            if (*p == ',')
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"comma");
            if (*p == '-')
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"dash");
            if (*p == 'D')
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"d");

            if (*p == 'H')
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"h");

            if (*p == 'M')
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"m");

            if (*p == 'S')
                (void) sprintf(tmpbuf,GIF_PATH_FMT ".gif",
                    g_sZ_ibase,digit_info->ddhead,"s");
        }

        sub_image=ReadImage(tmpbuf);
        if (sub_image != (Image *) NULL)
        {
            FlattenImage(base_image,sub_image,ReplaceCompositeOp,
                base_width,0);
            base_width += sub_image->columns;
            DestroyAnyImageStruct(&sub_image);
        }
        else
        {
            StringImage(" FAILED to combine digit images!");
            exit(1);
        }
    }
}
else
{
   rinfo.y=0;
   rinfo.width=15;
   rinfo.height=strip_image->rows;
    for (p=digitbuf; *p != '\0'; p++)
    {
        *tmpbuf='\0';
        if (isdigit(*p))
        {
            n=(*p-'0');
            rinfo.x=seg_array[n+1];
            rinfo.width=seg_array[n+2]-seg_array[n+1];
        }
        else
        {
            if (*p == ':')
            {
                assertSegarray(12,max_idx,*p);
                rinfo.x=seg_array[11];
                rinfo.width=seg_array[12]-seg_array[11];
            }
            if ((*p == 'A') || (*p == 'a'))
            {
                assertSegarray(13,max_idx,*p);
                rinfo.x=seg_array[12];
                rinfo.width=seg_array[13]-seg_array[12];
            }
            if ((*p == 'P') || (*p == 'p'))
            {
                assertSegarray(14,max_idx,*p);
                rinfo.x=seg_array[13];
                rinfo.width=seg_array[14]-seg_array[13];
            }
            if (*p == ',')
            {
                assertSegarray(15,max_idx,*p);
                rinfo.x=seg_array[14];
                rinfo.width=seg_array[15]-seg_array[14];
            }
            if (*p == '-')    
            {
                assertSegarray(16,max_idx,*p);
                rinfo.x=seg_array[15];
                rinfo.width=seg_array[16]-seg_array[15];
            }    
            if (*p == 'D')
            {
                assertSegarray(17,max_idx,*p);
                rinfo.x=seg_array[16];
                rinfo.width = (seg_array[17]-seg_array[16]);
            }
            if (*p == 'H')
            {
                assertSegarray(18,max_idx,*p);
                rinfo.x=seg_array[17];
                rinfo.width = (seg_array[18]-seg_array[17]);
            }
            if (*p == 'M')
            {
                assertSegarray(19,max_idx,*p);
                rinfo.x=seg_array[18];
                rinfo.width = (seg_array[19]-seg_array[18]);
            }
            if (*p == 'S')
            {
                assertSegarray(20,max_idx,*p);
                rinfo.x=seg_array[19];
                rinfo.width = (seg_array[20]-seg_array[19]);
            }
        }

        sub_image=CropImage(strip_image,&rinfo);
        if (sub_image != (Image *) NULL)
        {
            FlattenImage(base_image,sub_image,ReplaceCompositeOp, base_width,0);
            base_width += sub_image->columns;
            DestroyAnyImageStruct(&sub_image); 
        }
        else
        {
            StringImage("Failed to extract image from strip!");
            exit(1);
        }    
    } 
}
    return(base_image);
}


/*
**  updateCordinate()
** update the co-ordinate based on the specified alignment
**
**  Parameters:
**  DigitInfo   *digit_info
**  Image       *base_image
**  Image       *digit_image
**
**  Return Values:
**  upl_x and upl_y members of DigitInfo structure is modidfied.
**
**  Limitations and Comments:
**  0,0 is uppler left corener x is positive toward right and y is positive
**  downward. 
** 
**  For ALIGN_TOP_LEFT, offset is from left and top edge of the base image.
**  For ALIGN_TOP_CENTER, offset is from top edge of the base image
**  For ALIGN_MID_LEFT, offset is from left edge of the base image.
**  For ALIGN_MID_CENTER, offset is not significant
**  For ALIGN_MID_RIGHT, offset is from right edge of the base image.
**  For ALIGN_BOTTOM_LEFT, offset is from left and bottom edge of the base image
**  For ALIGN_BOTTOM_CENTER, offset is from bottom edge of the base image
**  For ALIGN_BOTTOM_RIGHT, offset is from bottom and right edge of the base
**  image
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   May-05-1999    first cut
*/

static void updateCordinate(digit_info,base_image,digit_image)
DigitInfo
    *digit_info;
Image
    *base_image,
    *digit_image;
{
    int
        offset;

    offset=digit_info->offset;

    switch(digit_info->alignment)
    {
        case ALIGN_LEFT:
        case ALIGN_TOP_LEFT:
        {
            digit_info->upl_x = 0 + offset;
            digit_info->upl_y = 0 + offset;
            break;
        }

        case ALIGN_TOP_CENTER:
        {
           digit_info->upl_x = (base_image->columns - digit_image->columns) / 2;
           digit_info->upl_y = 0 + offset;
           break;
        }

        case ALIGN_MID_LEFT:
        {
           digit_info->upl_x=0 + offset;
           digit_info->upl_y = (base_image->rows - digit_image->rows) / 2;
           break;
        }

        case ALIGN_CENTER:
        case ALIGN_MID_CENTER:
        {
           digit_info->upl_x = (base_image->columns - digit_image->columns) / 2;
           digit_info->upl_y = (base_image->rows - digit_image->rows) / 2;
           break;
        }

        case ALIGN_MID_RIGHT:
        {
           digit_info->upl_x = base_image->columns - digit_image->columns
                               - offset;
           digit_info->upl_y = (base_image->rows - digit_image->rows) / 2;
           break;
        }

        case ALIGN_RIGHT:
        case ALIGN_TOP_RIGHT:
        {
            digit_info->upl_x = base_image->columns - digit_image->columns
                                - offset;
            digit_info->upl_y = 0 + offset;
            break;
        }

        case ALIGN_BOTTOM_LEFT:
        {
           digit_info->upl_x = 0 + offset;
           digit_info->upl_y = base_image->rows - digit_image->rows - offset;
           break;
        }

        case ALIGN_BOTTOM_CENTER:
        {
           digit_info->upl_x = (base_image->columns - digit_image->columns) / 2;
           digit_info->upl_y = base_image->rows - digit_image->rows - offset;
           break;
        }

        case ALIGN_BOTTOM_RIGHT:
        {
           digit_info->upl_x = base_image->columns - digit_image->columns 
                               - offset;
           digit_info->upl_y = base_image->rows - digit_image->rows
                                - offset;
           break;
        }
    }
}

void makeCounterImage(digit_info,frame_info)
DigitInfo
    *digit_info;
FrameInfo
    *frame_info;
{
   Image
        *gif_image=(Image *) NULL,
        *digit_image=(Image *) NULL,
        *countdown_image=(Image *) NULL;

    int
        op;

    op=ReplaceCompositeOp;
    /* if GIF image, show it and get over with it. don't continue */
    if (digit_info->display_type == SHOW_GIF_FILE)
    {
        Debug2("gif file=%s",digit_info->gif_file,0);
        gif_image=ReadImage(digit_info->gif_file);
        if (gif_image == (Image *) NULL)
        {
            StringImage("Could not open the GIF file for reading");
            exit(0);
        }
        MogrifyImage(&gif_image,digit_info,frame_info);

        PrintHeader();
        (void) WriteGIFImage (gif_image, (char *)NULL);
        exit(0);
        /* won't be here */
    }


    /* combine digit images, may be countdown, counter or clock */
    digit_image=CombineImages(digit_info->pdigits,digit_info);
    if (digit_image == (Image *) NULL)
    {
        StringImage("Failed! Check DigitDir in config.h or dd in QUERY_STRING");
        exit(0);
    }

    /* read gif image file if needed, we may be compositing */
    if (*digit_info->gif_file != '\0')
    {
        Debug2("Displaying GIF file=\"%s\"",digit_info->gif_file,0);

        /* load the GIF file */
        gif_image=ReadImage(digit_info->gif_file);
        if (gif_image == (Image *) NULL)
        {
            StringImage("Could not open the GIF file for reading");
            exit(0);
        }
    }

    /* handle countdown */
    if (digit_info->display_type == SHOW_COUNTDOWN &&
            digit_info->show_from_date == 1)
    {
        countdown_image=makeCountdownImage(digit_image,digit_info);
    }

    /* if gif_image is not NULL, we're definitely compositing */
    if (gif_image != (Image *) NULL)
    {

        /*
        ** recompute x,y if alignment is specified but xy is
        ** not specified, xy has higher precedence (sp?)
        ** if the x,y is negative, alignment was specified.
        */
        if (digit_info->upl_x < 0 && digit_info->upl_y < 0)
            updateCordinate(digit_info,gif_image,digit_image);
           
        /*
        ** as we're combining with gif image, we'll handle transparency
        ** little differently. if trgb is specified we'll assume that
        ** the trgb is specified for the digit image and we'll make that
        ** color transparent.
        */
        if (digit_info->alpha == True)
        {
            if (countdown_image)
            {
                countdown_image->alpha_color=(RGB *) malloc(sizeof(RGB));
                if (countdown_image->alpha_color)
                {
                    countdown_image->alpha_color->red=digit_info->alpha_red;
                    countdown_image->alpha_color->green=digit_info->alpha_green;
                    countdown_image->alpha_color->blue=digit_info->alpha_blue;
                    op=TransparentOp;
                }

            }
            else
            {
                digit_image->alpha_color=(RGB *) malloc(sizeof(RGB));
                if (digit_image->alpha_color)
                {
                    digit_image->alpha_color->red=digit_info->alpha_red;
                    digit_image->alpha_color->green=digit_info->alpha_green;
                    digit_image->alpha_color->blue=digit_info->alpha_blue;
                    op=TransparentOp;
                }
            }
        }
 

        if (countdown_image != (Image *) NULL)
        {
            FlattenImage(gif_image,countdown_image,op, digit_info->upl_x, 
                digit_info->upl_y );
        }
        else
        {
           FlattenImage(gif_image,digit_image,op,digit_info->upl_x, 
                digit_info->upl_y );
        }

        digit_info->alpha=False;
        MogrifyImage(&gif_image,digit_info,frame_info);

        PrintHeader();
        WriteGIFImage(gif_image,(char *) NULL);
        exit(0);
        /* won't be here */
    }
    if (countdown_image != (Image *) NULL)
    {
        MogrifyImage(&countdown_image,digit_info,frame_info);

        PrintHeader();
        WriteGIFImage(countdown_image,(char *) NULL);
        exit(0);
        /* won't be here */
    }

    MogrifyImage(&digit_image,digit_info,frame_info);
    PrintHeader();
    WriteGIFImage(digit_image,(char *) NULL);
    exit(0);
    /* won't be here */
}


/*
** something went wrong..write the built in GIF image to stdout
*/

void SendErrorImage (bits, length)
unsigned char
    *bits;
int
    length;
{
    register unsigned char
        *p;

    register int
        i;

    p = bits;
    for (i=0; i < length; i++)
    {
        (void) fputc((char) *p, stdout);
        (void) fflush (stdout);
        p++;
    }
}

/* display digital time */
void displayTime(di,fi)
DigitInfo
    *di;
FrameInfo
    *fi;
{
    time_t
        daytime,
        tm;

    struct tm
        *ptm=(struct tm *) NULL;

    long
        diff;   /* in seconds */

    char
        ampm[2],
        hour[10],
        min[10],
        digit_buf[1024];

    int
        hr_dig1,
        hr_dig2,
        min_dig1,
        min_dig2;

        tm=time(NULL);
        if (*di->time_z != '\0')
        {
            /* no more TZ, 03-26-1996 */
            diff=checkTimezone(di->time_z);
            ptm=gmtime(&tm);
/*          Allow for UTC not being available - could make this specific to */
/*          VMS < 7.0, but ANSI standard does allow gmtime to return NULL   */
/*          and the following line will not be executed unless it is NULL   */
            if ( ! ptm ) ptm = localtime( &tm );
#ifdef HAVE_MKTIME
            daytime=mktime(ptm);
#else
            daytime=netMktime(ptm);
#endif
            daytime +=diff;
            ptm=localtime(&daytime);  
            Debug2("TIME GMT OFFSET=%s",asctime(ptm),0);
        }
        else
        {
            Debug2("No timezone specified",0,0);
            ptm=localtime(&tm);
        }

        if (di->time_format == 12)
        {
            if (ptm->tm_hour < 12)
                (void) strcpy(ampm,"A");
            else
                (void) strcpy(ampm,"P");

            if (ptm->tm_hour > 12)
                ptm->tm_hour=ptm->tm_hour-12;

            if (ptm->tm_hour == 0)
                ptm->tm_hour=12;
        }

        /* taking off strftime, 01-13-95 */
        if (di->leftpad == True)
        {
            hr_dig1=(ptm->tm_hour - (ptm->tm_hour % 10)) / 10;
            hr_dig2=ptm->tm_hour % 10;
            (void) sprintf(hour,"%d%d",hr_dig1,hr_dig2);
        }
        else
            (void) sprintf(hour,"%d",ptm->tm_hour);

        /* keep leading zeros in minutes */
        min_dig1=(ptm->tm_min - (ptm->tm_min % 10)) / 10;
        min_dig2=ptm->tm_min % 10;

        (void) sprintf(min,"%d%d",min_dig1,min_dig2);

        if (di->time_format == 12)
            (void) sprintf(digit_buf,"%s:%s%s",hour,min,ampm);
        else
        {
            (void) sprintf(digit_buf,"%s:%s",hour,min);
        }

    di->pdigits=mystrdup(digit_buf);
    makeCounterImage(di,fi);

    /* we won't be here */
}


/* display date */
void displayDate(di,fi)
DigitInfo
    *di;
FrameInfo
    *fi;
{
    time_t
        daytime,
        tm;

    struct tm
        *ptm=(struct tm *) NULL;

    long
        diff;   /* in seconds */

    char
        buf[1024];

    int
        mon_dig1,
        mon_dig2,
        day_dig1,
        day_dig2,
        year_dig1,
        year_dig2;

    int
        year_dig=2000,
        show_full_year=0;

        tm=time(NULL);
        if (*di->time_z != '\0')
        {
            diff=checkTimezone(di->time_z);
            ptm=gmtime(&tm);
#ifdef HAVE_MKTIME
            daytime=mktime(ptm);
#else
            daytime=netMktime(ptm);
#endif
            daytime +=diff;
            ptm=localtime(&daytime);  
        }
        else
        {
            Debug2("No timezone specified",0,0);
            ptm=localtime(&tm);
        }
        ptm->tm_mon++;
        /*
        ** padding, saw in xdaliclock
        */
        mon_dig1=(ptm->tm_mon-(ptm->tm_mon % 10)) / 10;
        mon_dig2=ptm->tm_mon % 10;
        
        day_dig1=(ptm->tm_mday-(ptm->tm_mday % 10)) / 10;
        day_dig2=ptm->tm_mday % 10;


        /*
        ** show full year digits if asked
        */
        if (di->show_full_year == True)
        {
            year_dig=1900+ptm->tm_year;
            show_full_year=1;
        }

        /*
        ** year 2000+ hack. asked by Cris Johnson (johnson@isn.dac.neu.edu)
        */
        if (show_full_year == 0)
        {
            if (ptm->tm_year >= 100)    
            {
                year_dig= 1900+ptm->tm_year;
                show_full_year=1;
            }
        }

         year_dig1=(ptm->tm_year-(ptm->tm_year % 10)) / 10;
         year_dig2=ptm->tm_year % 10;

         switch (di->date_format)
         {

                case DATE_MMDDYY:
                default:
                {
                    if (!show_full_year)
                    (void) sprintf(buf,"%d%d-%d%d-%d%d",
                mon_dig1,mon_dig2,day_dig1,day_dig2,year_dig1,year_dig2);
                    else
                    (void) sprintf(buf,"%d%d-%d%d-%d",
                mon_dig1,mon_dig2,day_dig1,day_dig2,year_dig);
                    break;
                }

                case DATE_DDMMYY:
                {
                    if (!show_full_year)
                    (void) sprintf(buf,"%d%d-%d%d-%d%d",
                day_dig1,day_dig2,mon_dig1,mon_dig2,year_dig1,year_dig2);
                    else
                    (void) sprintf(buf,"%d%d-%d%d-%d",
                day_dig1,day_dig2,mon_dig1,mon_dig2,year_dig);
                    break;
                }

                case DATE_YYMMDD:
                {
                    if (!show_full_year)
                    (void) sprintf(buf,"%d%d-%d%d-%d%d",
                year_dig1,year_dig2,mon_dig1,mon_dig2,day_dig1,day_dig2);
                    else
                    (void) sprintf(buf,"%d-%d%d-%d%d",
                year_dig,mon_dig1,mon_dig2,day_dig1,day_dig2);

                    break;
                }

                case DATE_YYDDMM:
                {
                    if (!show_full_year)
                    (void) sprintf(buf,"%d%d-%d%d-%d%d",
                year_dig1,year_dig2,day_dig1,day_dig2,mon_dig1,mon_dig2);
                    else
                    (void) sprintf(buf,"%d-%d%d-%d%d",
                year_dig,day_dig1,day_dig2,mon_dig1,mon_dig2);
                    break;
                }

                case DATE_MMYYDD:
                {
                    if (!show_full_year)
                    (void) sprintf(buf,"%d%d-%d%d-%d%d",
                mon_dig1,mon_dig2,year_dig1,year_dig2,day_dig1,day_dig2);
                    else
                    (void) sprintf(buf,"%d%d-%d-%d%d",
                mon_dig1,mon_dig2,year_dig,day_dig1,day_dig2);
                    break;
                }

                case DATE_DDYYMM:
                {
                    if (!show_full_year)
                    (void) sprintf(buf,"%d%d-%d%d-%d%d",
                day_dig1,day_dig2,year_dig1,year_dig2,mon_dig1,mon_dig2);
                    else
                    (void) sprintf(buf,"%d%d-%d-%d%d",
                day_dig1,day_dig2,year_dig,mon_dig1,mon_dig2);
                    break;
                }
         }  /* switch */
    
    di->pdigits=mystrdup(buf);
    makeCounterImage(di,fi);

    /* we won't be here */
}

/*
** if the index is greater than max index, exit
*/
void assertSegarray(idx,max_idx,c)
int
    idx;
int
    max_idx;
int
    c;
{
    /*
    ** used to be id > max_idx, so if the digit string has only digits
    ** (0-9), and say if : is specified, idx will be 12 but max idx will
    ** be 12 has well so even if the strip didn't have : it was passing.
    ** Thanks to:  Andy Machan <hostmaster@unitedbank.net> and
    ** "Daisuke Aoyama" <jack@st.rim.or.jp>, found the bug almost at the
    ** same time :)
    ** Feb-08-1999, 6:06 PM
    */
    if (idx > (max_idx - 1))
    {
        char
            buf[128];

        (void) sprintf(buf,"%s %c",
               "Image strip does not have the asked image segment:", c);
        StringImage(buf);
        exit(0);
    }
}


/*
** display the countdown
** dec-13-1998
*/
void displayCountdown(digit_info,frame_info)
DigitInfo
    *digit_info;
FrameInfo
    *frame_info;
{
    char
        *pbuf;

    struct tm
        target_tm,
        from_tm,
        *ptm_now=(struct tm *) NULL;

    time_t
        t1,
        t2,
        tdiff,
        now;

    int
        cdown_days,
        cdown_hours,
        cdown_minutes,
        cdown_seconds;

    long
        diff;

    target_tm.tm_year=digit_info->t_year - 1900;
    target_tm.tm_mon=digit_info->t_mon - 1;
    target_tm.tm_mday=digit_info->t_day;
    target_tm.tm_hour=digit_info->t_hr;
    target_tm.tm_min=digit_info->t_min;
    target_tm.tm_sec=digit_info->t_sec;
    target_tm.tm_isdst=(-1);



    /* if from values specified, calculate time_t for them -- mm,jul17,99 */
    if (digit_info->f_year >  0 &&
        digit_info->f_mon  >  0 &&
        digit_info->f_day  >  0 &&
        digit_info->f_hr   >= 0 &&
        digit_info->f_min  >= 0 &&
        digit_info->f_sec  >= 0)
    {
        from_tm.tm_year=digit_info->f_year - 1900;
        from_tm.tm_mon=digit_info->f_mon - 1;
        from_tm.tm_mday=digit_info->f_day;
        from_tm.tm_hour=digit_info->f_hr;
        from_tm.tm_min=digit_info->f_min;
        from_tm.tm_sec=digit_info->f_sec;
        from_tm.tm_isdst=(-1);

        now=mktime(&from_tm);
    }
    else
    {
        /* local time */
        now=time(NULL);
    }

    if (*digit_info->time_z != '\0')
    {
        /* offset from GMT in seconds */
        diff=checkTimezone(digit_info->time_z);
        now += diff;

        ptm_now=gmtime(&now);
        /*
        ** Allow for UTC not being available - could make this specific to 
        ** VMS < 7.0, but ANSI standard does allow gmtime to return NULL   
        ** and the following line will not be executed unless it is NULL  
        */
        if (! ptm_now) 
            ptm_now=localtime(&now);

#ifdef HAVE_MKTIME
        t1=mktime(ptm_now);
#else
        t1=netMktime(ptm_now);
#endif
    }
    else
        t1=now;

#ifdef HAVE_MKTIME
    t2=mktime(&target_tm);
#else
    t2=netMktime(&target_tm);
#endif
 
    tdiff=t2 - t1;
    cdown_days=tdiff/(60*60*24);
    cdown_hours=(tdiff % (60*60*24))/3600;
    cdown_minutes=(tdiff % 3600)/60;
    cdown_seconds=(tdiff % 60);

    pbuf=cMalloc(1024);
    if (digit_info->show_dhms == 1)
    {
        (void) sprintf(pbuf,"%dD:%dH:%dM:%dS",
                   cdown_days,
                   cdown_hours,
                   cdown_minutes,
                   cdown_seconds);

    }
    else
    {
        /* just show the days */
        (void) sprintf(pbuf,"%d",cdown_days);
    }
    digit_info->pdigits=mystrdup(pbuf);
    makeCounterImage(digit_info,frame_info);

    /* won't be here */
}

/*
**  makeCountdownImage()
**  if show from date is used. make a countdown image. 
**
**  Parameters:
**  digit_image     
**  digit_info
**
**  Return Values:
**  a pointer to countdown image on success
**  NULL on failure
**
**  Limitations and Comments:
**  - does not print error messages on failure. the caller should check
**    NULL return for failure.
**  - the caller is responsible to free the memory
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   May-01-1999    first cut
*/


static Image *makeCountdownImage(digit_image,digit_info)
Image
    *digit_image;
DigitInfo
    *digit_info;
{
    SFontInfo
        font_info;

    register Runlength
        *p;

    char
        *tstr;

    unsigned int
        string_width,
        base_width;

    int
        x,
        y;

    Image
        *base_image=(Image *) NULL;

    /*
    ** get the color of the bottom left pixel, we assume
    ** background is of that color. it will not work if the bg
    ** is multi-color
    */
    p=digit_image->pixels;

    font_info.bgr=p->red;
    font_info.bgg=p->green;
    font_info.bgb=p->blue;

    font_info.do_bg=True;

    font_info.fgr=digit_info->pen_red;
    font_info.fgg=digit_info->pen_green;
    font_info.fgb=digit_info->pen_blue;

    tstr=getLocalTime(digit_info,1);

    string_width=strlen(tstr)*(gdFontSmall->w);
    if (digit_image->columns < string_width)
        base_width=string_width+10;
    else
        base_width=digit_image->columns;
    x=(base_width - strlen(tstr)*gdFontSmall->w)/2;
    y=digit_image->rows+2;
    base_image=CreateBaseImage(base_width,
            digit_image->rows+gdFontSmall->h+3,
            p->red,p->green,p->blue,
            DirectClass);
    if (base_image == (Image *) NULL)
        return ((Image *) NULL);

    ImageString(base_image,gdFontSmall,x,y,tstr,&font_info);
    x=(base_width - digit_image->columns)/2;
    FlattenImage(base_image,digit_image,ReplaceCompositeOp,x,0);

    return(base_image);
}


/*
**  multilineStringImage()
**
**  Parameters:
**  char * the string
**
**  Return Values:
**
**
**  Limitations and Comments:
**
**
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   May-02-1999    first cut
*/


void multilineStringImage(s,align,bgr,bgg,bgb,fgr,fgg,fgb)
char
    *s;
int
    align;
int
    bgr,
    bgg,
    bgb;
int
    fgr,
    fgg,
    fgb;
{
    char
        *l,
        **lines,
        *p;

    int
        offset=0,
        i,
        x,y,
        tbw,
        base_width,
        base_height,
        line_count=1;

    FrameInfo
        frame_info;

    SFontInfo
        font_info;

    Image
        *image,
        *base_image;

    /* find out how many lines are there */
    for (p=s; *p != '\0'; p++)
    {
        if (*p == '\n')
            line_count++;
    }
    p=mystrdup(s);

    /* extract the lines */
    lines=(char **) malloc(line_count*sizeof(char *));
    i=0;
    while ((l=mystrtok(p,"\n")) != (char *) NULL)
    {
        lines[i++]=mystrdup(l);
        p=(char *) NULL;
    }

    font_info.bgr=bgr;
    font_info.bgg=bgg;
    font_info.bgb=bgb;

    font_info.fgr=fgr;
    font_info.fgg=fgg;
    font_info.fgb=fgb;

    font_info.do_bg=True;

    base_width=0;
    for (i=0; i < line_count; i++)
    {
        tbw=strlen(lines[i])*(gdFontSmall->w);
        if (tbw > base_width)
            base_width=tbw;
    }
    base_width=base_width+offset;
    base_height=(gdFontSmall->h*line_count)+offset;

    base_image=CreateBaseImage(base_width,
            base_height,
            bgr,bgg,bgb,
            DirectClass);

    x=offset/2;
    y=offset/2;
    for (i=0; i < line_count; i++)
    {
        switch (align)
        {
            case ALIGN_LEFT:
            default:
            {
                x=offset/2;
                break;
            }

            case ALIGN_CENTER:
            {
                x=((base_width-strlen(lines[i])*(gdFontSmall->w))/2)+offset/2;
                break;
            }

            case ALIGN_RIGHT:
            {
                x=(base_width-strlen(lines[i])*(gdFontSmall->w))+offset/2;
                break;
            }
        }
        y=(i*gdFontSmall->h)+offset/2;
        ImageString(base_image,gdFontSmall,x,y,lines[i],&font_info);
    }

    GetFrameInfo(base_image->columns,base_image->rows,&frame_info);
    image=FrameImage(base_image,&frame_info);

    /* memory leak - for future reference*/
    setCache(0);
    setExpire(0);
    PrintHeader();
    WriteGIFImage(image,(char *) NULL);

}
