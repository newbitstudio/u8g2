/*

  u8g2_ll_hvline.c
  
  low level hvline

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include "u8g2.h"
#include <assert.h>

/*=================================================*/
/*
  u8g2_ll_hvline_vertical_top_lsb
    SSD13xx
    UC1701    
*/


#ifdef U8G2_HVLINE_SPEED_OPTIMIZATION

/*
  x,y		Upper left position of the line within the local buffer (not the display!)
  len		length of the line in pixel, len must not be 0
  dir		0: horizontal line (left to right)
		1: vertical line (top to bottom)
  asumption: 
    all clipping done
*/
void u8g2_ll_hvline_vertical_top_lsb(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len, uint8_t dir)
{
  uint16_t offset;
  uint8_t *ptr;
  uint8_t bit_pos, mask;

  //assert(x >= u8g2->buf_x0);
  //assert(x < u8g2_GetU8x8(u8g2)->display_info->tile_width*8);
  //assert(y >= u8g2->buf_y0);
  //assert(y < u8g2_GetU8x8(u8g2)->display_info->tile_height*8);
  
  /* bytes are vertical, lsb on top (y=0), msb at bottom (y=7) */
  bit_pos = y;		/* overflow truncate is ok here... */
  bit_pos &= 7; 	/* ... because only the lowest 3 bits are needed */
  mask = 1;
  mask <<= bit_pos;

  offset = y;		/* y might be 8 or 16 bit, but we need 16 bit, so use a 16 bit variable */
  offset &= ~7;
  offset *= u8g2_GetU8x8(u8g2)->display_info->tile_width;
  ptr = u8g2->tile_buf_ptr;
  ptr += offset;
  ptr += x;
  
  if ( dir == 0 )
  {
    if ( u8g2->draw_color != 0 )
    {
      do
      {
	*ptr |= mask;
	ptr++;
	len--;
      } while( len != 0 );
    }
    else
    {
      mask = ~mask;
      do
      {
	*ptr &= mask;
	ptr++;
	len--;
      } while( len != 0 );
    }  
  }
  else
  {    
    do
    {
      if ( u8g2->draw_color != 0 )
      {
	*ptr |= mask;
      }
      else
      {
	*ptr &= ~mask;
      }
      
      bit_pos++;
      bit_pos &= 7;

      len--;

      if ( bit_pos == 0 )
      {
	ptr+=u8g2->width;
	
	/* another speed optimization, but requires about 60 bytes on AVR */
	/*
	while( len >= 8 )
	{
	  if ( u8g2->draw_color != 0 )
	  {
	    *ptr = 255;
	  }
	  else
	  {
	    *ptr = 0;
	  }
	  len -= 8;
	  ptr+=u8g2->width;
	}
	*/
	
	mask = 1;
      }
      else
      {
	mask <<= 1;
      }
    } while( len != 0 );
  }
}



#else /* U8G2_HVLINE_SPEED_OPTIMIZATION */

/*
  x,y position within the buffer
*/
static void u8g2_draw_pixel_vertical_top_lsb(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y)
{
  uint16_t offset;
  uint8_t *ptr;
  uint8_t bit_pos, mask;

  //assert(x >= u8g2->buf_x0);
  //assert(x < u8g2_GetU8x8(u8g2)->display_info->tile_width*8);
  //assert(y >= u8g2->buf_y0);
  //assert(y < u8g2_GetU8x8(u8g2)->display_info->tile_height*8);
  
  /* bytes are vertical, lsb on top (y=0), msb at bottom (y=7) */
  bit_pos = y;		/* overflow truncate is ok here... */
  bit_pos &= 7; 	/* ... because only the lowest 3 bits are needed */
  mask = 1;
  mask <<= bit_pos;

  offset = y;		/* y might be 8 or 16 bit, but we need 16 bit, so use a 16 bit variable */
  offset &= ~7;
  offset *= u8g2_GetU8x8(u8g2)->display_info->tile_width;
  ptr = u8g2->tile_buf_ptr;
  ptr += offset;
  ptr += x;
  
  
  if ( u8g2->draw_color != 0 )
  {
    *ptr |= mask;
  }
  else
  {
    mask ^= 255;
    *ptr &= mask;
  }  
}

/*
  x,y		Upper left position of the line within the local buffer (not the display!)
  len		length of the line in pixel, len must not be 0
  dir		0: horizontal line (left to right)
		1: vertical line (top to bottom)
  asumption: 
    all clipping done
*/
void u8g2_ll_hvline_vertical_top_lsb(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len, uint8_t dir)
{
  if ( dir == 0 )
  {
    do
    {
      u8g2_draw_pixel_vertical_top_lsb(u8g2, x, y);
      x++;
      len--;
    } while( len != 0 );
  }
  else
  {
    do
    {
      u8g2_draw_pixel_vertical_top_lsb(u8g2, x, y);
      y++;
      len--;
    } while( len != 0 );
  }
}


#endif /* U8G2_HVLINE_SPEED_OPTIMIZATION */

/*=================================================*/
/*
  u8g2_ll_hvline_horizontal_right_lsb
    ST7920
*/

/*
  x,y position within the buffer
*/
static void u8g2_draw_pixel_horizontal_right_lsb(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y)
{
  uint16_t offset;
  uint8_t *ptr;
  uint8_t bit_pos, mask;

  //assert(x >= u8g2->buf_x0);
  //assert(x < u8g2_GetU8x8(u8g2)->display_info->tile_width*8);
  //assert(y >= u8g2->buf_y0);
  //assert(y < u8g2_GetU8x8(u8g2)->display_info->tile_height*8);
  
  /* bytes are vertical, lsb on top (y=0), msb at bottom (y=7) */
  bit_pos = x;		/* overflow truncate is ok here... */
  bit_pos &= 7; 	/* ... because only the lowest 3 bits are needed */
  mask = 128;
  mask >>= bit_pos;
  x >>= 3;

  offset = y;		/* y might be 8 or 16 bit, but we need 16 bit, so use a 16 bit variable */
  offset *= u8g2_GetU8x8(u8g2)->display_info->tile_width;
  offset += x;
  ptr = u8g2->tile_buf_ptr;
  ptr += offset;
  
  
  if ( u8g2->draw_color != 0 )
  {
    *ptr |= mask;
  }
  else
  {
    mask ^= 255;
    *ptr &= mask;
  }  
}

/*
  x,y		Upper left position of the line within the local buffer (not the display!)
  len		length of the line in pixel, len must not be 0
  dir		0: horizontal line (left to right)
		1: vertical line (top to bottom)
  asumption: 
    all clipping done
*/
void u8g2_ll_hvline_horizontal_right_lsb(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t len, uint8_t dir)
{
  if ( dir == 0 )
  {
    do
    {
      u8g2_draw_pixel_horizontal_right_lsb(u8g2, x, y);
      x++;
      len--;
    } while( len != 0 );
  }
  else
  {
    do
    {
      u8g2_draw_pixel_horizontal_right_lsb(u8g2, x, y);
      y++;
      len--;
    } while( len != 0 );
  }
}

