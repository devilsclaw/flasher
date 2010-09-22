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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc,char** argv)
{
    time_t     curtime;
    struct tm  *lt;
    char       buf[60];
    FILE* hfile;

    curtime = time(NULL);
    lt = localtime(&curtime);
    strftime(buf, sizeof(buf), "#define STR_VERSION \"BUILD %m%d%Y_%I%M%p\"\n", lt);
    hfile = fopen(argv[1],"w+");
    fwrite(buf,1,strlen(buf),hfile);
    fclose(hfile);
    return 0;
}

