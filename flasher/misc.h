#ifndef MISC_H
#define MISC_H
#include <stdio.h>
#include <libmmc/libmmc.h>

void __sleep(int delay);
long swap32(long toswap);
short swap16(short toswap);
size_t find_pattern_buff(char* buff,char* pattern,size_t bsize,size_t psize);
int asciihex2int(char* buff);
size_t get_filesize(FILE* fileh);
size_t falloc(char* filename,char** inbuff);
char cmp_buff(char* buff1,char* buff2,size_t size);
void printd(char* buff,int offset,int buff_start,int buff_end);
#endif
