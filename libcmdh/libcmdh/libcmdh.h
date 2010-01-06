#ifndef CMDH_H
#define CMDH_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INDIR_ONLY 1
#define OUTDIR_ONLY 2
#define ALL_DIRS 3


typedef struct argcv{
  unsigned int argc;
  char** argv;
}argcv;

typedef struct CommandDB{
  char*     cmdPrimary;     //store commands like --help
  char*     cmdSecondary;   //stores commands like -h
  int       numParams;      //number of params
  int       ParamPos;     //param possition start
  int       cmdPriority;    //cmd pririty
  int       cmdGroup;     //cmd group
  char      cmdEnabled;     //cmd Enabled
  void *      cmdRef;       //Reference Address to function for command
}CommandDB;

#ifdef __cplusplus
extern "C" {
#endif
  char  InitializeCommandHandler(int argc,char *argv[]);
  char  AddCommand(const char * cmdPrimary,const char * cmdSecondary,int cmdGroup,int cmdPriority,int numParams,void * cmdRef);
  char  CompareCommands();
  char  RunCommand(int cmdGroup);
  char* GetExecName();
  char* GetFirstFile();
  char* GetNextFile();
  void  SetDirectories(int Dirs);
  char* GetInDirecotry();
  char* GetOutDirecotry();
  void  Param_1_Enabled();
  void ClearCommandHandler();

  int  BindParams();
  int  GetParamReq();
  int  GetMaxPriority();
  char CreateDBs();
#ifdef  __cplusplus
}
#endif //__cplusplus

#endif //CMDH_H
