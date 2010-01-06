#ifdef WIN32
#ifdef GTK
//#include <windows.h>
#pragma comment(lib, "gtk-win32-2.0.lib") //gtk
#pragma comment(lib, "gdk-win32-2.0.lib") //gdk
#pragma comment(lib, "gio-2.0.lib") //gio
//#pragma comment(lib, "gdk_pixbuf-2.0.lib") //gdk-pixbuf
//#pragma comment(lib, "pango-1.0.lib") //pango
#pragma comment(lib, "gobject-2.0.lib") //gobject
//#pragma comment(lib, "gmodule-2.0.lib") //gmodule
//#pragma comment(lib, "glib-2.0.lib") //glib
//#pragma comment(lib, "gthread-2.0.lib") //gthread

//keeping these just incase i want to use them later instead
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
#include <gtk/gtk.h>
#include <iostream>
using namespace std;


//using this so i dont have to include windows.h
#define HINSTANCE void*
#define LPSTR char*
#define WINAPI __stdcall

//define the needed function proto's
void ConCMDL2STD(int * argc, int * argv,char* strin);
extern int main(int argc,char* argv[]);


//the windows main entry point needed for windows programs that are running in GUI mode
//this is for windows visual studio programs
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
  //setup the needed variavles and they will not get destroyed until the function returns
  //and in windows that means the program is dead anyway so no worries
  int argc = 0;
  int* argv = 0; //char *argc[] is really just a char** and
  //a char** really is just a int* that contains pointers to other string arrays

  //now we pass the pointers of the arguments so that the function can edit its content as it pleases
  //and so we dont have to create a struct for something as simple as this
  //then we pass the command line string buffer to the function
  ConCMDL2STD(&argc,(int*)&argv,lpCmdLine);

  //now that we have everything we need to make this work like it used to we call on the main function
  //also we will take the return value of the main function exit code and return it on the exit of ours
  return main(argc,(char**)argv);

}


//this converts the windows char* GetCommandLine(VOID) string into the standard c++ console input format
void ConCMDL2STD(int * argc, int * argv,char* strin)
{
  int strsize = (int)strlen(strin); //get and store string size into strsize
  int* param_db = (int*) calloc(1,sizeof(int)); //setup the initial command array
  int param_tmp = 0; //param_tmp is a temporary storage for the previous loops param_num value
  int param_num = 0; //param_num stores the number of parameters that
  //are passed to the program including the program its self as one

  for(int i = 0; i < strsize;i++) //this loop will keep running until the end of the string
  {
    if(strin[i] != 0) //check to see if the current possition is filled with a null byte
    {
      if(!i) //this checks to see if the i is 0 if it is then run code
      {
        //check to see if the parameter starts with " because windows apps can
        if(strin[i] == '"')
        {
          strin[i++] = 0; //place a null byte into current possition and increase the i value by 1
          param_db[param_num] = (int)&strin[i]; //update the parameters array with current string pointer

          for(; i < strsize;i++) //this for will loop until the end of string or until it finds another "
          {
            if(strin[i] == '"') //if we found a " run code
            {
              strin[i] = 0; //place null byte into current string possition
              break; //break out of this current for loop
            }
          }
        }
        else //if the string did not start with a " this just update the parameters array
        {
          param_db[param_num] = (int)&strin[i]; //update the parameters array with current string pointer
        }
        param_num++; //update the parameters counter
      }

      else //we are not on the first pass run this code
      {
        if(strin[i] == '"') //we check to see if this parameter starts with "
        {
          strin[i++] = 0; //place a null byte into current possition and increase the i value by 1
          param_db[param_num] = (int)&strin[i];
          param_num++; //update the parameters counter

          for(; i < strsize;i++) //this for will loop until the end of string or until it finds another "
          {
            if(strin[i] == '"') //if we found a " run code
            {
              strin[i] = 0; //place null byte into current string possition
              break; //break out of this current for loop
            }
          }
        }
        else if(strin[i] == ' ') //if there was no " then we test for a space symbol
        {
          strin[i++] = 0; //place a null byte into current possition and increase the i value by 1
          if(strin[i] != '"') //we need to make sure that this possition is not a " since it could be
          {
            param_db[param_num] = (int)&strin[i]; //update the parameters array with current string pointer
            param_num++; //update the parameters counter
          }
          else i--; //if it did turn out to be a " then we need to subtract 1 from i so that the code works right on the loop back

        }
      }

      //thos code checks to see if there param_num is greater then one other wise we dont need to allocate more memory
      //btw there is always at least one since the program is included
      //then we check to see if there number of parameters changed since last loop
      if(param_tmp != param_num && param_num > 1)
      {
        param_tmp = param_num;
        param_db = (int*) realloc(param_db,(param_num+1)*sizeof(int)); //allocate new space for possible more parameters
        if(!param_db) //if we failed to allocat memory then its a bust
        {

        }
      }
    }
  }
  *argc = param_num; //set the number of parameters to the argc which will affect the winmains argc also
  *argv = (int)param_db; //update the argv pointer to point to our array and this will affect the winmains argv also
}


#endif //GTK
#endif //WIN32
