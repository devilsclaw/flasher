#ifdef LINUX
#include "libmmc/libmmc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>


//execute the cdb mmc command for linux
int drive_command(int drive,mmcdata_s* d,int direction){
  int ret = 1,count;
  struct cdrom_generic_command cgc;

  memset(&cgc, 0,sizeof(struct cdrom_generic_command));
  memcpy(cgc.cmd,d->cmd,d->cmdsize);
  cgc.buffer = (unsigned char*)d->data;
  cgc.buflen = d->datasize;
  cgc.data_direction = direction;
  cgc.timeout = 18000;
  cgc.sense = (struct request_sense *)d->sensedata;

  if(ioctl(drive,CDROM_SEND_PACKET,&cgc) < 0){
    ret = 0;
  }

  d->senseret.sk   = (d->sensedata[2] & 0x0F);
  d->senseret.asc  =  d->sensedata[12];
  d->senseret.ascq =  d->sensedata[13];

  if(*((int*)&d->senseret)){
    ret = 0;
  }
  return ret;
}

//open disk drive based off the dev path ex. /dev/hda
int drive_open(int device){
  int fd;
  //printf("open_drive: Opening Drive: %s.\n",device);
  if((fd = open((char*)device,O_RDONLY | O_NONBLOCK)) < 0){
    printf("open_drive: Cannot open device %s.\n",(char*)device);
    return 0;
  }

  //printf("open_drive: Device %s Opened.\n",device);

  if(drive_type(fd) != T_CDROM){
    printf("open_drive: %s is not a CD/DVD Drive type.\n",(char*)device);
    return 0;
  }

  return(fd);
}

char drive_close(int device){
  if(!fclose((FILE*)device)){
    return 1;
  }
  return 0;
}

drives_s* drives_available(){
  return 0;
}
#endif //LINUX
