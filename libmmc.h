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

#ifndef BASE_MMC_H
#define BASE_MMC_H

#if defined(__linux__)
#include <stdio.h>
#include <linux/cdrom.h>

/* CGC_DATA_UNKNOWN = 0 */
/* CGC_DATA_WRITE   = 1 */
/* CGC_DATA_READ    = 2 */
/* CGC_DATA_NONE    = 3 */
#define MMC_UNKNOWN CGC_DATA_UNKNOWN
#define MMC_WRITE   CGC_DATA_WRITE
#define MMC_READ    CGC_DATA_READ
#define MMC_NONE    CGC_DATA_NONE

#endif /* LINUX */

typedef struct inquiry_s
{
  char p_qual;			/* Peripheral qualifier */
  char p_devtype;		/* Peripheral_device_type */
  char rmb;			/* Removable Medium ;bool type */
  char devtype_mod;		/* Device-type modifier */
  char iso_ver;			/* ISO version */
  char ecma_ver;		/* ECMA version */
  char ansi_ver;		/* ANSI-approved version */
  char aec;			/* AEC   ;bool type */
  char triop;			/* TrIOP ;bool type */
  char resdata_for;		/* Response data format */
  char reladr;			/* RelAdr;bool type */
  char wbus32;			/* WBus32;bool type */
  char wbus16;			/* WBus16;bool type */
  char sync;			/* Sync  ;bool type */
  char linked;			/* linked;bool type */
  char cmdque;			/* CmdQue;bool type */
  char sftre;			/* SftRe ;bool type */
  char ven_id[9];		/* Vendor ID  //padded with 1 byte */
  char p_id[17];		/* Product ID //padded with 1 byte */
  char p_rev[5];		/* Product revision level padded with 1 byte */
  char ven_spec[3];		/* Vendor Specific padded with 1 byte */
} inquiry_s;

typedef struct senret
{
  char sk;
  char asc;
  char ascq;
} senret;

typedef struct mmcdata_s
{
  char cmd[16];
  size_t cmdsize;
  char *data;
  size_t datasize;
  char sensedata[24];
  senret senseret;
} mmcdata_s;

typedef struct drive_s
{
  char name[17];
  size_t id;
} drive_s;

typedef struct drives_s
{
  size_t number;
  drive_s *drives;
} drives_s;

/* cdb_cmd_e */
enum
{
  CMD_INQUIRY = 0x12,		/* Inquiry of drive */
  CMD_READ = 0x3C,		/* Read memory of various locations */
  CMD_WRITE = 0x3B,		/* Write memory of various locations */
  CMD_READ_TOC = 0x43,		/* Read Table of content */
  CMD_DISC_INF = 0x51,		/* Read disk info */
  CMD_TRACK_INF = 0x52,		/* Read Track info */
  CMD_MODE_SENSE = 0x5A,	/* Mode Sense */
};

/* mode_sense_e */
enum
{
  SEN_ERROR_RECOV = 0x01,	/* Error Recover Read/Write */
  SEN_WRITE_PARAM = 0x05,	/* Write Param */
  SEN_CD_AUDIO = 0x0E,		/* CD Audio */
  SEN_POWER_COND = 0x1A,	/* Power Condition */
  SEN_FAILUR_REPT = 0x1C,	/* Failur Report */
  SEN_TIMEOUT_PRO = 0x1D,	/* Time-out and Protect */
  SEN_CAPA_MECHA = 0x2A		/* Capa and Mecha State */
};

/* devtypes */
enum
{
  T_DIRECT = 0x00,		/* direct-access device (e.g., magnetic disk) */
  T_SEQUENTIAL = 0x01,		/* sequential-access device (e.g., magnetic tape) */
  T_PRINTER = 0x02,		/* printer device */
  T_PROCESSOR = 0x03,		/* processor device */
  T_WRITE_ONCE = 0x04,		/* write-once device */
  T_CDROM = 0x05,		/* CDROM device */
  T_SCANNER = 0x06,		/* scanner device */
  T_OPTICAL_MEM = 0x07,		/* optical memory device (e.g., some optical disks) */
  T_MEDIUM_CHANGER = 0x08,	/* medium Changer (e.g. jukeboxes) */
  T_COMMUNICATION = 0x09,	/* communications device */
  T_GRAPHIC_1 = 0x0A,		/* defined by ASC IT8 (Graphic arts pre-press devices) */
  T_GRAPHIC_2 = 0x0B,		/* defined by ASC IT8 (Graphic arts pre-press devices) */
  T_STORAGE_ARRAY = 0x0C,	/* Storage array controller device (e.g., RAID) */
  T_ENCLOSURE = 0x0D,		/* Enclosure services device */
  T_SIMP_DIRRECT = 0x0E,	/* Simplified direct-access device (e.g., magnetic disk) */
  T_OPTICAL_CARD_R = 0x0F,	/* Optical card reader/writer device */
  T_RESERVED_1 = 0x10,		/* Reserved for bridging expanders */
  T_OBJ = 0x11,			/* Object-based Storage Device */
  T_AUTOMATIC = 0x12,		/* Automation/Drive Interface */
  T_RESERVED_2 = 0x13,		/* reserved */
  T_RESERVED_3 = 0x1D,		/* reserved */
  T_WELL_KNOWN = 0x1E,		/* Well known logical unit */
  T_UNKNOWN = 0x1F		/* unknown or no device type */
};

#define SECTOR_SIZE 0x8000

inquiry_s *drive_inquiry (int device);
int drive_ready (int device);
int drive_stop (int device);
int drive_eject (int device);
int drive_safe_eject (int device);
char drive_type (int device);
int drive_command (int drive, mmcdata_s * d, int direction);
int drive_open (char *device);
char drive_close (FILE * intdevice);
drives_s *drives_available (void);

#endif /* BASE_MMC_H */
