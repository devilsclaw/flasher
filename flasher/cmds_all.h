#ifndef CMD_ALL_H
#define CMD_ALL_H
#define STR_VERSION "0.95 Beta"
#define STR_NAME "Devilsclaw's Renesas Utility"
extern char force_checksum;
extern char verify_firmware;

int cmd_verbose(void* Input);
int cmd_debug(void* Input);
int cmd_version(void* Input);
int cmd_help(void* Input);
int cmd_dumploc(void* Input);
int cmd_dumpmain(void* Input);
int cmd_dumpcore(void* Input);
int cmd_flashfirm(void* Input);
int cmd_split(void* Input);
int cmd_ripexe(void* Input);
int cmd_drive(void* Input);
int cmd_driveinfo(void* Input);
int cmd_test(void* Input);
int cmd_clearcache(void* Input);
int cmd_dumpcache(void* Input);
int cmd_forcechecksum(void* Input);
int cmd_checksum(void* Input);
int cmd_verifyfirm(void* Input);
int cmd_getdrives(void* Input);
#endif
