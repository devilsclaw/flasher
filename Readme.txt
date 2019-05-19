Note by Chris Kobayashi (20190519):

This fork of "flasher" is 64-bit clean on Linux.  No attempt was made to
validate it on either Windows or MacOS.

Original README:

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

WINDOWS AND MAX OS X USAGE:
Returns the drives name and id:
flasher -D

Dumps drives firmwares:
flasher -d [Drive ID] -m main_firmware.bin
flasher -d [Drive ID] -c core_firmware.bin

Flashes drive with new firmware:
flasher -d [Drive ID] -f firmware.bin

Verify firmware on drive from file (might require rebooting before it works):
flasher -d [Drive ID] -V firmware.bin

Rips the firmware files out of the windows update executable:
flasher -r firmware.exe

flasher -D
AVAILABLE DRIVES AND IDs:
Drive ID: 1 Name: DVDRAM GMA-4020B
Drive ID: 2 Name: DVD-ROM GDR8164B

If I want to dump the DVDRAM GMA-4020B drives firmware it would look like this:
flasher -d 1 -m main_firmware.bin
flasher -d 1 -c core_firmware.bin

If i want to flash the DVDRAM GMA-4020B drive with the main_firmware.bin:
flasher -d 1 -f main_firmware.bin

ONLY USE THIS IF YOU KNOW WHAT YOUR DOING AND ITS THE LAST OPTION:
Patches a firmware file to contain a correct checksum value:
flasher --checksum firmware_to_patch.bin


LINUX USAGE:
You have to be logged in as root or use sudo to run flasher.

Linux version uses the /dev/ path and the hardware node
eg.
/dev/cdrom
/dev/cdrw
/dev/hda
/dev/sda

Returns a message saying its not supported on Linux and to use -d:
flasher -D

Dumps drives firmwares:
flasher -d [Dev path] -m main_firmware.bin
flasher -d [Dev path] -c core_firmware.bin

Flashes drive with new firmware:
flasher -d [Dev path] -f firmware.bin

Verify firmware on drive from file (might require rebooting before it works):
flasher -d [Dev path] -V firmware.bin

Rips the firmware files out of the windows update executable
flasher -r firmware.exe

If I want to dump a drives firmware it would look like this:
flasher -d /dev/cdrom -m main_firmware.bin
flasher -d /dev/cdrom -c core_firmware.bin

If i want to flash a drive with the main_firmware.bin:
flasher -d /dev/cdrom -f main_firmware.bin

ONLY USE THIS IF YOU KNOW WHAT YOUR DOING AND ITS THE LAST OPTION:
Patches a firmware file to contain a correct checksum value:
flasher --checksum firmware_to_patch.bin
