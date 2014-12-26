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
#include "bd.h"
#ifdef FullInformation	
bool BD::operator==(XQAI &mb)
{
#define EQU(n) img[n]==mb.ui[n]
return color==mb.Turn() && EQU(4) && EQU(3) 
      && EQU(5) && EQU(2) && EQU(1) && EQU(0)
      && EQU(6) && EQU(7);
}
#endif
void BD::Set(XQAI *B,int a,int b,int s,int v,ushort mv)
{
lb=-20000;
ub=20000;
#ifdef FullInformation
B->SetUImage(img);
#else
signiture=B->hValue();
#endif
color=B->Turn();
Set(a,b,s,v,mv);
}
