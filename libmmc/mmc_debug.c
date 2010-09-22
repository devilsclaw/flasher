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

#include <stdio.h>
#include <ctype.h>

#ifdef _DEBUG

//sets the width the printd will use
#define width 0x10
//prints data blocks passed to it in hex and ascii
void printd(char* buff,int offset,int buff_start,int buff_end){
  char var = 0;
  int count;
  int count2;
  int count3 = 0;
  if(!buff_end) return;

  for(count = 0;count < ((buff_end%width)?(buff_end/width)+1:buff_end/width);count++)
  {
    //print the offset possition
    printf("0x%08X:",offset+(count*width));
    for(count2 = count*width;count2 < ((count*width+width < buff_end)?(count*width)+width:buff_end);count2+=2){
      //print the hex data in clumps of two
      printf(" %02x%02x", (0x000000FF &(char)buff[buff_start+count2]),((buff_start+count2+1 < buff_end)?(0x000000FF &(char)buff[buff_start+count2+1]):0));
      count3++; //used to calculate the spaced needed befor the ascii
    }
    printf(" ");
    for(;count3 < width/2;count3++){
      //fill in the extra space needed to keep the ascii aligned
      printf("     ");
    }
    count3 = 0;
    //print the ascii data
    for(count2 = count*width;count2 < ((count*width+width < buff_end)?(count*width)+width:buff_end);count2++){
      var = buff[buff_start+count2];
      var = (isgraph(var & 0x000000FF) ? var : '.');
      printf("%c",(char)var);
    }

    printf("\n");
  }
}
#else

void printd(char* buff,int offset,int buff_start,int buff_end){}

#endif //_DEBUG