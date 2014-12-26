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
#include <stdlib.h>
#include "mboard.h"
ull pos_hval[7][2][90];
static int hvalueIsSet=0;
void XQAI::hValueInit()
{
int t,y,x,c;
if(hvalueIsSet) return;
for(t=0;t<7;t++)
for(c=0;c<2;c++)	
for(y=0;y<10;y++)
for(x=0;x<9;x++)
pos_hval[t][c][YX_Set(y,x)]=
rand() ^ ((ull)rand() << 15) ^ ((ull)rand() << 30) ^
        ((ull)rand() << 45) ^ ((ull)rand() << 60);
hvalueIsSet=1;
}
