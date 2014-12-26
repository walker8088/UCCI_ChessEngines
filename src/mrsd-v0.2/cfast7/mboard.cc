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
#include "mboard.h"
#include <stdlib.h>
int cch_mginit=0;
U96 line[90][90];
U96 MoveArray[14][90];
int XQ::
TypeRule(int type,int sy, int sx, int dy, int dx)
{
  int  lx, ly;
  char cc;
  lx = dx - sx;
  ly = dy - sy;
  if (!lx && !ly)
	 return 0;
	 cc=0;
  switch (type)
	 {
	 case Knight:
		  if ((abs (lx) + abs (ly)) != 3)	return 0;
		if (!lx || !ly)           	return 0;
		break;
	 case Minister:
		if (abs (lx) != 2) 	return 0;
		if (abs (ly) != 2) 	return 0;
		if (!cc && dy > 4) 	return 0;
		if (cc && dy < 5)  	return 0;
			if (!cc && sy > 4) 	return 0;
		if (cc && sy < 5)  	return 0;
		break;
	 case Guard:
		if (abs (lx) != 1)	 return 0;
		if (abs (ly) != 1)  return 0;
		if (!cc && dy > 2) return 0;
		if (cc && dy < 7)   return 0;
		if (!cc && sy > 2) return 0;
		if (cc && sy < 7)   return 0;
		if(sx <3 || sx >5) return 0;
		if (dx < 3 || dx > 5)   return 0;
		break;
	 case Pawn:
		if ((abs (lx) + abs (ly)) != 1)   	return 0;

	  if(ly < 0)    return 0;
if(!ly) if( sy <= 4)  return 0;
		break;
	 case Rook:
	 case Cannon:
		if(sx!=dx && sy!=dy) return 0;
		break;
	 case King:
		if (dy>6)
	{
		 if(sx!=dx) return 0;
	}
		else
	{
	  if ((abs (lx) + abs (ly)) != 1)
		 return 0;
	  if (!cc && dy > 2)
		 return 0;
	  if (dx < 3 || dx > 5)
		 return 0;
	}
		break;
	 default:
		return 0;
	 }
  return 1;
}
XQAI::XQAI():XQ()
{
int t,x1,y1,x2,y2,i,j,k;
//int dummy;
YX s1,s0;
if(!cch_mginit)
{
for(t=0;t<7;t++)
for(y1=0;y1<10;y1++)
  for(x1=0;x1<9;x1++)
  {  s1=s0=YX_Set(y1,x1);
         MoveArray[2*t][s0].clear();
         MoveArray[2*t+1][YX_T(s0)].clear();
	 for(y2=0;y2<10;y2++)
		for(x2=0;x2<9;x2++)
		{
		if(TypeRule(t,y1,x1,y2,x2))
		  {  
              cch_mg[(t<<1)|1][YX_T(s0)][YX_T(s1)]=YX_T(YX_Set(y2,x2));
		s1=cch_mg[t<<1][s0][s1]=YX_Set(y2,x2);
                MoveArray[2*t][s0].setbit(s1);
                MoveArray[2*t+1][YX_T(s0)].setbit(YX_T(s1));
				}
				}
		  YX_kill(cch_mg[t<<1][s0][s1]);
                YX_kill(cch_mg[(t<<1)|1][YX_T(s0)][YX_T(s1)]);
	}
	cch_mginit=1;
	//bitboard
        bmap.clear();
#define piece(y,x) bmap.setbit(YX_Set(y,x))
#define piece2(p,y,x) piece(y,x);piece(9-y,x)
#define piece4(p,y,x) piece2(p,y,x);piece2(p,y,8-x);
	 piece2 (king, 0, 4);
  	 piece4 (guard1, 0, 3);
  	 piece4 (minister1, 0, 2);
  	 piece4 (knight1, 0, 1);
  	 piece4 (rook1, 0, 0);
  	 piece4 (cannon1, 2, 1);
  	 piece2 (pawn1, 3, 4);
  	 piece4 (pawn2, 3, 2);
  	 piece4 (pawn4, 3, 0);
	for(i=0;i<90;i++)
		for(j=0;j<90;j++)
		{
			line[i][j].clear();
			if(i/9==j/9)
			{
				for(k=i+1;k<=j-1;k++)
					line[i][j].setbit(k);
				for(k=j+1;k<=i-1;k++)
					line[i][j].setbit(k);

			}
			else if(i%9==j%9)
			{
				for(k=i+9;k<=j-9;k+=9)
					line[i][j].setbit(k);
				for(k=j+9;k<=i-9;k+=9)
					line[i][j].setbit(k);
			}
			else	line[i][j].set(0xffffffff,0xffffffff,0xffffffff);
					
		}

 }
/*for(i=0;i<90;i++)
for(j=0;j<90;j++)
	line[i][j].show();*/
//xptr[0]=0;
//for(j=0;j<NLEVEL;j++)
//for(i=0;i<cch_ngm_size;i++) cch_ngm_data[j][i]=0;
ui=(unsigned int *) &cch_p[2];
cch_value[0]=0;
ValueInit();
hValueInit();
hvalue[0]=0;
}
XQAI::XQAI(XQ &bd):XQ(bd)
{
ValueInit();
ui=(unsigned int *)&cch_p[2];
hValueInit();
hvalue[0]=0;
}
XQAI::~XQAI()
{
}

int XQAI::Move()
{
if(!XQ::Move()) return 0;
cch_value[_Ply]=cch_value[_Ply-1];
hvalue[_Ply]=hvalue[_Ply-1];
return 1;
}
