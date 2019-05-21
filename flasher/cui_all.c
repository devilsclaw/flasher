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

#ifdef CUI
#include <stdlib.h>
#include "cmds_all.h"
#include <libcmdh.h>

int
main (int argc, char **argv)
{
  InitializeCommandHandler (argc, argv);

  /* Add commands to the command DB */
  /* Short, Long, Group, Priority, Parameters, function pointer */
  AddCommand ("-v", "--version", 0, 0, 0, (void *) &cmd_version);
  AddCommand ("-h", "--help", 1, 0, 0, (void *) &cmd_help);
#if !defined(__linux__)
  AddCommand ("-D", "--drives", 2, 0, 0, (void *) &cmd_getdrives);
  AddCommand ("-d", "--drive", 3, 0, 1, (void *) &cmd_drive);
#else
  AddCommand ("-D", "--drives", 2, 0, 0, (void *) &cmd_lgetdrives);
  AddCommand ("-d", "--drive", 3, 0, 1, (void *) &cmd_ldrive);
#endif
  AddCommand ("-m", "--main", 4, 0, 1, (void *) &cmd_dumpmain);
  AddCommand ("-c", "--core", 4, 1, 1, (void *) &cmd_dumpcore);
  AddCommand ("-l", "--dumploc", 4, 2, 4, (void *) &cmd_dumploc);
  AddCommand ("-f", "--flash", 5, 0, 1, (void *) &cmd_flashfirm);
  AddCommand ("-V", "--ver_firm", 5, 1, 1, (void *) &cmd_verifyfirm);
  AddCommand ("-r", "--rip_exe", 6, 0, 1, (void *) &cmd_ripexe);
  AddCommand (0, "--checksum", 7, 0, 1, (void *) &cmd_checksum);
  AddCommand ("-n", "--nologo", 10, 0, 0, 0);	/* dont show my header */

  if (CompareCommands ())
    {
      /* it does not matter if these pass or fail so not testing */
      if (!RunCommand (10))
	printf ("%s\n", STR_NAME);
      RunCommand (0);
      if (RunCommand (1))
	exit (0);
      if (RunCommand (2))
	exit (0);
      if (RunCommand (7))
	exit (0);

      if (RunCommand (3))
	{
	  if (RunCommand (4))
	    exit (0);
	  if (RunCommand (5))
	    exit (0);
	  if (RunCommand (6))
	    exit (0);
	  exit (0);
	}
      else if (!RunCommand (6))
	exit (1);
      exit (0);
    }

  return 0;
}

#endif /* CUI */
