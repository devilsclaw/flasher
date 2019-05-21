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

#include "lg_renesas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "cui_all.h"
char core_pat[] =
  { 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 'C', 'O', 'R', 'E', 0x20,
  0x20, 0x20, 0x20,
  'R', 'E', 'V', 0x20, 'L', 'V', 'L', 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  'R', 'O', 'M', 0x20, 'R', 'E', 'V', 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

char main_pat[] =
  { 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 'M', 'A', 'I', 'N', 0x20,
  0x20, 0x20, 0x20,
  'R', 'E', 'V', 0x20, 'L', 'V', 'L', 0x20, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
  0x00, 0x00, 0x00,
  'R', 'O', 'M', 0x20, 'R', 'E', 'V', 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

/* Simple Read */
char *
lgren_sread (int device, char source, size_t pos, size_t size)
{
  mmcdata_s d;
  memset (&d, 0, sizeof (mmcdata_s));

  if (!lgren_read (device, source, &d, pos, size))
    {
      free (d.data);
      return 0;
    }
  return (d.data);
}

int
lgren_diag (int device)
{
  char data[6] = { 0 };
  mmcdata_s d;
  memset (&d, 0, sizeof (mmcdata_s));

  d.cmdsize = 6;		/* CDB6 */
  d.data = data;
  d.datasize = 6;

  d.data[0] = 0x23;		/* this was done by the flasher so are we */
  d.data[5] = (char) 0x80;

  d.cmd[0] = 0x1D;
  d.cmd[1] = 0x01;
  d.cmd[4] = 0x06;
  return (drive_command (device, &d, MMC_WRITE));
}

/* read any memory location from the drive */
int
lgren_read (int device, char source, mmcdata_s * d, size_t pos, size_t size)
{
  char *buffer = 0;
  if (!(d->data) && size)
    {
      buffer = (char *) malloc (size);
      if (!buffer)
	{
	  return 0;
	}
      d->data = buffer;
      d->datasize = size;
      memset (d->data, 0, d->datasize);
    }

  d->cmdsize = 10;		/* CDB10 */
  d->cmd[0] = CMD_READ;
  d->cmd[1] = ((!source) ? LOC_MEMORY : source);
  *((int *) (&d->cmd[2])) = swap32 (pos);	/* LSB to MSB fill [2] through [5] */
  d->cmd[6] = 0x00;
  *((short *) (&d->cmd[7])) = swap16 (size);	/* LSB to MSB fill [7] through [8] */
  d->cmd[9] = 0x44;
  return (drive_command (device, d, MMC_READ));
}

/* used to write to memory location on the drive */
/* so far only cache is reliable to write to. */
int
lgren_write (int device, char source, mmcdata_s * d, size_t pos, size_t size)
{
  d->cmdsize = 10;		/* CDB10 */
  d->cmd[0] = CMD_WRITE;
  d->cmd[1] = source;
  *((int *) (&d->cmd[2])) = swap32 (pos);	/* LSB to MSB fill [2] through [5] */
  d->cmd[6] = 0x00;
  *((short *) (&d->cmd[7])) = swap16 (size);	/* LSB to MSB fill [7] through [8] */
  d->cmd[9] = 0x44;
  return (drive_command (device, d, MMC_WRITE));
}

char
get_firminfo_drive (int device, char *pattern, size_t patsize, firminfo * fi)
{
  char tdata[8] = { 0 };
  size_t firm_hdrloc;
  mmcdata_s d;

  memset (&d, 0, sizeof (mmcdata_s));

  firm_hdrloc = find_pattern_drive (device, LOC_MEMORY, pattern, patsize);
  if (!firm_hdrloc)
    {
      printf ("get_firminfo_drive: failed to find pattern\n");
      return 0;
    }

  d.data = (char *) fi;
  d.datasize = sizeof (firminfo);
  if (!lgren_read (device, LOC_MEMORY, &d, firm_hdrloc, d.datasize))
    {
      printf ("get_firminfo: failed to read firmware\n");
      return 0;
    }

  if (!fi->firm_size)
    {
      printf ("get_firminfo_drive: firmware stop location invalid\n");
      return 0;
    }
  else if (fi->firm_size == -1)
    {
      d.datasize = 8;
      d.data = tdata;
      if (!lgren_read
	  (device, LOC_MEMORY, &d, swap32 (fi->firm_start) + 0xA0,
	   d.datasize))
	{
	  printf ("get_firminfo_drive: failed to get firmware info 1\n");
	  return 0;
	}

      if (((int *) d.data)[0] != fi->firm_start)
	{
	  if (!lgren_read
	      (device, LOC_MEMORY, &d, swap32 (fi->firm_start) + 0x5A0,
	       d.datasize))
	    {
	      printf ("get_firminfo_drive: failed to get firmware info 2\n");
	      return 0;
	    }
	}

      if (((int *) d.data)[0] != fi->firm_start)
	{
	  printf
	    ("get_firminfo_drive: original firmware location and new different\n");
	  return 0;
	}
      fi->firm_size = ((int *) d.data)[1];
      if (!fi->firm_size || fi->firm_size == -1)
	{
	  printf ("get_firminfo_drive: failed to get firmware info\n");
	  return 0;
	}
    }
  return 1;
}

char
get_firminfo_buff (char *buff, char *pattern, size_t bsize, size_t patsize,
		   firminfo * fi)
{
  char tdata[8] = { 0 };
  int tdatasize = 8;
  size_t firm_hdrloc;

  firm_hdrloc = find_pattern_buff (buff, pattern, bsize, patsize);
  if (!firm_hdrloc)
    {
      printf ("get_firminfo_file: failed to find pattern\n");
      return 0;
    }

  memcpy (fi, &buff[firm_hdrloc], sizeof (firminfo));
  if (!fi->firm_size)
    {
      printf ("get_firminfo_file: firmware stop location invalid\n");
      return 0;
    }
  else if (fi->firm_size == -1)
    {
      memcpy (tdata, &buff[0x4A0], tdatasize);

      if (((int *) tdata)[0] != fi->firm_start)
	{
	  memcpy (tdata, &buff[0x9A0], tdatasize);
	}

      if (((int *) tdata)[0] != fi->firm_start)
	{
	  printf
	    ("get_firminfo_file: original firmware location and new different\n");
	  return 0;
	}
      fi->firm_size = ((int *) tdata)[1];
      if (!fi->firm_size || fi->firm_size == -1)
	{
	  printf ("get_firminfo_file: failed to get firmware info\n");
	  return 0;
	}
    }
  return 1;
}

/* calculates the checksum */
unsigned short
firm_chksum_calc (char *buff, size_t size, short key)
{
  short *buff2 = (short *) buff;
  unsigned short chksum;
  size_t count;

  /* skip first two bytes */
  chksum = key;
  for (count = 1; count < (size / 2); count++)
    {
      /* byte swap then add into chksum */
      chksum +=
	((buff2[count] & 0x000000FF) << 8) | ((buff2[count] & 0x0000FF00) >>
					      8);
    }
  /* produce final result */
  chksum = ~chksum;

  /* verbose("firm_chksum_calc: chksum = 0x%04X\n", chksum & 0x0000FFFF); */
  return chksum;
}

/* Check various things about the firmware that should stay common */
int
firm_validate (char *buff, size_t size)
{
  size_t firm_size;
  int type;

  if (strncmp (&buff[0x20], "REV LVL ", 8))
    {
      /* verbose("firm_validate: REV LVL check failed\n"); */
      return 1;
    }

  if (strncmp (&buff[0x30], "ROM REV ", 8))
    {
      /* verbose("firm_validate: ROM REV check failed\n"); */
      return 2;
    }

  firm_size = size - 0x400;
  if (swap32 (*(int *) (&buff[0x44])) != firm_size)
    {
      /* verbose("firm_validate: Firmware size check 1 failed\n"); */
      return 3;
    }

#if defined(__notyet__)
  /* This may not be valid since it is posible that there are 0's in there */
  for (int count = 0x400; count < 0x410; count++)
    {
      if (!buff[count])
	{
	  verbose ("VALIDATE: REV LVL check failed\n");
	  return 4;
	}
    }
#endif

  type = get_firmtype (buff);
  /* MAIN */
  if (type == 1)
    {

#if defined(__notyet__)
      /* not valid on all firmware only newer so far */
      if (swap32 (*(int *) (&buff[0x4A4])) != firm_size)
	{
	  verbose ("firm_validate: Firmware size check 2 failed\n");
	  return 5;
	}
      if (swap32 (*(int *) (&buff[0x4B4])) != firm_size)
	{
	  verbose ("firm_validate: Firmware size check 3 failed\n");
	  return 6;
	}
#endif

      if (strncmp (&buff[0x400], &buff[size - 0x10], 0x10))
	{
	  /* verbose("firm_validate: 16 byte string test failed\n"); */
	  return 7;
	}
    }

  /* CORE */
#if defined(__notyet__)
  /* not valid on all firmware only newer so far */
  else if (type == 2)
    {
      if (swap32 (*(int *) (&buff[0x9A4])) != firm_size)
	{
	  verbose ("firm_validate: Firmware size check 2 failed\n");
	  return 6;
	}

      /* not valid on all firmware only newer so far */
      if (swap32 (*(int *) (&buff[0x9B4])) != firm_size)
	{
	  verbose ("firm_validate: Firmware size check 2 failed\n");
	  return 7;
	}
    }
#endif

  if (firm_chksum_calc (buff, size, swap16 (*(short *) buff)))
    {
      /* verbose("firm_validate: Checksum check failed\n"); */
      return 8;
    }
  return 0;
}

char
firm_flasher (int device, char *buff, size_t fsize)
{
  mmcdata_s d;
  size_t loop, size, pos;
  char source;
  int v;
  int force_checksum = 0;
  u_int16_t chksum = 0;

  memset (&d, 0, sizeof (mmcdata_s));

  v = firm_validate (buff, fsize);

  /* this will generate and patch the firmware with
     the right checksum for its data DANGEROUS!!!
     you best know the firmware is not corrupted if you do this */
  if (v == 8 && force_checksum)
    {
      chksum = firm_chksum_calc (buff, fsize, 0x0000);
      buff[0] = (chksum >> 8) & 0x000000FF;
      buff[1] = chksum & 0x000000FF;
    }
  else if (v)
    {
      printf ("firm_flasher: Firmware validation failed: %i\n", v);
      return 0;
    }

  /* check if drive is ready including if cdrom is in drive
     if no cdrom is in drive the its not ready which is
     what we want. it needs to fail or we should not flash */
  for (loop = 0; loop < 4; loop++)
    if (drive_ready (device))
      if (!drive_safe_eject (device))
	{
	  printf
	    ("firm_flasher: Please remove cd from drive and then try again.\n");
	  return 0;
	}

  printf ("\nfirm_flasher: Drive should be flashing its light.\n"
	  "firm_flasher: This indicates its flashing its self.\n"
	  "firm_flasher: This can take a while, Please be patiant.\n");

  d.data = buff;
  d.datasize = 0x8000;
  size = 0;
  source = 0;
  for (pos = 0; pos < fsize;)
    {
      size =
	((d.datasize + pos <
	  fsize) ? d.datasize : d.datasize - ((d.datasize + pos) - fsize));
      d.data = &buff[pos];

      if (pos + size == fsize)
	{
	  source = LOC_MEMORY;
	  d.datasize = size;
	}
      else
	source = LOC_CACHE;

      /* write to the cache of the drive */
      if (!lgren_write (device, source, &d, pos, size))
	{
	  printf ("firm_flasher: Failed to write at 0x%08X\n"
		  "firm_flasher: your drive should still be fine.\n",
		  (unsigned int) pos);
	  return 0;
	}
      pos += size;
      ud_progress ((pos / (double) fsize));
      /* printf("\b\b\b\b\b\b\b\b\b\b\b%.2f",(pos/(float)fsize)*100.0); */
      fflush (stdout);
    }

  if (!lgren_diag (device))
    {
      printf ("firm_flasher: Failed Self diag. Possibly bad drive flash!\n");
      return 0;
    }
  return 1;
}

char
firm_verify (int device, char *buff, size_t firm_start, size_t firm_size)
{
  int loop = 0;
  char *buff2;
  size_t fsize2;

  printf ("firm_verify: Verifying firmware\n");
  for (loop = 0; loop < 4; loop++)
    {
      __sleep (1000);
      drive_ready (device);
    }


  buff2 = 0;
  fsize2 = firm_dumper (device, &buff2, LOC_MEMORY, firm_start, firm_size);
  if (!fsize2)
    {
      printf ("firm_verify: Failed to dump firmware for verification\n");
      return 0;
    }
  else
    {
      if (!cmp_buff (&buff[0x400], buff2, fsize2))
	{
	  printf ("firm_verify: Verification failed\n");
	  return 0;
	}
    }
  return 1;
}

/* dump the firmware of the drive to file */
/* based off GSA-T21N and tested on GMA-4082N */
size_t
firm_dumper (int device, char **inbuff, int loc, size_t pos, size_t dsize)
{
  size_t start_pos, stop_pos, dumpped_size, size;
  /* size_t dump_size; */
  char *buff;
  mmcdata_s d;

  *inbuff = 0;

  if (!dsize)
    {
      return 0;
    }

  memset (&d, 0, sizeof (mmcdata_s));

  start_pos = pos;
  stop_pos = dsize + pos;
  /* dump_size = dsize; */
  dumpped_size = 0;

  buff = (char *) calloc (1, dsize);
  if (!buff)
    {
      printf ("firm_dumper: failed to allocate memory for buff\n");
      return 0;
    }

  /* TODO: Update code so that is only does one copy instead of creating a buffer */
  /* then copying it to the other buffer. */
  size = 0;
  while (start_pos < stop_pos)
    {
      /* calculate the current block size */
      size =
	((SECTOR_SIZE + start_pos <
	  stop_pos) ? SECTOR_SIZE : SECTOR_SIZE - ((SECTOR_SIZE + start_pos) -
						   stop_pos));
      if (!lgren_read (device, loc, &d, start_pos, size))
	{
	  printf
	    ("firm_dumper: Failed to read from drive. Will write what we got so far\n");
	  break;
	}

      memcpy (&buff[dumpped_size], d.data, size);

      /* update possition and percentage counter */
      start_pos += size;
      dumpped_size += size;
      free (d.data);		/* clean up read_firmware memory */
      d.data = 0;
      /* printf("\b\b\b\b\b\b\b\b\b\b\b\b%.2f",(dumpped_size/(double)dump_size)); */
      /* ud_progress((dumpped_size/(double)dump_size)); */
      fflush (stdout);		/* print to screen now */
    }
  *inbuff = buff;
  return dumpped_size;
}

char
firm_spliter (char *buff, size_t max_size, const char *outfile)
{
  size_t firm_size, fpos, loop;
  FILE *fileh;

  firm_size = 0;
  fpos = 0;
  loop = 0;
  do
    {
      int type;
      char *ofile;
      firm_size = swap32 (*(int *) (&buff[fpos + 0x44]));
      if (!firm_size)
	{
	  break;
	}

      firm_size += 0x400;

      type = get_firmtype (&buff[fpos]);
      ofile = calloc (1, strlen (outfile) + 6);
      if (type == 1)
	{
	  strcat (ofile, "MAIN_");
	}
      else if (type == 2)
	{
	  strcat (ofile, "CORE_");
	}
      else
	{
	  printf ("firm_spliter: Could not find firmware type\n");
	  free (ofile);
	  return 0;
	}
      strcat (ofile, outfile);

      if (!(fileh = fopen (ofile, "wb+")))
	{
	  printf ("firm_spliter: Unable to create %s\n", ofile);
	  return 0;
	}

      if (fwrite (&buff[fpos], 1, firm_size, fileh) != firm_size)
	{
	  printf ("firm_spliter: Failed to write data to %s\n", ofile);
	  fclose (fileh);
	  return 0;
	}
      fclose (fileh);

      if (firm_chksum_calc
	  (&buff[fpos], firm_size, swap16 (*(short *) (&buff[fpos]))))
	{
	  printf
	    ("firm_spliter: checksum failed for %s. but was still written to drive anyway.\n",
	     ofile);
	}
      fpos += firm_size;
      loop++;
      free (ofile);
    }
  while (firm_size && fpos < max_size);

  if (!loop)
    {
      return 0;
    }
  return 1;
}

int
firm_demangle (char *buff, char *buff2, size_t firm_start, size_t firm_stop,
	       size_t firm_end)
{
  size_t cur_pos, count2, block_start, block_size, count1;
  char *manginfo;

  manginfo = &buff[firm_start];
  cur_pos = firm_start + 0x204;
  block_start =
    ((((((manginfo[4] & 0xFF) << 8) + (manginfo[5] & 0xFF)) << 8) +
      (manginfo[6] & 0xFF)) << 8) + (manginfo[7] & 0xFF);
  block_size =
    ((((((manginfo[0x104] & 0xFF) << 8) + (manginfo[0x105] & 0xFF)) << 8) +
      (manginfo[0x106] & 0xFF)) << 8) + (manginfo[0x107] & 0xFF);
  count1 = 0;

  manginfo = &manginfo[5];
  count2 = 0;
  for (; cur_pos < firm_end; count2++, cur_pos++)
    {
      if (block_size && (cur_pos == block_start))
	{
	  for (; block_size; block_size--, count2++)
	    {
	      if (buff2)
		buff2[count2] = 0;
	    }

	  count1++;
	  manginfo += 4;
	  if (count1 < 0x40)
	    {
	      block_start =
		((((((manginfo[-1] & 0xFF) << 8) +
		    (manginfo[0] & 0xFF)) << 8) +
		  (manginfo[1] & 0xFF)) << 8) + (manginfo[2] & 0xFF);
	      block_size =
		((((((manginfo[0xFF] & 0xFF) << 8) +
		    (manginfo[0x100] & 0xFF)) << 8) +
		  (manginfo[0x101] & 0xFF)) << 8) + (manginfo[0x102] & 0xFF);
	    }
	  else
	    {
	      block_start = 0;
	      block_size = 0;
	    }
	}
      if (buff2)
	buff2[count2] = buff[cur_pos];
    }
  return (int) count2;
}

int
get_firmtype (char *buff)
{
  int pos, count;

  for (pos = 0; pos < 0x400; pos++)
    {
      for (count = 0; count + pos < 0x400; count++)
	{
	  if (count + 4 <= 0x400)
	    {
	      if (!strncmp (&buff[count], "MAIN", 4))
		{
		  return 1;
		}

	      if (!strncmp (&buff[count], "CORE", 4))
		{
		  return 2;
		}
	    }
	  else
	    break;
	}
    }
  return 0;
}

int
get_enckeypos (char *buff, int type)
{
  char udtag[] = "Update Data for HLDS's Drive";
  char temp[0x200];
  size_t brute, count;

  for (brute = 0x100; brute >= sizeof (udtag); brute--)
    {
      size_t key2;
      memcpy (temp, &buff[0 - brute], brute);
      key2 = ((temp[0] << 8) & 0x0000FF00) | (temp[1] & 0x000000FF);
      if (!firm_decrypter (&temp[2], type, key2, brute))
	{
	  return 0;
	}
      for (count = 0; count < brute; count++)
	{
	  if (count + sizeof (udtag) - 1 <= sizeof (temp))
	    {
	      if (!strncmp (&temp[count], udtag, sizeof (udtag) - 1))
		{
		  return brute;
		}
	    }
	  else
	    break;
	}
    }
  return 0;
}

int
get_enctype (char *buff)
{
  int ret = 0, type;

  for (type = 1; type <= 3; type++)
    {
      ret = get_enckeypos (buff, type);
      if (ret)
	return type;
    }
  return 0;
}

char
firm_decrypter (char *buff, int type, size_t key, size_t size)
{
  size_t tkey = 0, tmanip = 0, count;

  switch (type)
    {
    case 1:
      if (!key)
	key = 1;
      key = key & 0x0000FFFF;
      for (count = 0; count < size; count++)
	{
	  key *= 0x6C078965;
	  tkey = key >> 0x18;
	  buff[count] = (buff[count] & 0x000000FF) ^ (tkey & 0x000000FF);
	}
      break;
    case 2:
      if (!key)
	key = 1;
      for (count = 0; count < size; count++)
	{
	  key = key & 0x0000FFFF;
	  tmanip = ((key % 0x0F) + 1);
	  tkey = key >> (0x10 - tmanip);
	  key += (tkey | (key << tmanip));
	  buff[count] = buff[count] ^ ((char) key);
	}
      break;
    case 3:
      if (!key)
	key = 1;
      for (count = 0; count < size; count++)
	{
	  key = key & 0x0000FFFF;
	  tkey = (((key >> 8) + key) % 0x0F) + 1;
	  key = ((key >> (0x10 - tkey)) | (key << tkey)) + key;
	  buff[count] = buff[count] ^ key;
	}
      break;
    default:
      /* printf("firm_decrypter: Unknown Encryption type: %i\n",type); */
      return 0;
    };
  return 1;
}

size_t
find_pattern_drive (int device, int loc, char *pattern, size_t psize)
{
  size_t bcount, pcount, scount;
  char found;
  mmcdata_s d;

  memset (&d, 0, sizeof (mmcdata_s));
  d.data = calloc (1, SECTOR_SIZE);
  d.datasize = SECTOR_SIZE;
  if (!d.data)
    return 0;

  found = 0;
  for (bcount = 0; bcount < (unsigned int) (0xFFFFFFFF);
       bcount += SECTOR_SIZE - psize)
    {
      if (!lgren_read (device, loc, &d, bcount, d.datasize))
	{
	  free (d.data);
	  return 0;
	}
      for (scount = 0; scount < SECTOR_SIZE; scount++)
	{
	  for (pcount = 0; pcount < psize; pcount++)
	    {
	      if (scount + pcount >= SECTOR_SIZE)
		{
		  found = 0;
		  break;
		}
	      if (pattern[pcount] == d.data[scount + pcount]
		  || pattern[pcount] == (char) 0xFF)
		{
		  found = 1;
		}
	      else
		{
		  found = 0;
		  break;
		}
	    }
	  if (found)
	    {
	      free (d.data);
	      return bcount + scount;
	    }
	}
    }

  free (d.data);
  return 0;
}

char
firm_ripexe (char *filename)
{
  int type, found, firm_start, firm_stop, firm_end, dsize, firm_size, fpos;
  size_t fsize = 0, key;
  char *tbuff2, *ofile, *buff2, *buff;

  fsize = falloc (filename, &buff);
  /* Error messages handled in falloc */
  if (!fsize)
    return 0;

  type = get_enctype (&buff[fsize]);
  if (!type)
    {
      printf ("firm_ripexe: Unknown encryption type\n");
      return 0;
    }

  /* end sections change size depending on the exe, so we have to brute force */
  /* the location and test against a known string. then we will be able to get */
  /* the start position and the stop position of the firmware. */
  found = get_enckeypos (&buff[fsize], type);


  if (found)
    {
      size_t key3 =
	((buff[fsize - found] << 8) & 0x0000FF00) | (buff[fsize - found + 1] &
						     0x000000FF);
      if (!firm_decrypter (&buff[fsize - found + 2], type, key3, found - 2))
	{
	  printf ("firm_ripexe: Decryption failed. Unkown type 1\n");
	  return 0;
	}
    }
  else
    {
      printf ("firm_ripexe: Could not bruteforce size of firmware info\n");
      return 0;
    }

  /* for some reason the data is in MSB form and we have to change it to LSB */
  firm_start = swap32 (*(int *) (&buff[fsize - found + 2]));
  firm_stop = swap32 (*(int *) (&buff[fsize - found + 6]));
  firm_end = (firm_start + firm_stop) - found;

  /* printf("Firm location 0x%08X\n",firm_start); */

  if (!firm_stop)
    {
      printf ("firm_ripexe: firm_stop was 0. firmware must have a size\n");
      return 0;
    }

  /* Decrypt the mangled firmware */
  key =
    ((buff[firm_start] << 8) & 0x0000FF00) | (buff[firm_start + 1] &
					      0x000000FF);
  if (!firm_decrypter (&buff[firm_start + 2], type, key, firm_stop))
    {
      printf ("firm_ripexe: Decryption failed. Unkown type 2\n");
      return 0;
    }

  dsize = firm_demangle (buff, 0, firm_start, firm_stop, firm_end);
  if (!dsize)
    {
      printf ("firm_ripexe: Unabled to calculate demangled firmware size\n");
      return 0;
    }

  buff2 = (char *) malloc (dsize);
  if (!buff2)
    {
      printf ("firm_ripexe: Failed to allocate memory 1\n");
      return 0;
    }

  memset (buff2, 0, dsize);
  if (!firm_demangle (buff, buff2, firm_start, firm_stop, firm_end))
    {
      printf ("firm_ripexe: Failed to demangle firmware\n");
      return 0;
    }

  firm_size = 0;
  fpos = 0;
  tbuff2 = 0;
  do
    {
      firm_size = swap32 (*(int *) (&buff2[fpos + 0x44]));
      if (!firm_size)
	{
	  break;
	}
      fpos += firm_size + 0x400;

      if (fpos > dsize)
	{
	  tbuff2 = (char *) realloc (buff2, fpos);
	  if (!tbuff2)
	    {
	      free (buff);
	      printf ("firm_ripexe: Failed to reallocate memory 2\n");
	      return 0;
	    }
	  else
	    buff2 = tbuff2;
	  memset (&buff2[dsize], 0, fpos - dsize);
	}
    }
  while (firm_size && fpos < dsize);

  ofile = (char *) calloc (1, strlen (filename) + 5);
  strcat (ofile, filename);
  strcat (ofile, ".bin");
  if (!firm_spliter (buff2, dsize, ofile))
    {
      printf ("firm_ripexe: Failed to split firmware\n");
      free (ofile);
      return 0;
    }
  free (ofile);
  return 1;
}
