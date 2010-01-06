#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "misc.h"
#include "lg_renesas.h"
#include <libmmc/libmmc.h>

#define STR_VERSION "0.95 Beta"
#define STR_NAME "Devilsclaw's Renesas Utility"
int drive = 0;
char force_checksum;  //bool type
char verify_firmware; //bool type

int cmd_verbose(void* Input)
{
  printf("VERBOSE: ON\n");
  return 1;
}

int cmd_debug(void* Input)
{
  printf("DEBUG: ON\n");
  return 1;
}

int cmd_version(void* Input)
{
  printf("VERSION: %s\n",STR_VERSION);
  return 1;
}

int cmd_help(void* Input)
{
  printf(
         "Usage: flasher (options)\n"
      "\n"
      "      --verbose       Enables verbose output\n"
      "      --debug         Enables debug output\n"
      "  -v, --version       Displays version info\n"
      "  -h, --help          Displays this message\n"
      "  -D, --drives        Displays available drives and ids\n"
      "  -d, --drive         What drive to use eg. e\n"
      "  -e, --erasecache    Zero out the cache memory\n"
      "    , --driveinfo     Displays detailed memory dumps\n"
      "  -V, --ver_firm      Verify flashed firmware\n"
      "  -p, --patchchksum   Patch a firmware to have the correct checksum\n"
      "  -m, --main          Dump main firmware to file eg. firm.bin\n"
      "  -c, --core          Dump core firmware to file eg. firm.bin\n"
      "  -C, --cache         Dump cache to file eg. cache.bin\n"
      "  -l, --dumploc       Dump a specified location\n"
      "  -f, --flash         Flash a firmware to the drive eg. official.bin\n"
      "  -F, --force         DANGEROUS!!! Developers only other wise dont!!\n"
      "  -r, --rip_exe       Rip the firmware from the exe\n"
      "  -s, --split         Split firmware dumped from exe\n"
      "  -n, --nologo        Stops the program from showing its title\n"
      "Usage Examples:\n"
      "WIN: flashing firmware: flasher -d e -f official.bin\n"
      "WIN: dumping  firmware: flasher -d e -c firm.bin\n"
      "LIN: flashing firmware: flasher -d /dev/hda -f official.bin\n"
      "LIN: dumping  firmware: flasher -d /dev/hda -c firm.bin\n");
  return 1;
}

int cmd_dumploc(void* Input){

  char** tmpInput = (char**)Input;
  char* buff = 0;
  size_t source,size,fsize;
  int loc;
  FILE* fileh;

  source = asciihex2int(tmpInput[1]);
  printf("cmd_dumploc: Starting dumping process\n");
  if(source == -1){
    printf("cmd_dumploc: Source was invalid hex string\n");
    return 0;
  }

  loc = asciihex2int(tmpInput[2]);
  if(loc == -1){
    printf("cmd_dumploc: Location passed was not a valid string of hex\n");
    return 0;
  }

  size = asciihex2int(tmpInput[3]);
  if(size == -1 || size == 0){
    printf("cmd_dumploc: Passed size was 0, or invalid hex string\n");
    return 0;
  }

  fsize = firm_dumper(drive,&buff,source,loc,size);
  if(!fsize){
    printf("cmd_dumploc: Failed to dump location\n");
    return 0;
  }

  if(fsize < size){
    printf("cmd_dumploc: Size of dump was smaller then asked still going to dump what we can\n");
  }

  //debug("cmd_dumploc: Opening %s\n",tmpInput[0]);
  fileh = fopen(tmpInput[0],"wb+");
  if(!fileh){
    free(buff);
    printf("cmd_dumploc: Failed to open %s\n",tmpInput[0]);
    return 0;
  }

  //debug("cmd_dumploc: Writing %s\n",tmpInput[0]);
  if(fwrite(buff,1,fsize,fileh) != fsize){
    free(buff);
    printf("cmd_dumploc: Failed to write %s\n",tmpInput[0]);
    return 0;
  }

  free(buff);
  fflush(fileh);
  fclose(fileh);
  printf("cmd_dumploc: Dump process finished\n");
  return 1;
}

int cmd_dumpmain(void* Input)
{
  char** tmpInput = (char**)Input;
  char  *buff = 0, *firm_out;
  size_t firm_start,firm_size,fsize,msb;
  unsigned short chksum;
  firminfo fi;
  inquiry_s* inquiry_d;
  FILE* fileh;


  printf("cmd_dumpmain: Starting dumping process\n");
  memset(&fi,0,sizeof(firminfo));

  if(!get_firminfo_drive(drive,main_pat,sizeof(main_pat),&fi)){
    printf("cmd_dumpmain: failed to get firmware info\n");
    return 0;
  }

  //LSB to MSB
  firm_start = swap32(fi.firm_start);
  firm_size  = swap32(fi.firm_size);

  fsize = firm_dumper(drive,&buff,LOC_MEMORY,firm_start,firm_size);
  if(!fsize){
    printf("cmd_dumpmain: failed to dump main firmware\n");
    return 0;
  }
  else if(fsize != firm_size){
    printf("cmd_dumpmain: dumped size and firmware size missmatch\n");
    return 0;
  }

  //debug("cmd_dumpmain: Opening %s\n",tmpInput[0]);
  fileh = fopen(tmpInput[0],"wb+");
  if(!fileh){
    printf("cmd_dumpmain: Failed to open %s\n",tmpInput[0]);
    free(buff);
    return 0;
  }

  inquiry_d = drive_inquiry(drive);
  if(!inquiry_d){
    free(buff);
    printf("cmd_dumpmain: Failed to inquiry drive info\n");
    return 0;
  }

  firm_out = (char*)malloc(fsize+0x400);
  if(!firm_out){
    free(inquiry_d);
    free(buff);
    printf("cmd_dumpmain: Failed to allocate memory for firm_out\n");
    return 0;
  }
  memset(firm_out,0,fsize+0x400);
  memcpy(&firm_out[0x400],buff,fsize);
  memcpy(firm_out,&fi,sizeof(firminfo));
  free(buff);
  memset(firm_out,0,4);

  memcpy(&firm_out[0x28],inquiry_d->p_rev,4);
  free(inquiry_d);

  msb = swap32(fsize);
  *(int*)(&firm_out[0x44]) = msb;

  fsize += 0x400;
  chksum = firm_chksum_calc(firm_out,fsize,0);
  chksum = swap16(chksum);
  *(short*)firm_out = chksum;

  if(firm_validate(firm_out,fsize)){
    free(firm_out);
    printf("cmd_dumpmain: Failed validation process\n");
    return 0;
  }

  //debug("cmd_dumpmain: Writing %s\n",tmpInput[0]);
  if(fwrite(firm_out,1,fsize,fileh) != fsize){
    free(firm_out);
    printf("cmd_dumpmain: Failed to write %s\n",tmpInput[0]);
    return 0;
  }

  free(firm_out);
  fflush(fileh);
  fclose(fileh);
  printf("cmd_dumpmain: Dump process finished\n");
  return 1;
}

int cmd_dumpcore(void* Input)
{
  char** tmpInput = (char**)Input;
  char* buff = 0,*firm_out;
  size_t firm_start,firm_size,fsize,msb;
  unsigned short chksum;
  firminfo fi;
  FILE* fileh;

  printf("cmd_dumpcore: Starting dumping process\n");
  memset(&fi,0,sizeof(firminfo));

  if(!get_firminfo_drive(drive,core_pat,sizeof(core_pat),&fi)){
    printf("cmd_dumpcore: failed to get firmware info\n");
    return 0;
  }

  //LSB to MSB
  firm_start = swap32(fi.firm_start);
  firm_size  = swap32(fi.firm_size);

  fsize = firm_dumper(drive,&buff,LOC_MEMORY,firm_start,firm_size);
  if(!fsize){
    printf("cmd_dumpcore: failed to dump core firmware\n");
    return 0;
  }
  else if(fsize != firm_size){
    printf("cmd_dumpcore: dumped size and firmware size missmatch\n");
    return 0;
  }

  //debug("cmd_dumpcore: Opening %s\n",tmpInput[0]);
  fileh = fopen(tmpInput[0],"wb+");
  if(!fileh){
    printf("cmd_dumpcore: Failed to open %s\n",tmpInput[0]);
    free(buff);
    return 0;
  }

  firm_out = (char*)malloc(fsize+0x400);
  if(!firm_out){
    free(buff);
    printf("cmd_dumpcore: Failed to allocate memory for firm_out\n");
    return 0;
  }
  memset(firm_out,0,fsize+0x400);
  memcpy(&firm_out[0x400],buff,fsize);
  memcpy(firm_out,&fi,sizeof(firminfo));
  free(buff);
  memset(firm_out,0,4);

  msb = swap32(fsize);
  *(int*)(&firm_out[0x44]) = msb;

  fsize += 0x400;
  chksum = firm_chksum_calc(firm_out,fsize,0);
  chksum = swap16(chksum);
  *(short*)firm_out = chksum;

  if(firm_validate(firm_out,fsize)){
    free(firm_out);
    printf("cmd_dumpcore: Failed validation process\n");
    return 0;
  }

  //debug("cmd_dumpcore: Writing %s\n",tmpInput[0]);
  if(fwrite(firm_out,1,fsize,fileh) != fsize){
    free(firm_out);
    printf("cmd_dumpcore: Failed to write %s\n",tmpInput[0]);
    return 0;
  }

  free(firm_out);
  fflush(fileh);
  fclose(fileh);
  printf("cmd_dumpcore: Dump process finished\n");
  return 1;
}

int cmd_flashfirm(void* Input)
{
  char** tmpInput = (char**)Input;
  char* buff = 0;
  size_t fsize,firm_start,firm_size;
  firminfo fi;

  printf("cmd_flashfirm: Flashing process started\n");
  fsize = falloc(tmpInput[0],&buff);
  if(!fsize){
    printf("cmd_flashfirm: Failed to falloc %s\n",tmpInput[0]);
    return 0;
  }

  if(!firm_flasher(drive,buff,fsize)){
    printf("cmd_flashfirm: Flashing process failed\n");
    free(buff);
    return 0;
  }

  memset(&fi,0,sizeof(firminfo));
  memcpy(&fi,buff,sizeof(firminfo));

  //LSB to MSB
  firm_start = swap32(fi.firm_start);
  firm_size  = swap32(fi.firm_size);
  //debug("fi.firm_size = %08X\nfi.firm_start = %08X\n",firm_size,firm_start);

  if(!firm_verify(drive,buff,firm_start,firm_size)){
    printf("cmd_flashfirm: Verification process failed\n");
    free(buff);
    return 0;
  }

  printf("cmd_flashfirm: Flashing process finished\n");
  free(buff);
  return 1;
}

int cmd_split(void* Input){

  char** tmpInput = (char**)Input;
  char* buff = 0;
  size_t fsize;

  printf("cmd_split: Starting split firmware process\n");
  fsize = falloc(tmpInput[0],&buff);
  if(!fsize){
    printf("cmd_split: Failed to falloc %s\n",tmpInput[0]);
    return 0;
  }

  if(!firm_spliter(buff,fsize,tmpInput[0])){
    free(buff);
    printf("cmd_split: Failed to split firmware process\n");
    return 0;
  }

  printf("cmd_split: Finished firmware splitting process\n");
  free(buff);
  return 1;
}

int cmd_ripexe(void* Input){
  char** tmpInput = (char**)Input;

  printf("cmd_ripexe: Starting firmware ripping process\n");
  if(!firm_ripexe(tmpInput[0])){
    printf("cmd_ripexe: Failed firmware ripping process\n");
    return 0;
  }
  printf("cmd_ripexe: Finished firmware ripping process\n");
  return 1;
}

int cmd_drive(void* Input)
{
  char** tmpInput = (char**)Input;
  int drive_id;

  printf("cmd_drive: Opening Drive: %s.\n",tmpInput[0]);
  drive_id = atoi(tmpInput[0]);
  if(!drive_id)return 0;

  if(!(drive = drive_open(drive_id))){
    return 0;
  }
  //verbose("cmd_drive: Drive %s Opened.\n",tmpInput[0]);
  return 1;
}

int cmd_driveinfo(void* Input)
{
  dump_drive_info(drive);
  return 1;
}

int cmd_test(void* Input)
{
  mmcdata_s d;
  memset(&d,0,sizeof(mmcdata_s));

  d.cmdsize = 6; //CDB6

  d.datasize = 0x24;
  d.data = (char*)calloc(1,d.datasize);

  d.cmd[0] = 0x12;
  d.cmd[1] = 0x00;
  d.cmd[2] = 0x00;
  d.cmd[3] = 0x00;
  d.cmd[4] = 0x24;
  d.cmd[5] = 0x00;
  d.cmd[6] = 0x00;
  d.cmd[7] = 0x00;
  d.cmd[8] = 0x00;
  d.cmd[9] = 0x00;
  return (drive_command(drive,&d,MMC_READ));
}

int cmd_clearcache(void* Input)
{
  return clr_cache(drive);
}

int cmd_dumpcache(void* Input)
{

  char** tmpInput = (char**)Input;
  char* buff = 0;
  size_t size,fsize;
  mmcdata_s d;
  FILE* fileh;

  printf("cmd_dumpcache: Starting dump cache process\n");
  memset(&d,0,sizeof(mmcdata_s));

  if(!lgren_read(drive,LOC_CACHE_SIZE,&d,0,4)){
    printf("cmd_dumpcache: Failed to get cache size\n");
    return 0;
  }
  size = swap32(*((int*)d.data));
  if(!size){
    printf("cmd_dumpcache: Cache size was 0\n");
    return 0;
  }

  fsize = firm_dumper(drive,&buff,LOC_CACHE,0,size);
  if(!fsize){
    printf("cmd_dumpcache: Dump cache process failed\n");
    return 0;
  }

  //verbose("cmd_dumpcache: Opening %s\n",tmpInput[0]);
  fileh = fopen(tmpInput[0],"wb+");
  if(!fileh){
    printf("cmd_dumpcache: Failed to open %s\n",tmpInput[0]);
    free(buff);
    return 0;
  }

  //debug("cmd_dumpcache: Writing %s\n",tmpInput[0]);
  if(fwrite(buff,1,fsize,fileh) != fsize){
    printf("cmd_dumpcache: Failed to write %s\n",tmpInput[0]);
    free(buff);
    return 0;
  }

  free(buff);
  fflush(fileh);
  fclose(fileh);
  printf("cmd_dumpcache: Finished dump cache process\n");
  return 1;
}

int cmd_forcechecksum(void* Input){
  char ans = 0;
  printf("WARNING!!!! Forcing checksum with a bad firmware can destroy your drive.\n"
      "This is for devolopers who need the checksum generated on a patched firmware.\n");
  do{
    printf("Are you sure you want to continue.(y/n):\n");
    ans = tolower(getchar());
  }while(ans != 'y' && ans != 'n');

  if(ans == 'y'){
    force_checksum = 1;
  }
  else return 0;
  return 1;
}

int cmd_checksum(void* Input){
  char** tmpInput = (char**)Input;
  char* buff = 0;
  size_t fsize;
  int v;
  unsigned short chksum;
  FILE* fileh;

  fsize = falloc(tmpInput[0],&buff);
  //Error messages handled in falloc
  if(!fsize)return 0;

  printf("cmd_checksum: Validating firmware\n");
  v = firm_validate(buff,fsize);
  if(v && v != 8){
    printf("cmd_checksum: Failed to validate firmware\n");
    return 0;
  }

  chksum = firm_chksum_calc(buff,fsize,0x0000);
  buff[0] = (chksum >> 8) & 0x000000FF;
  buff[1] = chksum & 0x000000FF;

  printf("cmd_checksum: Opening %s\n",tmpInput[0]);
  fileh = fopen(tmpInput[0],"wr+");
  if(!fileh){
    printf("cmd_checksum: Failed to open %s",tmpInput[0]);
    return 0;
  }

  if(fwrite(buff,1,fsize,fileh) != fsize){
    printf("cmd_checksum: Failed to write %s\n",tmpInput[0]);
    free(buff);
    return 0;
  }
  free(buff);
  printf("cmd_checksum: Finished\n");
  return 1;
}

int cmd_verifyfirm(void* Input){
  verify_firmware = 1;
  return 1;
}

int cmd_getdrives(void* Input){
  int count = 0;
  drives_s* dd;

  dd = drives_available();
  if(dd){
    printf("\nAVAILABLE DRIVES AND IDs:\n");
    for(count = 0;count < dd->number;++count){
       printf("Drive ID: %i Name: %s\n",dd->drives[count].id,dd->drives[count].name);
    }
  }
  else{
    printf("cmd_getdrives: unable to detect any cdrom drives\n");
    return 0;
  }

  return 1;
}
