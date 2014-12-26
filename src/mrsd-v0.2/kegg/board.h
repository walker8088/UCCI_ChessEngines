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

#ifndef BOARD_H
#define BOARD_H
#include <stdlib.h>
#include <stdio.h>
#define myabs(x) ((x)>=0 ? (x) :-(x))
//#define abs myabs
#define BOUNDED(x,l,u) {if(x<l) x=(l);if(x>(u)) x=(u);}
// YX(YX)的定義
#define YX unsigned char
//Constructor
// 無參數  YX起始值為0xff(死)
#define YX_Set(Y,X) (((Y)<<3)+(Y)+(X))
#define YX_SetX(yx,X) (yx+=((X)-yx%9))
#define YX_SetY(yx,y) (yx=((y)<<3)+(y)+yx%9)
#define YX_X(yx) ((yx)%9)
#define YX_Y(yx) ((yx)/9)
#define YX_Y_X(yx) YX_Y(yx),YX_X(yx)
#define YX_kill(yx) (yx=0xff)
#define YX_isalive(yx)  ((yx)!=0xff)
#define YX_T(yx) (89-(yx)) 
#define YX_R(yx) ((yx)==0xff ? 0xff :  (yx)+8-2*((yx)%9) )
#define YX_LR(yx) ((yx)==0xff ? 0 : ( (yx%9)-4)

// YX定義結束
typedef struct  trace
{
  YX src, dst;
  char eat;
} trace;
  enum
	 {  nop = 0, king, guard1,guard2,
 minister1,minister2, knight1,knight2, rook1,rook2, cannon1
,cannon2, pawn1, pawn2, pawn3, pawn4, pawn5
	 };
class XQ
{
  protected:
 enum  {   King, Guard, Minister, Knight, Rook,Cannon, Pawn};
 enum
	 {
      red = 0, black = 1
    };

  int pidS(int t,char c);
   int pidO(int t,char c,char d);
//char &_bd(const YX yx){if(yx>=90) printf("XXXXXXXX!!!!!!!\n");return cch_b[yx];}
#define _bd(yx) cch_b[yx]
YX &_p(const int n){if(n<0 || n>=34) printf("_pXXXXXXXX!!!!!!!\n");return cch_p[n];}
char & bd(const int y,const int x){return _bd(YX_Set(y,x));}

  int TypeRule(int,int,int,int,int);
  int _Ply, _MaxPly, _MaxTrace;
  int Error;
  int bcount (const int sy, const int sx, const int dy, const int dx,const int max=1)
  {	 register int i, j, sum;
	 if (sy == dy)
	    {
		if (sx < dx)
	          {
		 	i = YX_Set(sy,sx + 1);
		 	j = YX_Set(sy,dx - 1);
	  	 }
	     else
	    	{
		 i = YX_Set(sy,dx + 1);
		 j = YX_Set(sy,sx - 1);
	        }
	for (sum = 0; i <= j; i++)
	  if (_bd(i))
	  {	 sum++;
            if(sum>max) return sum;
	  }


	return sum;
	}
     else if (sx == dx)
		{
	if (sy < dy)
	  {
		 i = YX_Set(sy +1,sx);
		 j = YX_Set(dy - 1,sx);
	  }
	else
	  {
		 i = YX_Set(dy + 1,sx);
		 j = YX_Set(sy - 1,sx);
	  }
 	for (sum = 0; i <= j; i+=9)
	  if (_bd(i))
                 {
		 sum++;
                if(sum>max) return sum;
                   }
	return sum;
		}
	 else
		return -1;
  }
public:
trace tr[1000];
char cch_b[90];
YX cch_p[34];
int id2type(const int id){return id>23 ? Pawn:id>>2;};
void type2id(const int type,const int co,int &start,int &end)
{
start=(type<<2)|co;
end=start+(type==6 ? 8 : 2);
if(!type) start|=2;		
}
#define maxx  9
#define maxy 10
  void init(YX pos[],int sc);
  void init();
  XQ ();
  XQ(XQ &XQ2);
  YX P(const int n){return cch_p[n];}

  void Image(YX *pos,int &sc);
  void Image(unsigned int *pos,int &sc);
  void xdel(YX);
  void xdel(int y,int x);
  void xdel(int n);
  void xset(int,YX);
  void xset(int,int,int);
  void xmove(YX,YX);
  ~XQ(){};
  char B(int y,int x){return bd(y,x);}
  //becarful ,no bounded check ,no color check
  int Rule (const int sy,const int sx,const int dy,const int dx);
  int Rule (const YX src,const YX dst) {return Rule(YX_Y_X(src),YX_Y_X(dst));}
  virtual int Move (const YX src, const YX dst,const int checkrule=1);
  virtual int Move (const int sy,const int sx,const int dy,const int dx,int checkrule=1)
   {return Move(YX_Set(sy,sx),YX_Set(dy,dx),checkrule);}
  virtual int Move();
  char Turn ()  {   return _Ply & 1; }
  int Ply ()   {    return _Ply;  }
  int MaxPly(){return _MaxPly;}
  unsigned short movepos(int mv_Ply=-1){
	  if(mv_Ply>=_MaxPly) return 0;
	  if(mv_Ply==-1) mv_Ply=_Ply-1;
          if(mv_Ply<0) return 0;
     return (tr[mv_Ply].src<<8)|tr[mv_Ply].dst;
  };
  void movepos(YX &s,YX &d,int mv_Ply=-1){
	 if(mv_Ply>=_MaxPly) {
  				YX_kill(s);
  				return;
  				}
  if(mv_Ply==-1) mv_Ply=_Ply-1;
  if(mv_Ply<0) {YX_kill(s);return;}
  s=tr[mv_Ply].src;
  d=tr[mv_Ply].dst;
  };
  char *movepos(char *str,int mv_Ply=-1) {
	  YX s,d;
	  movepos(s,d,mv_Ply);
	  str[0]=YX_X(s)+'A';
	  str[1]=YX_Y(s)+'0';
	  str[2]='-';
	  str[3]=YX_X(d)+'A';
	  str[4]=YX_Y(d)+'0';
	  str[5]=0;
	  return str;
  }
  int Back ()
  {
	register int s;
	register YX pos;
	 if (_Ply == 0)      {Error = 1;return 0;}
	_Ply--;
	pos=tr[_Ply].dst;
        if(pos==0xff) return 1;
        s=tr[_Ply].src;
        cch_p[_bd(s)=_bd(pos)]=s;
        if(_bd(pos)=tr[_Ply].eat)
                  cch_p[_bd(pos)]=pos;
	 return 1;
  };
  void Back(int i)
  {
  	for(int j=0;j<i;j++)
		Back();
  }
  int Forward ()
  {int s,d;
  YX pos;
	 if (_Ply >= _MaxPly){Error = 2;return 0;}
         pos=tr[_Ply].src;
        if(!YX_isalive(pos)){_Ply++;return 1;}
        s= _bd(pos);
        d= _bd(tr[_Ply].dst);
        YX_kill(cch_p[d]);
cch_p[s]=tr[_Ply].dst;
_bd(tr[_Ply].dst)=s;
_bd(pos)=nop;
	 _Ply++;
	 return 1;
  };
  void Forward(int i)
  {
  	for(int j=0;j<i;j++) Forward();
  }
  int GotoStart ();
  void NewGame (){init();};
  int GotoPly(int i);
  int GotoEnd ();
virtual  int CmdMove(char *str);
XQ& operator=(XQ &b1);
char *Chinese_Notation(int turn=-1);
int FollowBook();
};
#ifndef __GNUC__
//#define strcasecmp stricmp
inline int strcasecmp(const char *s1,const char *s2)
{
	while(*s1 && toupper(*s1) == toupper(*s2)) {s1++;s2++;}
	return *s1 || *s2;
}
#endif
#endif

 
