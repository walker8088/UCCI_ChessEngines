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
#include <stdio.h>
#include <stdlib.h>
#ifdef VC
#define printf
#define D(x)
#else
//#define printf(a,b...) 
#define D(x)
//#define D(x) {x;fflush(stdout);}
#endif
//int cch_ngms[NLEVEL];
//unsigned int cch_ngm_data[NLEVEL][cch_ngm_size];
YX cch_mg[14][90][90];
static YX *mdptr;
YX XQAI::mg_start(YX yx)
{
mdptr=cch_mg[(id2type(_bd(yx))<<1) | (_bd(yx)&1)][yx];
 return mdptr[yx];
}

#define MG_NEXT (mddst == 0xff ? 0xff : mddst=mdptr[mddst])
int cch_cap_level[7][7]={{0,6,6,2,1,2,2},
                            {6,6,6,2,1,2,3},
                            {6,6,6,2,1,2,3},
                            {0,3,3,2,1,2,4},
                            {0,3,3,3,2,3,4},
                            {0,3,3,2,1,2,4},
                            {0,3,3,1,1,1,3}};
int cap_level[34][34];
static int MoveOrder[16]={rook1<<1,rook2<<1,cannon1<<1,cannon2<<1, 
                          knight1<<1, knight2<<1,pawn1<<1,pawn2<<1,
			  pawn3<<1,pawn4<<1,pawn5<<1,king<<1,
			  minister1<<1,minister2<<1,guard1<<1,guard2<<1};

int XQAI::gam_easy(MoveList &ml)
{
int i,i0,d,total=0;
int level;
register YX yx,pyx;
register  int x;
int y,px,clr;
#define ml_Set(LV,_YX,_D) {ml.lmv[LV][ml.lmoves[LV]++]=(pyx<<8)|_YX;\
				total++;}
#define	AddAttackMove(__P,__YX) d=_bd(__YX);\
     			if(d && (d&1) !=clr)\
                 	{\
      		   		level=cch_cap_level[__P][id2type(d)];\
      		   		ml_Set(level,__YX,d);\
				if(level==0) return total;\
                 	}
clr=Turn();
for(i=0;i<6;i++) ml.lmoves[i]=0;
for(i0=0;i0<2;i0++)
 if(YX_isalive((pyx=cch_p[i=clr+MoveOrder[i0]])))
	  {
      px=YX_X(pyx);
	y=pyx-px;
	        for(x=pyx-1;x>=y && !_bd(x);x--) {ml_Set(5,x,0);}
		if(x>=y)  {AddAttackMove(Rook,x)}
        y+=8;
	        for(x=pyx+1;x<=y &&!_bd(x);x++) {ml_Set(5,x,0);}
     		if(x<=y)  {AddAttackMove(Rook,x)}
	        for(x=pyx+9;x<90 && !_bd(x);x+=9) 
			{ml_Set(5,x,0);}
		if(x<90) {AddAttackMove(Rook,x)}
		for(x=pyx-9;x>=0 && !_bd(x);x-=9)
			{ml_Set(5,x,0);}
		if(x>=0)  {AddAttackMove(Rook,x)}

  }

for(;i0<4;i0++)
 if(YX_isalive((pyx=cch_p[i=clr+MoveOrder[i0]])))
	  {
	px=YX_X(pyx);
	y=pyx-px;
		for(x=pyx-1;x>=y && !_bd(x);x--)
			{ml_Set(5,x,0);}
		for(x--;x>=y && !_bd(x);x--);
		if(x>=y) {AddAttackMove(Cannon,x)}
	y+=8;	
		for(x=pyx+1;x<=y && !_bd(x);x++)  
			{ml_Set(5,x,0);}
		for(x++;x<=y && !_bd(x);x++);
		if(x<=y) {AddAttackMove(Cannon,x)}


		for(x=pyx+9;x<90 && !_bd(x);x+=9) 
			{ml_Set(5,x,0);}
		for(x+=9;x<90 && !_bd(x);x+=9);
		if(x<90) {AddAttackMove(Cannon,x)}
		for(x=pyx-9;x>=0 && !_bd(x);x-=9) 
			{ml_Set(5,x,0);}
		for(x-=9;x>=0 && !_bd(x);x-=9);
		if(x>=0) {AddAttackMove(Cannon,x)}
  }//if(cch_p[i].存活()

for(;i0<16;i0++)
 if(YX_isalive((pyx=cch_p[i=clr+MoveOrder[i0]])))
for(yx=mg_start(cch_p[i]);yx!=0xff;yx=mdptr[yx])
	{
d=_bd(yx);
if(!d)  {ml_Set(5,yx,0);}
 else if((d&1) !=clr)
                 {
      		   level=cch_cap_level[id2type(i)][id2type(d)];
		   if(level<6)
      		     ml_Set(level,yx,d);
                 }
	 
     }//for yx mg
  return total; 
}

int XQAI::gam_cf(MoveList &ml)
{
int i,i0,d,total=0;
int level;
register YX yx,pyx;
register  int x;
int y,px,clr;
clr=Turn();
for(i=0;i<6;i++) ml.lmoves[i]=0;
for(i0=0;i0<2;i0++)
 if(YX_isalive((pyx=cch_p[i=clr+MoveOrder[i0]])))
	  {
		  U96 t;
      px=YX_X(pyx);
	y=pyx-px;
		for(x=pyx-1;x>=y && !_bd(x);x--);
		if(x>=y) {AddAttackMove(Rook,x)}
/*		x=max(bmap.and_frs(line[pyx][y]),y);
		{AddAttackMove(車,x)}*/
        y+=8;
	        for(x=pyx+1;x<=y &&!_bd(x);x++);
     		if(x<=y)  {AddAttackMove(Rook,x)}
/*		x=min(bmap.and_ffs(line[pyx][y]),y);
		{AddAttackMove(車,x)}*/
	        for(x=pyx+9;x<90 && !_bd(x);x+=9);
		if(x<90) {AddAttackMove(Rook,x)}
		for(x=pyx-9;x>=0 && !_bd(x);x-=9);
		if(x>=0)  {AddAttackMove(Rook,x)}
/*	for(j=(!clr)|2;j<34;j+=2)
		if(RelativeRule(pyx,cch_p[j]))
			{AddAttackMove(車,cch_p[j])}*/

  }//if(cch_p[i].存活()

for(;i0<4;i0++)
 if(YX_isalive((pyx=cch_p[i=clr+MoveOrder[i0]])))
	  {
	px=YX_X(pyx);
	y=pyx-px;
		for(x=pyx-1;x>=y && !_bd(x);x--);
		for(x--;x>=y && !_bd(x);x--);
		if(x>=y) {AddAttackMove(Cannon,x)}
	y+=8;	
		for(x=pyx+1;x<=y && !_bd(x);x++); 
		for(x++;x<=y && !_bd(x);x++);
		if(x<=y) {AddAttackMove(Cannon,x)}
		for(x=pyx+9;x<90 && !_bd(x);x+=9);
		for(x+=9;x<90 && !_bd(x);x+=9);
		if(x<90) {AddAttackMove(Cannon,x)}
		for(x=pyx-9;x>=0 && !_bd(x);x-=9);
		for(x-=9;x>=0 && !_bd(x);x-=9);
		if(x>=0) {AddAttackMove(Cannon,x)}
  }//if(cch_p[i].存活()

for(;i0<16;i0++)
 if(YX_isalive((pyx=cch_p[i=clr+MoveOrder[i0]])))
for(yx=mg_start(cch_p[i]);yx!=0xff;yx=mdptr[yx])
	{
d=_bd(yx);
     if(d && ((d&1) !=clr)) {
  		level=cch_cap_level[id2type(i)][id2type(d)];
		if(level<6)
      		    ml_Set(level,yx,0);
         }
	 
     }//for yx mg
   return total;
}
extern int MaterialValue[7];
int XQAI::mm_quiet(mmvar)
{
#define Compare if(v>BestScore) if((BestScore=v)>alpha) \
	if((alpha=v)>=beta){D(printf("quiet CUT %d\n",v));return v;}
int BestScore,v,i0,t,lmoves;
int CallPly;
YX yx,pyx;
int clr,eat_level;
MoveList ml;
D(printf("mm_quiet %d %s %d\n",sdepth,Chinese_Notation(),_Ply));
clr=Turn();
if(!sdepth) info.TotalNodes++;
v=(clr ? -Value()+5 : Value()+5);
if(abs(v)>=8000/* && sdepth <= -InitDepth*/) return v;
beta=min(beta,10000-_Ply-1);
BestScore=-20000;
Compare
	
int mp=YX_isalive(cch_p[(rook1<<1)|(!clr)])
		  ||YX_isalive(cch_p[(rook1<<2)|(!clr)])
		  ? 420: 192;
if(v+mp<alpha)
{
	YX Opponent=cch_p[2|(!clr)];
	for(int i0=0;i0<12;i0++)
		if(CaptureRule(MoveOrder[i0]|clr,Opponent))
			return 10000-_Ply-1;
	return v+mp;
}

//if(v+500<alpha) return v;
if(!gam_cf(ml)) return v;
int first=1;
int ov=v;
//for(eat_level=0;eat_level<5;eat_level++)
//printf("%d %d %d\n",sdepth,eat_level,ml.lmoves[eat_level]);
for(eat_level=0;eat_level<5;eat_level++)
     for(t=0,lmoves=ml.lmoves[eat_level];t<lmoves;t++)
     {
	     {
	      register unsigned int temp=ml.lmv[eat_level][t];
	      yx=temp&0xff;
	      if(MaterialValue[id2type(_bd(yx))]+ov<alpha) continue;
	      pyx=temp>>8;
	     i0=_bd(pyx)>>4;
	     }
              CallPly=sdepth-1;
	      D(printf("mm_quiet %d %d,%d %d,%d\n",sdepth,YX_Y_X(pyx),YX_Y_X(yx)))
	      if(i0<4) //跳開車炮的case，也許會快一點
	      {      
	      if(i0==3) //馬
	      		{if(_bd(KnightLeg(pyx,yx))) continue;}
	      else if(i0==2) //象
	        	{if(_bd(MinisterEye(pyx,yx))) continue;}
	      else if (!i0)//將帥
		   	{if(!RelativeRule(pyx,yx)) continue;}
	      }
                  fMove(pyx,yx);
		  if(!first && beta-alpha>2)
		  {
                  	v=-mm_quiet(CallPly,-alpha-1,-alpha);
			if(v>alpha && v<beta)
				v= max(v,-mm_quiet(CallPly,-beta,-v));
		  }
                  else v=-mm_quiet(CallPly,-beta,-alpha);
		  first=0;
	        Back();
	     Compare
	}
  D(printf("mm_quiet %d 最後return %d\n",sdepth,BestScore));
  return BestScore;
}

