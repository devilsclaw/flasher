/*
  This file is part of flasher.

  flasher is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  flasher is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with flasher.  If not, see <http://www.gnu.org/licenses/>.
  */

#if defined(__linux__)
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include "misc.h"

/* Change LSB to MSB 32bit variable */
u_int32_t
swap32 (u_int32_t toswap)
{
  u_int32_t swapped = 0;
  swapped |= (toswap >> 24) & 0x000000FF;
  swapped |= (toswap >> 8) & 0x0000FF00;
  swapped |= (toswap << 8) & 0x00FF0000;
  swapped |= (toswap << 24) & 0xFF000000;
  return swapped;
}

/* change LSB to MSB 16bit variable */
u_int16_t
swap16 (u_int16_t toswap)
{
  u_int16_t swapped = 0;
  swapped = ((toswap >> 8) & 0x00FF) | ((toswap << 8) & 0xFF00);
  return swapped;
}

size_t
find_pattern_buff (char *buff, char *pattern, size_t bsize, size_t psize)
{
  char found = 0;
  size_t bcount;
  size_t pcount;

  for (bcount = 0; bcount < bsize; bcount++)
    {
      for (pcount = 0; pcount < psize; pcount++)
	{
	  if (pcount + bcount >= bsize)
	    return 0;

	  if (pattern[pcount] == buff[bcount + pcount]
	      || pattern[pcount] == (char) 0xFF)
	    found = 1;
	  else
	    {
	      found = 0;
	      break;
	    }
	}
      if (found)
	return bcount;
    }
  return 0;
}

long
asciihex2int (char *buff)
{
  size_t str_size = strlen (buff);

  if (!str_size || str_size > 8)
      return -1;

  return strtol(buff, NULL, 16);
}

/* Open file and allocate memory and store it */
size_t
falloc (char *filename, char **inbuff)
{
  FILE *fileh = fopen (filename, "rb");
  char *buff = 0;
  size_t fsize;
  struct stat buf;

  *inbuff = buff;
  if (!fileh)
    {
      printf ("falloc: Could not open %s file\n", filename);
      fclose (fileh);
      return 0;
    }

  if (fstat(fileno(fileh), &buf))
    {
      printf ("falloc: File size is 0\n");
      fclose (fileh);
      return 0;
    }

  fsize = buf.st_size;

  buff = (char *) malloc (fsize);
  if (!buff)
    {
      printf ("falloc: Malloc failed\n");
      fclose (fileh);
      return 0;
    }

  if (fread (buff, 1, fsize, fileh) != fsize)
    {
      printf ("falloc: Could not fully read %s\n", filename);
      free (buff);
      fclose (fileh);
      return 0;
    }

  *inbuff = buff;
  return fsize;
}
