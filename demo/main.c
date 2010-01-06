#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libmmc/libmmc.h>

void printd(char* buff,int offset,int buff_start,int buff_end);
int swap32(int toswap);
short swap16(short toswap);
size_t firm_dumper(int device,char** inbuff,int loc,int pos,size_t dsize);



int lgren_read(int device,char source,mmcdata_s* d,int pos,int size){
  char* buffer = 0;
  if(!(d->data) && size){
    buffer = (char*)malloc(size);
    if(!buffer){
      return 0;
    }
    d->data = buffer;
    d->datasize = size;
    memset(d->data,0,d->datasize);
  }

  //CDB12 FE 00 AA DD DD RR BB SI ZE 00 00 00
  d->cmdsize = 0xC; //CDB10
  d->cmd[0] = 0xFE;
  *((int*)(&d->cmd[2])) = swap32(pos); //LSB to MSB fill [2] through [5]
  d->cmd[6] = ((!source)?1:source);
  *((short*)(&d->cmd[7])) = swap16(size); //LSB to MSB fill [7] through [8]
  return drive_command(device,d,MMC_READ);
}

void vendor_mode(int device){
  mmcdata_s d;
  memset(&d,0,sizeof(mmcdata_s));

  //CDB12: FD 00 00 00 00 00 00 00 FF 00 00 00
  d.cmdsize = 0xC;
  d.cmd[0]  = 0xFD;
  d.cmd[8]  = 0xFF;
  drive_command(device,&d,MMC_READ);

  //CDB12: FD 01 4D 41 54 53 48 49 54 41 02 00
  d.cmd[0]  = 0xFD;
  d.cmd[1]  = 0x1;
  d.cmd[2]  = 'M';
  d.cmd[3]  = 'A';
  d.cmd[4]  = 'T';
  d.cmd[5]  = 'S';
  d.cmd[6]  = 'H';
  d.cmd[7]  = 'I';
  d.cmd[8]  = 'T';
  d.cmd[9]  = 'A';
  d.cmd[10] = 0x2;
  drive_command(device,&d,MMC_READ);

  //CDB12: FD 00 52 41 4D 34 37 46 4B 38 03 00
  d.cmd[0]  = 0xFD;
  d.cmd[1]  = 0x0;
  d.cmd[2]  = 'R';
  d.cmd[3]  = 'A';
  d.cmd[4]  = 'M';
  d.cmd[5]  = '4';
  d.cmd[6]  = '7';
  d.cmd[7]  = 'F';
  d.cmd[8]  = 'K';
  d.cmd[9]  = '8';
  d.cmd[10] = 0x3;
  drive_command(device,&d,MMC_READ);

}

int main(int argc,char** argv){
  int drive = 0;
  char* buff = 0;
  size_t fsize;
  FILE* fileh;
  int count;
  int count2;
  mmcdata_s d;
  int size = 0;
  memset(&d,0,sizeof(mmcdata_s));

  drive = drive_open(3);
  vendor_mode(drive);
  //lgren_read(drive,1,&d,0,0x10);
  //lgren_read(drive,1,&d,0x40000000,0x10);

  fsize = firm_dumper(drive,&buff,1,0x40000000,0x400000);
  if(!fsize){
    printf("cmd_dumploc: Failed to dump location\n");
    system("pause");
    return 0;
  }

  if(fsize < size){
    printf("cmd_dumploc: Size of dump was smaller then asked still going to dump what we can\n");
  }

  //debug("cmd_dumploc: Opening %s\n",tmpInput[0]);
  fileh = fopen("matdump.hex","wb+");
  if(!fileh){
    free(buff);
    printf("cmd_dumploc: Failed to open %s\n","matdump.hex");
    return 0;
  }

  //debug("cmd_dumploc: Writing %s\n",tmpInput[0]);
  if(fwrite(buff,1,fsize,fileh) != fsize){
    free(buff);
    printf("cmd_dumploc: Failed to write %s\n","matdump.hex");
    return 0;
  }

  free(buff);
  fflush(fileh);
  fclose(fileh);
  printf("cmd_dumploc: Dump process finished\n");
  return 0;
}

int swap32(int toswap){
  int swapped = 0;
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

size_t firm_dumper(int device,char** inbuff,int loc,int pos,size_t dsize){
  size_t start_pos,stop_pos,dump_size,dumpped_size,size;
  char* buff;
  mmcdata_s d;

  *inbuff = 0;

  if(!dsize){
    return 0;
  }

  memset(&d,0,sizeof(mmcdata_s));

  start_pos = pos;
  stop_pos  = dsize + pos;
  dump_size = dsize;
  dumpped_size = 0;

  buff = (char*)calloc(1,dsize);
  if(!buff){
    printf("firm_dumper: failed to allocate memory for buff\n");
    return 0;
  }

  //TODO: Update code so that is only does one copy instead of creating a buffer
  //then copying it to the other buffer.
  size = 0;
  while(start_pos < stop_pos)
  {
    //calculate the current block size
    size = ((0x1000 + start_pos < stop_pos)?0x1000:0x1000 - ((0x1000 + start_pos) - stop_pos));
    if(!lgren_read(device,loc,&d,start_pos,size)){
      printf("firm_dumper: Failed to read from drive. Will write what we got so far\n");
      break;
    }

    memcpy(&buff[dumpped_size],d.data,size);

    //update possition and percentage counter
    start_pos += size;
    dumpped_size += size;
    free(d.data); //clean up read_firmware memory
    d.data = 0;
    //printf("\b\b\b\b\b\b\b\b\b\b\b\b%.2f",(dumpped_size/(double)dump_size));
    //ud_progress((dumpped_size/(double)dump_size));
    fflush(stdout); //print to screen now
  }
  *inbuff = buff;
  return dumpped_size;
}
