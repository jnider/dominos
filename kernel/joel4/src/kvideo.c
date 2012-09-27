////////////////////////////////////////////////////////////////////////////////
/// @file   kvideo.cpp
/// @short  text mode screen driver
/// @author J.Nider
/// @date   30/10/2009
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "kvideo.h"
#include "kstdio.h"

#define COLUMNS                 80        /* The number of columns. */
#define LINES                   24        /* The number of lines. */
#define ATTRIBUTE               7         /* The attribute of an character. */
#define VIDEO                   0xB8000   /* The video memory address. */

static int xpos;/* Save the X position. */
static int ypos;/* Save the Y position. */
static unsigned char *video;/* Point to the video memory. */

/* Clear the screen and initialize VIDEO, XPOS and YPOS. */
void k_cls(void)
{
  int i;

  video = (unsigned char *) VIDEO;

  for (i = 0; i < COLUMNS * LINES * 2; i++)
    *(video + i) = 0;

  xpos = 0;
  ypos = 0;
}

// Put the character C on the screen
void k_putchar(int c)
{
   unsigned int i;
   if (c == '\n' || c == '\r')
   {
      newline:
         xpos = 0;
         ypos++;
         if (ypos >= (LINES))
         {
            // scroll
            for (i=0; i<(LINES); i++)
            {
               k_memcpy(video+(i*COLUMNS*2), video+((i+1)*COLUMNS*2), COLUMNS*2);
            }
            ypos = LINES-1;
         }
         return;
   }

   *(video + (xpos + ypos * COLUMNS) * 2) = c & 0xFF;
   *(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;

   xpos++;
   if (xpos >= COLUMNS)
      goto newline;
}

