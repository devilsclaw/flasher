This file is part of flasher.

This project aims to create a generic cross-platform (CD/DVD)-ROM drive firmware dumping and flashing software.

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


WARNING: Flashing drives with any software can potentially kill your drive for various reasons.
Use my tool and any other tools at your own risk. This software was created by me for personal
use but I figured others might have a use for it.

### WINDOWS AND MAC OS X USAGE:
Returns the drives name and id:

```
flasher -D
AVAILABLE DRIVES AND IDs:
Drive ID: 1 Name: DVDRAM GMA-4020B
Drive ID: 2 Name: DVD-ROM GDR8164B
```

Subsequent commands need to have the drive specified by "Drive ID", for example
```
flasher -d 1 -m main_firmware.bin
flasher -d 1 -c core_firmware.bin
etc.
```


### LINUX USAGE:
You have to be logged in as root or use sudo to run flasher.

Linux version uses the /dev/ path and the hardware node, e.g.
```/dev/cdrom
/dev/hda
/dev/sda
/dev/sr0
etc.
```

`flasher -D` is not supported on Linux. Commands need to have the drive specified with `-d [Drive path]`, such as
`flasher -d /dev/cdrom -m main_fw.bin`


### More examples

Dump 1kB of raw data at 0x400000 from drive memory (LOC_MEMORY=5, see enum loc_e in lg_renesas.h)
`flasher -d [Drive ID] -l rawdump.bin 5 0x400000 0x400`

Flashes drive with new firmware:
`flasher -d [Drive ID] -f firmware.bin`

Verify firmware on drive from file (might require rebooting before it works):
`flasher -d [Drive ID] -V firmware.bin`

Rips the firmware files out of the windows update executable:
`flasher -r firmware.exe`

ONLY USE THIS IF YOU KNOW WHAT YOUR DOING AND ITS THE LAST OPTION:
Patches a firmware file to contain a correct checksum value:
`flasher --checksum firmware_to_patch.bin`

