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

#ifndef CBFILE_H
#define CBFILE_H
#include "board.h"
#include <stdio.h>
#include <string.h>
#define CF_LINEMAX  1024
#define KEYS 9 
#define SKEYS 3
class XQFile
{
static char *KeyEng[];
static char *KeyChn[];
FILE *file;
char KeyWord[KEYS][256];
void PrintComment(int );
int MaxRemark;
char *Comment[1500];
char FileName[256];
char NONE[1];
public:
void CleanComment(int start=0);
int Result;
XQFile(char *FN=NULL);
XQFile(int n);
int Save(XQ &B,int cnt=1,int style=0);
int Load(XQ &B,int cnt=1);
void SetFileName(const char *a);
void SetFileName(const int n);
char *GetFileName();

void SetComment(char *,int n);
void AddComment(char *,int);
char *LoadComment(int n);
char *GetKeyWord(int n);
char *GetKeyWord(const char *kname);
};
#endif

