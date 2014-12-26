#include "mydb.h"
#include <stdlib.h>
#include <stdio.h>
int hseed[33]={2213717,4424134,1417452,6754133,8977143,5589417,4793167,7771436,
               4914274,6256747,7097111,6541323,8484561,8390239,1744147,1261732,
               924876,3095417,4017453,5091142,6087645,7009876,8041746,9076765,
               731424,967545,2998096,7576545,6577419,5488747,9293393,8311727,
               555555}; 
static FILE *fhash,*fdata;
//fix length=12
#define IL 4
//#define IL 8 
static int file_end_offset(FILE *f)
{
fseek(f,0,SEEK_END);
return ftell(f);
}
static int key2hashkey(uchar *key)
{
int i,res;
res=0;
for(i=0;i<KL;i++)
  res+=key[i]*hseed[i];
  return res&0xffff;
}
GDBM_FILE my_connect(GDBM_FILE ms,char *name)
{
int i,res=-1;
char hash_fname[256],data_fname[256];
sprintf(hash_fname,"%s.hash",name);
sprintf(data_fname,"%s.data",name);
fhash=fopen(hash_fname,"a+");
fdata=fopen(data_fname,"a+");
fclose(fhash);fclose(fdata);
fhash=fopen(hash_fname,"rb+");
if(!file_end_offset(fhash))
{fprintf(stderr,"creating hash file\n");
 for(i=0;i<0x10000;i++) fwrite(&res,4,1,fhash);
}
fdata=fopen(data_fname,"rb+");
return 1;
}
static int my_length;
static int my_last;
static int my_next;
static int hashkey;
static int my_search(uchar *key)
{
uchar buf[60];
int fdata_size,i;
int my_offset;

fdata_size=file_end_offset(fdata);

hashkey=key2hashkey(key);
fseek(fhash,hashkey*4,SEEK_SET);
fread(&my_offset,4,1,fhash);

my_last=my_offset;

while(my_offset!=-1)
{
if(my_offset<0 ||my_offset > fdata_size-KL-DL) 
	{fprintf(stderr,"key offset %d %d\n",my_offset,fdata_size);exit(1);}

fseek(fdata,my_offset,SEEK_SET);
if(fread(buf,1,KL+IL,fdata)==KL+IL)
{
if(!memcmp(buf,key,KL))
  {
//    my_length=*(int *)(&buf[KL+4]);
    my_length=12;
    my_next=*(int *)(&buf[KL]);
      return my_offset;
  }
}
else {fprintf(stderr,"read error %d %d\n",hashkey,my_offset);exit(1);}
 my_last=my_offset; 
 my_offset=*(int *)(&buf[KL]); 
}
my_next=-1;
return -1;  
}
int my_fetch(GDBM_FILE ms,uchar *key,uchar *data,int *length)
{
int my_offset,i;

my_offset=my_search(key);
if(my_offset>=0)
{  
  *length=my_length;
  fseek(fdata,my_offset+IL+KL,SEEK_SET);
  fread(data,1,*length,fdata);
  return 1;
  }
return 0;
}
int my_store(GDBM_FILE ms,uchar *key,uchar *data,int length)
{
int my_offset,i;
if(length>DL) {fprintf(stderr,"DL\n");exit(1);}
my_offset=my_search(key);
if(my_offset<0)
{
	my_offset=file_end_offset(fdata);
	if(my_last!=-1)
	{
	if(my_last%(KL+IL+DL))fprintf(stderr,"my_last error\n");
	fseek(fdata,my_last+KL,SEEK_SET);
	fwrite(&my_offset,4,1,fdata);
	}
	else
	{
	fseek(fhash,4*hashkey,SEEK_SET);
	fwrite(&my_offset,4,1,fhash);
	}

}
	if(my_offset%(KL+IL+DL))
	fprintf(stderr,"my_offset error %d\n",my_offset);
fseek(fdata,my_offset,SEEK_SET);
fwrite(key,1,KL,fdata);
fwrite(&my_next,4,1,fdata);
//fwrite(&length,4,1,fdata);
for(i=length;i<DL;i++) data[i]=0;
fwrite(data,1,DL,fdata);
return 1;
}
void my_close(GDBM_FILE ms)
{
fclose(fdata);
fclose(fhash);
}

void my_sync(GDBM_FILE ms)
{
}

