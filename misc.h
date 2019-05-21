/*This file is part of flasher.

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

#ifndef MISC_H
#define MISC_H
#include <stdio.h>
#include "libmmc.h"

#define SLASH "/"

u_int32_t swap32 (u_int32_t toswap);
u_int16_t swap16 (u_int16_t toswap);
size_t find_pattern_buff (char *buff, char *pattern, size_t bsize,
			  size_t psize);
long asciihex2int (char *buff);
size_t falloc (char *filename, char **inbuff);
void printd (char *buff, int offset, int buff_start, int buff_end);
#endif
