/*
Ki11egg , Xiangqi(Chinese Chess) engine
Copyright (C) Tzer-jen Wei

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef CBDB_H
#define CBDB_H
#ifndef uchar
#define uchar unsigned char
#endif
extern "C"{
#include "mydb.h"
}
#include <string.h>
#include <stdlib.h>
#include "cbfile.h"
class ImageNode
{
public:
unsigned int GoodMove;
short CacheMove;
short CacheScore;
short CacheDepth;
int Result[3];
int Games;
void Clean(){Games=0;Result[0]=Result[1]=Result[2]=0;}
ImageNode(){Clean();}
int Add(int r,YX s,YX d);
};

class XQDB
{
GDBM_FILE  file;
int img2key(YX *img,uchar *key,int color);
int Normalize(YX *img);
public:
XQDB(char *name="open");
int SaveNode(uchar * key,ImageNode &p);
int GetNode(uchar * key,ImageNode &p);
int Add(YX *img,int color,int Result,YX next_src,YX next_dst) ;
int Add(XQ *b,int r,int Ply=40);
int SetNode(XQ *b,ImageNode &p);
int FindNode(YX *img,int color,ImageNode &p);
int FindNode(XQ *,ImageNode &p);
int Cut(char *);
void sync(){ my_sync(file);}
~XQDB();
};
#endif
