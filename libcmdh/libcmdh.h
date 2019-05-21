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

#ifndef CMDH_H
#define CMDH_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INDIR_ONLY 1
#define OUTDIR_ONLY 2
#define ALL_DIRS 3


typedef struct argcv
{
  unsigned int argc;
  char **argv;
} argcv;

typedef struct CommandDB
{
  char *cmdPrimary;		/* store commands like --help */
  char *cmdSecondary;		/* stores commands like -h */
  int numParams;		/* number of params */
  int ParamPos;			/* param possition start */
  int cmdPriority;		/* cmd pririty */
  int cmdGroup;			/* cmd group */
  char cmdEnabled;		/* cmd Enabled */
  void *cmdRef;			/* Reference Address to function for command */
} CommandDB;

#ifdef __cplusplus
extern "C"
{
#endif
  char InitializeCommandHandler (int argc, char *argv[]);
  char AddCommand (const char *cmdPrimary, const char *cmdSecondary,
		   int cmdGroup, int cmdPriority, int numParams,
		   void *cmdRef);
  char CompareCommands ();
  char RunCommand (int cmdGroup);
  char *GetExecName ();
  char *GetFirstFile ();
  char *GetNextFile ();
  void SetDirectories (int Dirs);
  char *GetInDirecotry ();
  char *GetOutDirecotry ();
  void Param_1_Enabled ();
  void ClearCommandHandler ();

  int BindParams ();
  int GetParamReq ();
  int GetMaxPriority ();
  char CreateDBs ();
#ifdef  __cplusplus
}
#endif				/* __cplusplus */

#endif				/* CMDH_H */
