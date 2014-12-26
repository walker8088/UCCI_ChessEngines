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
static short KingPawnPositionValue[90]={
/*
  0,  1,  2,  3,  4,  5,  6,  7,  8  */
  0,  0,  0, 10, 50, 10,  0,  0,  0, 
  0,  0,  0,-80,-80,-80,  0,  0,  0, 
  0,  0,  0,-90,-90,-90,  0,  0,  0,
-20,  0,-20,  0, 60,  0,-20,  0,-20,
 30,  0, 40,  0, 70,  0, 40,  0, 30,
100,180,220,350,400,350,220,180,100,
200,270,300,400,420,400,350,270,200, 
200,300,450,550,550,550,450,300,200,
200,300,500,650,700,650,500,300,200,
  0, 0,   0, 20, 40, 20,  0,  0,  0
};
static short GuardMinisterPositionValue[90]={
/*
  0,  1,  2,  3,  4,  5,  6,  7,  8  */
  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  0,  0,  0,  0, 30,  0,  0,  0,  0,
-20,  0,  0,  0, 30,  0,  0,  0,-20,
  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  0,  0,  0,  0,  0,  0,  0,  0,  0, 
  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0
};

static short KnightPositionValue[90]={
/*
  0,  1,  2,  3,  4,  5,  6,  7,  8  */
  0,-30, 20,  0, 20,  0, 20,-30,  0, 
-30, 20, 40, 50,-100,50, 40, 20,-30, 
 50, 40, 60, 70, 40, 70, 60, 40, 50, 
 40, 60,100, 70,100, 70,100, 60, 40, 
 20,100,130,140,150,140,130,100, 20, 
 20,120,110,150,160,150,110,120, 20, 
 50,200,120,190,120,190,120,200, 50, 
 40,100,110,150,110,150,110,100, 40, 
 20, 80,150, 90, 60, 90,150, 80, 20, 
 20, 20, 20, 80, 20, 80, 20, 20, 20
};
static short CannonPositionValue[90]={
/*
  0,  1,  2,  3,  4,  5,  6,  7,  8  */
  0,  0, 10, 30, 30, 30, 10,  0,  0, 
  0, 10, 20, 20, 20, 20, 20, 10,  0, 
 10,  0, 40, 30, 50, 30, 40,  0, 10, 
  0,  0,  0,  0,  0,  0,  0,  0,  0, 
-10,  0, 30,  0, 40,  0, 30,  0,-10, 
  0,  0,  0,  0, 40,  0,  0,  0,  0, 
  0, 30, 30, 20, 40, 20, 30, 30,  0, 
 10, 10,  0,-50,-40,-50,  0, 10, 10, 
 20, 20,  0,-40,-70,-40,  0, 20, 20, 
 40, 40,  0,-50,-60,-50,  0, 40, 40
};
static short RookPositionValue[90]={
/*
  0,  1,  2,  3,  4,  5,  6,  7,  8  */
-60, 60, 40,120,  0,120, 40, 60,-60,
 50, 80, 60,120,  0,120, 60, 80, 50,
-20, 80, 40,120,120,120, 40, 80,-20,
 40, 90, 40,120,140,120, 40, 90, 40,
 80,120,120,140,150,140,120,120, 80,
 80,110,110,140,150,140,110,110, 80,
 60,130,130,160,160,160,130,130, 60,
 60, 80, 70,140,160,140, 70, 80, 60,
 60,120, 90,160,330,160, 90,120, 60,
 60, 80, 70,130,140,130, 70, 80, 60
};
int cch_value[512];
static short *PositionValue[7]={
KingPawnPositionValue,GuardMinisterPositionValue,
GuardMinisterPositionValue,KnightPositionValue,
RookPositionValue,CannonPositionValue,
KingPawnPositionValue
};
static int EvaluationTablesAreSet=0;
int MaterialValue[7]={50000,400,400,880,2000,960,90};
int XQAI::MaterialPositionValue[34][90];
void XQAI::ValueInit()
{
int t,y,x,i;
if(EvaluationTablesAreSet) return;
for(i=2;i<34;i+=2)
{
t=id2type(i);
for(y=0;y<10;y++)
for(x=0;x<9;x++)
{
MaterialPositionValue[i][(YX_Set(y,x))]=
	(PositionValue[t][(YX_Set(y,x))]+MaterialValue[t])/5;
MaterialPositionValue[i+1][YX_T(YX_Set(y,x))]=
	-(PositionValue[t][(YX_Set(y,x))]+MaterialValue[t])/5;
}
}
EvaluationTablesAreSet=1;
/*FILE *f=fopen("idv","rb");
short s[350];
fread(s,1,700,f);
fclose(f);
	for(int i=2;i<34;i+=2)
	{
		int t=id2type(i);
		for(int y=0;y<10;y++)
		for(int x=0;x<5;x++)
		{
		MaterialPositionValue[i][(YX_Set(y,x))]=
		MaterialPositionValue[i][(YX_Set(y,8-x))]=s[t*50+y*5+x];
		MaterialPositionValue[i+1][YX_T(YX_Set(y,x))]=
		MaterialPositionValue[i+1][YX_T(YX_Set(y,8-x))]=-s[t*50+y*5+x];
		}
	}*/
}
#if 1 
int XQAI::
fMove (YX yx1,YX yx2)
{
  register char s , d;
  s=_bd(tr[_Ply].src=yx1);
  tr[_Ply].eat=d=_bd(cch_p[s]=tr[_Ply].dst=yx2);
  _bd(yx2) = s;
  _bd(yx1) = 0;
  _MaxPly = ++_Ply;
  YX_kill(cch_p[d]);
  bmap.clearbit(yx1);
cch_value[_Ply]=cch_value[_Ply-1]+MaterialPositionValue[s][yx2]
	-MaterialPositionValue[s][yx1]-MaterialPositionValue[d][yx2];
  return 1;
}
#endif
/*flags bit0 RuleÀË¬d
 *      bit1 HASH 
 */
int XQAI::Move(YX syx,YX dyx,int flags)
{
register char s,d;
if(flags&1) {
	s=_bd(syx);
	d=_bd(dyx);
	if(!XQ::Move(syx,dyx,1)) return 0;
}
else {
  s=_bd(tr[_Ply].src=syx);
  tr[_Ply].eat =d=_bd(cch_p[s]=tr[_Ply].dst=dyx);
  _bd(dyx) = s; 
  _bd(syx) = 0;
  _MaxPly = ++_Ply;
}
if(d) YX_kill(cch_p[d]);
else bmap.setbit(dyx);
bmap.clearbit(syx);
cch_value[_Ply]=cch_value[_Ply-1]-Value(s,syx)+Value(s,dyx)
                         -Value(d,dyx);
if(flags&2)
hvalue[_Ply]=hvalue[_Ply-1]^hValue(s,syx)^hValue(s,dyx)^hValue(d,dyx);

return 1;
}
