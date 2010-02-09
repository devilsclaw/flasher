#ifndef CMD_ALL_H
#define CMD_ALL_H
#define STR_NAME "Devilsclaw's LG Renesas Drive Utility"
extern char verify_firmware;

int cmd_version(void* Input);
int cmd_help(void* Input);
int cmd_dumploc(void* Input);
int cmd_dumpmain(void* Input);
int cmd_dumpcore(void* Input);
int cmd_flashfirm(void* Input);
int cmd_ripexe(void* Input);
int cmd_drive(void* Input);
int cmd_verifyfirm(void* Input);
int cmd_getdrives(void* Input);
#endif
