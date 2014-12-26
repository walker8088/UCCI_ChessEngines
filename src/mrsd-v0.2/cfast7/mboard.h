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
#ifndef MBOARD_H
#define MBOARD_H
#define TRANS
#define HASHSIZE 0x100000
#define HASHMASK 0xfffff
#include <board.h>
#include <stdio.h>
#include "tools.h"
#define ushort unsigned short
extern int (*msg_cb)(const char *,...);
#ifdef PVLINE
class PVLine
{
	public:
	int length;
	unsigned short move[30];
	void operator=(PVLine &p)
	{
		length=p.length;
		memcpy(move,p.move,60);
	}
} PVLine;
PVLine pvline[64];
#endif
extern ushort _pvline[30];
template <class T>int same(T *a,T *A,int n)
{
int i;
for(i=0;i<n;i++)
if(a[i]!=A[i]) return 0;
return 1;
}
//§O¦r­×¥¿
typedef struct  
{
unsigned short lmv[6][128];
int lmoves[6];
} MoveList;
class GameStatus
{
public:
int TotalNodes;
char ErrorText[300];
long time;
trace tr[10];
int data[10];
int sa[20][7];
int hung,checked;
};
#define cch_ngm_size 45
extern int cch_mginit;
extern YX cch_mg[14][90][90];
extern int cch_cap_level[7][7];
extern int cch_value[512];
#define NLEVEL 256
#define NLEVELMASK 0xff
extern int cch_ngms[NLEVEL];
extern unsigned int cch_ngm_data[NLEVEL][cch_ngm_size];
extern	U96 line[90][90];
extern	U96 MoveArray[14][90];
extern ull pos_hval[7][2][90];
class XQAI:public XQ
{
protected:
int usegm;
int useext;
U96 bmap;
public:
static int MaterialPositionValue[34][90];
GameStatus info;
int dummyid;
YX dummypos;
XQAI ();
unsigned int *ui;
YX mg_start(YX yx);
XQAI(XQ &XQ2);
//XQAI(XQAI &XQ2);
~XQAI();
int automove(int AUTO_LEVEL=8,int AUTO_TIME=3);
int _FollowBook();
#define MMVAR const int sdepth,int alpha=-20000,int beta=20000
#define mmvar  const int sdepth,int alpha,int beta
ushort BestMove[500];
//MinMax
int mm_root(MMVAR);
int mm_pv(MMVAR);
int mm_quiet(MMVAR);
int mm_7(MMVAR);
//Move generators
int gam_easy(MoveList &);
int gam_cf(MoveList &);
int XQAI::Value()
{
return   !YX_isalive(cch_p[2]) ?  -10000+_Ply :
      (!YX_isalive(cch_p[3]) ?  10000-_Ply :cch_value[_Ply]);
}
int Value(int i,YX yx){return (i>=2 && i<=33 ?MaterialPositionValue[i][yx]:0); }
void ValueInit();
void addgm(ushort mv);
int Back ()
  {
	register int s;
	register YX pos;
	if (_Ply == 0)  {Error = 1;return 0;}
	pos=tr[--_Ply].dst;
        if(pos==0xff) return 1;
        bmap.setbit(s=tr[_Ply].src);
        cch_p[_bd(s)=_bd(pos)]=s;
        if((_bd(pos)=tr[_Ply].eat))
		 cch_p[_bd(pos)]=pos;
	else bmap.clearbit(pos);
	return 1;
  };

int Forward()
  {
	register int s;
	register YX pos;
	 if (_Ply >= _MaxPly){Error = 2;return 0;}
         pos=tr[_Ply].src;
        if(!YX_isalive(pos)){_Ply++;return 1;}
	if(tr[_Ply].eat)
	{
//	    printf("Forward%d %d",_Ply,tr[_Ply].eat);
	    YX_kill(cch_p[tr[_Ply].eat]);
	}
	else
		bmap.setbit(tr[_Ply].dst);
        s= _bd(pos);
	_bd(cch_p[s]=tr[_Ply].dst)=s;
	bmap.clearbit(pos);
	_bd(pos)=nop;
	 _Ply++;
	 return 1;
  };

int Move(const int sy,const int sx,const int dy,const int dx,const int flags=3)
{return Move(YX_Set(sy,sx),YX_Set(dy,dx));};
int fMove(const YX src,const YX dst);
#ifdef XXX
inline int nMove(YX yx1,YX yx2);
#endif
int Move(const YX src,const YX dst,const int flags=3);
int _Move(const ushort mv,const int flags=3){return Move(mv>>8,mv&0xff,flags);};
int Move();
YX KnightLeg(const YX a,const YX b) {return b>a ? (b>a+11 ? a+9 : a+(b<a+9 ? -1 : 1))
                                     : (a>b+11 ? a-9 : a+(a>b+9 ? -1 : 1));}
YX MinisterEye(const YX a,const YX b) {return (a+b)>>1;}
ull hvalue[500];
ull hValue(){return Turn() ? hvalue[_Ply] : ~hvalue[_Ply];};
ull hValue(int i,YX yx){
	return (i<2 || i>33) ? 0: pos_hval[id2type(i)][i&1][yx];};
void hValueInit();
int IsCheck(const int BackPly=0);
bool CaptureRule(const int id,YX dst)
{
 YX src=cch_p[id];
 if(!YX_isalive(src)) return 0;
 if((id>>1)<rook1 || (id>>1)>cannon2)
  {if(!MoveArray[(id2type(id)<<1)|(id&1)][src].bit(dst)) return 0;}
   return RelativeRule(src,dst);
}


int RelativeRule(const YX,const YX);
//Flags
int checked,multi_hung;
int trace;
int usefind;
int usefast;
int savegm;
int usepvs;
int in_nullstep;
int InitDepth;
int stop_thinking;
};
extern int MAXLEVEL; 
extern int L1,M1,M2,S1;
class MoveNode {
public:
ushort move;
int ub,lb;
MoveNode& operator=(MoveNode &a){move=a.move;ub=a.ub;lb=a.lb;return *this;}
bool operator>(MoveNode &a){return lb>a.ub || (lb==a.ub && ub>lb);}
};
#endif

