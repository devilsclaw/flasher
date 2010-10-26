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
#include <libmmc/libmmc.h>

#ifdef WIN32
#define SLASH "\\"
#else
#define SLASH "/"
#endif

void __sleep(int delay);
long swap32(long toswap);
short swap16(short toswap);
size_t find_pattern_buff(char* buff,char* pattern,size_t bsize,size_t psize);
int asciihex2int(char* buff);
size_t get_filesize(FILE* fileh);
size_t falloc(char* filename,char** inbuff);
char cmp_buff(char* buff1,char* buff2,size_t size);
void printd(char* buff,int offset,int buff_start,int buff_end);
#endif
