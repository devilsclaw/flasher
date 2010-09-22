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

#ifdef __APPLE__
#include "libmmc/libmmc.h"
#include <stdio.h>

int drive_command(int drive,mmcdata_s* d,int direction){
  drive_data_s* dd = (drive_data_s*)drive;
  SCSITaskInterface**       tsk  = NULL;
  IOVirtualRange* rng = NULL;
  SCSITaskStatus  tsk_status;
  UInt64          trans_count = 0;
  int ret = 1;

  if(!(*dd->dev_intf)->ObtainExclusiveAccess(dd->dev_intf)){ //IF 1
    tsk = (*dd->dev_intf)->CreateSCSITask(dd->dev_intf);
    if(tsk != NULL){ //IF 2
      rng = (IOVirtualRange*)malloc(sizeof(IOVirtualRange));
      if(rng){ //IF 3
        rng->address = (int)d->data;
        rng->length  = d->datasize;
        if(!(*tsk)->SetCommandDescriptorBlock(tsk,(UInt8*)d->cmd,d->cmdsize) &&
              !(*tsk)->SetScatterGatherEntries(tsk,rng,1,d->datasize,direction) &&
              !(*tsk)->SetTimeoutDuration(tsk,10000)){ //IF 4
          if((*tsk)->ExecuteTaskSync(tsk,(SCSI_Sense_Data*)d->sensedata,&tsk_status,&trans_count)){ //IF 5
            ret = 0;
          } //IF 5
              } else ret = 0; //IF 4
              free(rng);
      }else ret = 0; //IF 3
      (*tsk)->Release(tsk);
    }else ret = 0; //IF 2
    if((*dd->dev_intf)->ReleaseExclusiveAccess(dd->dev_intf)){
      ret = 0;
    }
  }else ret = 0; //IF 1

  d->senseret.sk   = (d->sensedata[2] & 0x0F);
  d->senseret.asc  =  d->sensedata[12];
  d->senseret.ascq =  d->sensedata[13];

  if(*((int*)&d->senseret)){
    ret = 0;
  }
  return ret;
}

int drive_open(int device){
  CFMutableDictionaryRef main_dict = NULL;
  CFMutableDictionaryRef sec_dict  = NULL;
  io_name_t   cls_name;
  io_service_t  sd = IO_OBJECT_NULL;
  SInt32        score = 0;
  drive_data_s* dd = 0;
  int count;
  int ret = 1;

  dd = (drive_data_s*)calloc(1, sizeof(drive_data_s));
  if(!dd) return 0;
  dd->itt = IO_OBJECT_NULL;

  if((main_dict = CFDictionaryCreateMutable(kCFAllocatorDefault,0,&kCFTypeDictionaryKeyCallBacks,&kCFTypeDictionaryValueCallBacks)) != NULL){
    if((sec_dict = CFDictionaryCreateMutable(kCFAllocatorDefault,0,&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks)) != NULL){
      CFDictionarySetValue(sec_dict,CFSTR(kIOPropertySCSITaskDeviceCategory),CFSTR(kIOPropertySCSITaskAuthoringDevice));
      CFDictionarySetValue(main_dict,CFSTR(kIOPropertyMatchKey),sec_dict);
      CFRelease(sec_dict);
    }
  }

  if(IOServiceGetMatchingServices(kIOMasterPortDefault,main_dict,&dd->itt)){
    printf("no matching services\n");
    return 0;
  }

  for(count = 0;count < device;++count){
    sd = IOIteratorNext(dd->itt);
  }

  if(!sd){
    printf("could not find drive based off id\n");
    return 0;
  }

  if(!IOObjectGetClass(sd,cls_name) &&
      !IOCreatePlugInInterfaceForService(sd,kIOMMCDeviceUserClientTypeID,kIOCFPlugInInterfaceID,&dd->plg_in_intf,&score) &&
      !(*dd->plg_in_intf)->QueryInterface(dd->plg_in_intf,CFUUIDGetUUIDBytes(kIOMMCDeviceInterfaceID),(LPVOID*)&dd->mmc_intf)){

    dd->dev_intf = (*dd->mmc_intf)->GetSCSITaskDeviceInterface(dd->mmc_intf);
    if(dd->dev_intf == NULL){
      ret = 0;
    }
      }
      if(IOObjectRelease(sd)){
        printf("could not release sd");
        return 0;
      }

      if(ret) return(int)dd;
      else return 0;
}

char drive_close(int device){
  drive_data_s* dd = (drive_data_s*)device;
  (*dd->mmc_intf)->Release(dd->mmc_intf);
  IODestroyPlugInInterface(dd->plg_in_intf);
  IOObjectRelease(dd->itt);
  return 0;
}

drives_s* drives_available(){
  CFMutableDictionaryRef  main_dict = NULL;
  CFMutableDictionaryRef  sec_dict  = NULL;
  io_iterator_t           itt = IO_OBJECT_NULL;
  io_name_t               cls_name;
  io_service_t            sd = IO_OBJECT_NULL;
  IOCFPlugInInterface     **plg_in_intf = NULL;
  SCSITaskDeviceInterface **dev_intf = NULL;
  MMCDeviceInterface      **mmc_intf = NULL;
  SInt32                  score = 0;
  int count;
  int ret = 1;
  drive_s* d = 0;
  drives_s* dd = 0;

  if((main_dict = CFDictionaryCreateMutable(kCFAllocatorDefault,0,&kCFTypeDictionaryKeyCallBacks,&kCFTypeDictionaryValueCallBacks)) != NULL){
    if((sec_dict = CFDictionaryCreateMutable(kCFAllocatorDefault,0,&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks)) != NULL){
      CFDictionarySetValue(sec_dict,CFSTR(kIOPropertySCSITaskDeviceCategory),CFSTR(kIOPropertySCSITaskAuthoringDevice));
      CFDictionarySetValue(main_dict,CFSTR(kIOPropertyMatchKey),sec_dict);
      CFRelease(sec_dict);
    }
  }

  if(IOServiceGetMatchingServices(kIOMasterPortDefault,main_dict,&itt)){
    printf("no matching services\n");
    return 0;
  }

  count = 0;
  while((sd = IOIteratorNext(itt))) {
    if(!IOObjectGetClass(sd,cls_name)){
      if(!IOCreatePlugInInterfaceForService(sd,kIOMMCDeviceUserClientTypeID,kIOCFPlugInInterfaceID,&plg_in_intf,&score) &&
          !(*plg_in_intf)->QueryInterface(plg_in_intf,CFUUIDGetUUIDBytes(kIOMMCDeviceInterfaceID),(LPVOID*)&mmc_intf))
      {
        dev_intf = (*mmc_intf)->GetSCSITaskDeviceInterface(mmc_intf);
        if(dev_intf){
          drive_data_s dd;
          dd.mmc_intf = mmc_intf;
          dd.plg_in_intf = plg_in_intf;
          dd.itt = itt;
          dd.dev_intf = dev_intf;
          if(drive_type((int)&dd) == T_CDROM){
            inquiry_s* inq;
            count++;
            d = (drive_s*)realloc(d,count*sizeof(drive_s));
            inq = drive_inquiry((int)&dd);
            memcpy(&d[count-1].name,inq->p_id,sizeof(inq->p_id));
            d[count-1].id = count;
            free(inq);
          }
        }
      }
      if(IOObjectRelease(sd)) ret = 0;
    }
    if(mmc_intf != NULL)(*mmc_intf)->Release(mmc_intf);
    if(plg_in_intf != NULL)IODestroyPlugInInterface(plg_in_intf);
  }
  IOObjectRelease(itt);

  dd = (drives_s*)malloc(sizeof(drives_s));
  if(dd){
    dd->drives = d;
    dd->number = count;
  }else{
    free(d);
    return 0;
  }
  if(ret)
    return dd;
  else return 0;
}
#endif //__APPLE__
