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
int cmd_ldrive(void* Input);
int cmd_verifyfirm(void* Input);
int cmd_getdrives(void* Input);
int cmd_lgetdrives(void* Input);
int cmd_checksum(void* Input);
#endif
