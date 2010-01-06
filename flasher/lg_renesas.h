#ifndef LG_RENESAS_H
#define LG_RENESAS_H
#include <libmmc/libmmc.h>
#include <stdio.h>

extern char main_pat[72];
extern char core_pat[72];
extern char force_checksum;
extern char verify_firmware;

typedef struct firminfo{
  size_t checksum;
  size_t reserved[3];
  char   devname[16];
  char   revlvl[16];
  char   romrev[16];
  size_t firm_start;
  size_t firm_size;
}firminfo;

enum loc_e{
  LOC_CACHE_SIZE = 0x01, //Location points to the drives cache size
  LOC_CACHE      = 0x02, //location points to the cache
  LOC_MEMORY     = 0x05, //Location points to Absolute memory
  LOC_DEBUG      = 0x06, //Location points to debug info
};

enum mem6_e{
  MEM6_HOST_LOG     = 0x88000000, //Size: 0x0200 : Host Transfer LOG
  MEM6_DRV_TRACE1   = 0x80000000, //Size: 0x2000 : Drive Trace (Use trc_az.exe)
  MEM6_DRV_TRACE2   = 0x81000000, //Size: 0x0200 : Replaced Trace (Trace2)
  MEM6_DRIVE_LOG    = 0x82000000, //Size: 0x0400 : Drive LOG (Use log_viewer.exe)
  MEM6_PROC_ERR_LOG = 0x87000000  //Size: 0x0100 : Process Error LOG
};

enum mem5_e{
  MEM5_INNER_SRAM = 0x00000000, //Size: 0x8000 : Work RAM (Inner SRAM)
  MEM5_SRAM1      = 0x00800000, //Size: 0x8000 : Work RAM (SDRAM)
  MEM5_SRAM2      = 0x00808000, //Size: 0x8000 : Work RAM (SDRAM)
  MEM5_SRAM3      = 0x00810000, //Size: 0x8000 : Work RAM (SDRAM)
  MEM5_SRAM4      = 0x00818000, //Size: 0x8000 : Work RAM (SDRAM)
  MEM5_DSP        = 0x04000000, //Size: 0x0800 : Helios4 LSI Reg.
  MEM5_LDD        = 0x06000000, //Size: 0x0128 : LDD LSI Reg.
  MEM5_AFE        = 0x04000190, //Size: 0x00F0 : AFE LSI Read Write Reg.
  MEM5_SERVO_REG1 = 0x05000000, //Size: 0x0200 : (WORD)Servo Reg.1
  MEM5_SERVO_REG2 = 0x05000100, //Size: 0x0200 : (WORD)Servo Reg.2
  MEM5_SERVO_RAM1 = 0x05000400, //Size: 0x0200 : (WORD)Servo Ram1
  MEM5_SERVO_RAM2 = 0x05000500, //Size: 0x0200 : (WORD)Servo Ram2
  MEM5_SERVO_RAM3 = 0x05000600, //Size: 0x0200 : (WORD)Servo Ram3
  MEM5_CRAM0      = 0x05000900, //Size: 0x0200 : (WORD)CRAM0
  MEM5_CRAM1      = 0x05000B00, //Size: 0x0200 : (WORD)CRAM1
  MEM5_CRAM2      = 0x05000D00, //Size: 0x0200 : (WORD)CRAM2
  MEM5_CRAM3      = 0x05000F00, //Size: 0x0200 : (WORD)CRAM3
  MEM5_EEPROM     = 0x01000000, //Size: 0x0900 : Drive EEPROM
  MEM5_DVDRAM_43  = 0x132B0000, //Size: 0x0800 : PDL
  MEM5_DVDRAM_44  = 0x132C0000, //Size: 0x0800 : SDL,RMD,TOC
  MEM5_DVDRAM_45  = 0x132D0000, //Size: 0x0800 : Dpool,RMD2,DCB
  MEM5_DVDRAM_46  = 0x132E0000, //Size: 0x0800 : Sector Status
  MEM5_DVDRAM_47  = 0x132F0000  //Size: 0x0800 : Control Data
};

char* lgren_sread(int device,char source,int pos,int size);
int lgren_diag(int device);
int lgren_read(int device,char source,mmcdata_s* buff,int pos,int size);
int lgren_write(int device,char source,mmcdata_s* buff,int pos,int size);
char get_firminfo_drive(int device,char* pattern,size_t patsize,firminfo* fi);
char get_firminfo_buff(char* buff,char* pattern,size_t bsize,size_t patsize,firminfo* fi);
unsigned short firm_chksum_calc(char* buff,size_t size,short key);
int firm_validate(char* buff, size_t size);
char firm_flasher(int device,char* buff,size_t fsize);
char firm_verify(int device,char* buff,size_t firm_start,size_t firm_size);
size_t firm_dumper(int device,char** inbuff,int loc,int pos,size_t dsize);
char firm_spliter(char* buff,int max_size,const char* outfile);
int firm_demangle(char* buff,char* buff2,int firm_start,int firm_stop,int firm_end);
int get_firmtype(char* buff);
int get_enckeypos(char* buff,int type);
int get_enctype(char* buff);
char firm_decrypter(char* buff,int type,int key,size_t size);
void info_dump(int device,char loc,int mem,int size,const char* name);
void dump_drive_info(int device);
char clr_cache(int device);
size_t find_pattern_drive(int device,int loc,char* pattern,size_t psize);
char firm_ripexe(char* filename);
#endif //LG_RENESAS_H
