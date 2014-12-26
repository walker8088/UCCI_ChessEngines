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
#ifndef BD_H
#include "mboard.h"
#include <stdio.h>
class BD{
public:
#ifdef FullInformation
unsigned int img[8];
#else
ull signiture;
#endif
short ub,lb;
signed char color;char sdepth;ushort move;
BD(){color=-1;move=0;ub=20000;lb=-20000;};
void Set(XQAI *B,int a=0,int b=0,int s=0,int s2=0,ushort mv=0);
BD(XQAI *B,int a=0,int b=0,int s=0,int s2=0,ushort mv=0)
{Set(B,a,b,s,s2,mv);}

void Set(int a=0,int b=0,int s=0,int v=0,ushort mv=0)
{
if(s<0) {color=-1;lb=-20000;ub=20000;return;}
if(v>a && v<b) lb=ub=v;
else if(v<=a) {ub=v;lb=-20000;}
else if(v>=b) {lb=v;ub=20000;}
sdepth=s;
move=mv;
}
bool operator==(XQAI &mb)
#ifndef FullInformation
{return color==mb.Turn() && signiture==mb.hValue();}
#endif
;
int Accept(int &a,int &b,int &s);
void Clean(){color=-1;lb=-20000;ub=20000;}
};
BD *Get_HASH(ull sig);
#endif
