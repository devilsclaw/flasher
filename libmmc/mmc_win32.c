#ifdef WIN32
#include "libmmc/libmmc.h"
#include <stdio.h>


//execute the edb mmc command for windows
int drive_command(int drive,mmcdata_s* d,int direction){
  int ret = 1;
  DWORD ret_bytes;
  char sptd_size;
  SCSI_PASS_THROUGH_DIRECT* sptd;
  int count;

   //windows is weird it decides to add the sense data to the
   //end of the common structure sent to device io so we calc
   //the size of the struct and then add on the size of the
   //sense date, then we allocate space for it
  sptd_size = sizeof(SCSI_PASS_THROUGH_DIRECT) + sizeof(d->sensedata);
  sptd      = (SCSI_PASS_THROUGH_DIRECT*)malloc(sptd_size);

  memset(sptd,0,sptd_size); //clean it up so we dont have to set everything
   //some values can stay 0

  sptd->DataBuffer = d->data;             //points to the buffer where date will be received
  sptd->DataTransferLength = d->datasize; //let windows know the size of the buffer
  sptd->DataIn = direction;               //direction as in reading or writing date to the device
  memcpy(sptd->Cdb,d->cmd,d->cmdsize);    //copy the mmc command into the struct
  sptd->CdbLength = d->cmdsize;           //tell windows the size of the command.. max is 16
  sptd->SenseInfoOffset = sizeof(SCSI_PASS_THROUGH_DIRECT); //sense offest is based off the start of the struct
   //not real memory offset

  sptd->SenseInfoLength = sizeof(d->sensedata);    //size of the sense date
  sptd->Length = sizeof(SCSI_PASS_THROUGH_DIRECT); //the size of the struct in general
  sptd->TimeOutValue = 1800; //how long to wait until we consider it a failed event in seconds

  //printf("CDB:\n");
  //printd(d->cmd,0,0,d->cmdsize);
   //send the data to the device
  if(!DeviceIoControl((HANDLE)drive,IOCTL_SCSI_PASS_THROUGH_DIRECT,sptd,sptd_size,sptd,sptd_size,&ret_bytes,NULL)){
    ret = 0; //device io passed
  }
  memcpy(d->sensedata,&((char*)sptd)[sizeof(SCSI_PASS_THROUGH_DIRECT)],sizeof(d->sensedata));

  d->senseret.sk   = (d->sensedata[2] & 0x0F);
  d->senseret.asc  =  d->sensedata[12];
  d->senseret.ascq =  d->sensedata[13];

  if(*((int*)&d->senseret)){
    ret = 0;
  }
  //printf("SK/ASC/ASCQ\n%02X/%02X/%02X\n",d->senseret.sk,d->senseret.asc,d->senseret.ascq);
  return ret;
}

//open a disk drive based off its drive letter
int drive_open(int device){
  char tdrive = 0;
  char drives[1024];
  char drive_name[256] = "\\\\.\\%c:";
  char drive_name2[256];
  size_t count,dcount;
  int    fd = 0;

  memset(drives,0,sizeof(drives));
  dcount = 0;
  GetLogicalDriveStrings(sizeof(drives),drives);
  for(count = 0;count < sizeof(drives);){
    if(!drives[count]){
      return 0;
    }
    tdrive = tolower(drives[count]);
    memset(drive_name2,0,sizeof(drive_name2));
    sprintf(drive_name2,drive_name,tdrive);
    if((fd = (int)CreateFile(drive_name2,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0)) == INVALID_HANDLE_VALUE){
          printf("drive_open: Cannot open device %c.\n",tdrive);
        }
        if(fd != INVALID_HANDLE_VALUE && drive_type(fd) == T_CDROM){
          ++dcount;
          if(dcount == device){
            break;
          }
        }

        if(fd!= INVALID_HANDLE_VALUE  && !drive_close(fd)){
          return 0;
        }

        count += strlen(&drives[count]) + 1;
  }
  return fd;
}

char drive_close(int device){
  if(CloseHandle((HANDLE)device)){
    return 1;
  }
  return 0;
}

drives_s* drives_available(){
  char tdrive;
  char drives[1024];
  char drive_name[256] = "\\\\.\\%c:";
  char drive_name2[256];
  size_t count,dcount;
  int    device;
  drive_s* d = 0;
  drives_s* dd = 0;

  memset(drives,0,sizeof(drives));
  device = 0;
  dcount = 0;
  GetLogicalDriveStrings(sizeof(drives),drives);
  for(count = 0;count < sizeof(drives);){
    if(!drives[count]){
      break;
    }
    memset(drive_name2,0,sizeof(drive_name2));
    tdrive = tolower(drives[count]);
    sprintf(drive_name2,drive_name,tdrive);
    if((device = (int)CreateFile(drive_name2,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0)) == INVALID_HANDLE_VALUE){
          printf("drives_available: Cannot open device %c.\n",tdrive);
        }
        if(device != INVALID_HANDLE_VALUE && drive_type(device) == T_CDROM){
          inquiry_s* inq;
          ++dcount;
          d = (drive_s*)realloc(d,dcount*sizeof(drive_s));
          inq = drive_inquiry(device);
          memcpy(&d[dcount-1].name,inq->p_id,sizeof(inq->p_id));
          d[dcount-1].id = dcount;
          free(inq);
        }
        if(device != INVALID_HANDLE_VALUE  && !drive_close(device)){
          return 0;
        }
        count += strlen(&drives[count]) + 1;
  }

  dd = (drives_s*)malloc(sizeof(drives_s));
  if(dd){
    dd->drives = d;
    dd->number = dcount;
  }else{
    free(d);
    return 0;
  }
  return dd;
}
#endif //WIN32
