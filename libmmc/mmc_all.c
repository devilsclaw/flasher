#include "libmmc/libmmc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//suffix _l means local
//suffix _s means struct
//suffix _e means enumerate

//CDB: 12h,0,0,0,size,0
inquiry_s* drive_inquiry(int device){
  int ret = 1;
  mmcdata_s data_l;
  inquiry_s* inquiry_l;
  memset(&data_l,0,sizeof(mmcdata_s));
  data_l.cmdsize = 6; //CDB6
  data_l.datasize = 0x24;
  data_l.data = (char*)calloc(1,data_l.datasize);

  inquiry_l = (inquiry_s*)calloc(1,sizeof(inquiry_s));
  if(!inquiry_l){
    printf("drive_inquiry: failed to allocate memory for inquiry_l\n");
    ret = 0;
  } else {
    data_l.cmd[0] = 0x12;
    data_l.cmd[4] = (char)data_l.datasize;
    if(!drive_command(device,&data_l,MMC_READ)){
      printf("drive_inquiry: failed to inquiry drive info\n");
      ret = 0;
    } else {
      inquiry_l->p_qual      = (data_l.data[0] >> 5) & 7;     //(byte)
      inquiry_l->p_devtype   =  data_l.data[0]       & 0x1F;  //(byte)
      inquiry_l->rmb         = (data_l.data[1] >> 7) & 1;     //(bool)
      inquiry_l->devtype_mod =  data_l.data[1]       & 0x7F;  //(char)

      inquiry_l->iso_ver     = (data_l.data[2] >> 6) & 3;     //(char)
      inquiry_l->ecma_ver    = (data_l.data[2] >> 4) & 7;     //(char)
      inquiry_l->ansi_ver    =  data_l.data[2]       & 7;     //(char)

      inquiry_l->aec         = (data_l.data[3] >> 7) & 1;     //(bool)
      inquiry_l->triop       = (data_l.data[3] >> 6) & 1;     //(bool)
      inquiry_l->resdata_for =  data_l.data[3]       & 0xF;   //(char)

      inquiry_l->reladr      = (data_l.data[7] >> 7) & 1;     //(bool)
      inquiry_l->wbus32      = (data_l.data[7] >> 6) & 1;     //(bool)
      inquiry_l->wbus16      = (data_l.data[7] >> 5) & 1;     //(bool)
      inquiry_l->sync        = (data_l.data[7] >> 4) & 1;     //(bool)
      inquiry_l->linked      = (data_l.data[7] >> 3) & 1;     //(bool)
      inquiry_l->cmdque      = (data_l.data[7] >> 1) & 1;     //(bool)
      inquiry_l->sftre       =  data_l.data[7]       & 1;     //(bool)

      memcpy(inquiry_l->ven_id  ,&(data_l.data[8]) ,8);       //(8  bytes)
      memcpy(inquiry_l->p_id    ,&(data_l.data[16]),16);      //(16 bytes)
      memcpy(inquiry_l->p_rev   ,&(data_l.data[32]),4);       //(4  bytes)
      memcpy(inquiry_l->ven_spec,&(data_l.data[36]),2);       //(2  bytes)
    }
  }
  free(data_l.data);
  if(ret)
    return(inquiry_l);
  else
    return 0;
}

//CDB 0,0,0,0,0,0
int drive_ready(int device){
  mmcdata_s data_l;
  memset(&data_l,0,sizeof(mmcdata_s));
  data_l.cmdsize = 6; //CDB6
  return(drive_command(device,&data_l,MMC_READ));
}

//Stop Spinig of the drive
//CDB 1Bh,0,0,0,0,0
int drive_stop(int device){
  mmcdata_s data_l;
  memset(&data_l,0,sizeof(mmcdata_s));
  data_l.cmdsize = 6; //CDB6

  data_l.cmd[0] = 0x1B;
  return (drive_command(device,&data_l,MMC_READ));
}

//CDB 1Bh,0,0,0,2,0
int drive_eject(int device){
  mmcdata_s data_l;
  memset(&data_l,0,sizeof(mmcdata_s));
  data_l.cmdsize = 6; //CDB6

  data_l.cmd[0] = 0x1B;
  data_l.cmd[4] = 0x02;
  return(drive_command(device,&data_l,MMC_READ));
}

//Safe Eject
int drive_safe_eject(int device){
  if(!drive_stop(device) || !drive_eject(device)){
    return 0;
  }
  return 1;
}

char drive_type(int device){
  inquiry_s* inquiry_d = drive_inquiry(device);
  int drivetype = -1;
  if(inquiry_d){
    drivetype = (inquiry_d)->p_devtype;
    free(inquiry_d);
  }
  return drivetype;
}
