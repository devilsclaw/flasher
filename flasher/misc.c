#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
//#include <unistd.h>
#include <string.h>
#include "misc.h"

void __sleep(int delay){
#ifdef WIN32
  Sleep(delay);
#else
  sleep(delay/1000);
#endif
}

//Changs LSB to MSB 32bit variable
long swap32(long toswap){
  long swapped = 0;
  swapped |= (toswap >> 24) & 0x000000FF;
  swapped |= (toswap >> 8)  & 0x0000FF00;
  swapped |= (toswap << 8)  & 0x00FF0000;
  swapped |= (toswap << 24) & 0xFF000000;
  return swapped;
}

//change LSB to MSB 16bit variable
short swap16(short toswap){
  short swapped = 0;
  swapped = ((toswap >> 8)  & 0x000000FF) | ((toswap << 8)  & 0x0000FF00);
  return swapped;
}

size_t find_pattern_buff(char* buff,char* pattern,size_t bsize,size_t psize){
  char found = 0;
  size_t bcount;
  size_t pcount;

  for(bcount = 0;bcount < bsize;bcount++){
    for(pcount = 0;pcount < psize;pcount++){
      if(pcount+bcount >= bsize){
        return 0;
      }

      if(pattern[pcount] == buff[bcount+pcount] || pattern[pcount] == (char)0xFF){
        found = 1;
      }else{
        found = 0;
        break;
      }
    }
    if(found){
      return bcount;
    }
  }
  return 0;
}

int asciihex2int(char* buff){
  size_t str_size = strlen(buff);
  int value = 0;
  int ret = 0;
  size_t count;
  size_t count2;

  if(!str_size || str_size > 8){
    return -1;
  }

  count2 = str_size-1;
  for(count = 0;count < str_size;count++){
    value = 0;
    switch(toupper(buff[count2]))
    {
      case '0':
        value = 0;
        break;
      case '1':
        value = 1;
        break;
      case '2':
        value = 2;
        break;
      case '3':
        value = 3;
        break;
      case '4':
        value = 4;
        break;
      case '5':
        value = 5;
        break;
      case '6':
        value = 6;
        break;
      case '7':
        value = 7;
        break;
      case '8':
        value = 8;
        break;
      case '9':
        value = 9;
        break;
      case 'A':
        value = 10;
        break;
      case 'B':
        value = 11;
        break;
      case 'C':
        value = 12;
        break;
      case 'D':
        value = 13;
        break;
      case 'E':
        value = 14;
        break;
      case 'F':
        value = 15;
        break;
      default:
        return -1;
    };
    ret |= value << count * 4;
    count2--;
  }
  //printf("ret = 0x%08X\n",ret);
  return ret;
}

//Get the size of the file
//Returns file size
size_t get_filesize(FILE* fileh){
  //TODO: Add error checking
  long org_pos;
  size_t fsize;

  org_pos = ftell(fileh);
  fseek(fileh,0,SEEK_END);
  fsize = ftell(fileh);
  rewind(fileh);
  fseek(fileh,org_pos,SEEK_SET);
  return fsize;
}

//Open file and allocate memory and store it
size_t falloc(char* filename,char** inbuff){
  FILE* fileh = fopen(filename,"rb");
  char* buff = 0;
  size_t fsize;

  *inbuff = buff;
  if(!fileh){
    //verbose("falloc: Could not open %s file\n",filename);
    fclose(fileh);
    return 0;
  }
  fsize = get_filesize(fileh);
  if(!fsize){
    //verbose("falloc: File size is 0\n");
    fclose(fileh);
    return 0;
  }

  buff = (char*)malloc(fsize);
  if(!buff){
    //verbose("falloc: Malloc failed\n");
    fclose(fileh);
    return 0;
  }

  if(fread(buff,1,fsize,fileh) != fsize){
    //verbose("falloc: Could not fully read %s\n",filename);
    free(buff);
    fclose(fileh);
    return 0;
  }

  *inbuff = buff;
  return fsize;
}

//Compare two memory locations reanges data
char cmp_buff(char* buff1,char* buff2,size_t size){
  int count;
  for(count = 0; count < size;count++){
    if(buff1[count] != buff2[count]){
      return 0;
    }
  }
  return 1;
}

//clear memory loctaion range with zeros
//void clr_buff(void* buf,size_t size){
//  memset((char*)buf,0,size);
//}

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
