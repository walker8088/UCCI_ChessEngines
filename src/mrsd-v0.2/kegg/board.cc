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
#include <stdio.h>
#include <string.h>
#include "board.h"
#include <ctype.h>
#define piece(name,y,x,c) {bd(y,x)=(name<<1)|c;cch_p[(name<<1)|c]=YX_Set(y,x);}
#define piece2(name,y,x) piece(name,y,x,red);piece(name,9-y,x,black);
#define piece4(name,y,x) piece2(name,y,x);piece2((name+1),y,8-x);
void XQ::xdel(YX yx)
{
YX_kill(cch_p[_bd(yx)]);
_bd(yx)=0;
}
void XQ::xdel(int y,int x)
{
if(bd(y,x)<34)
YX_kill(cch_p[bd(y,x)]);
bd(y,x)=0;
}
void XQ::xdel(int n)
{
if(YX_isalive(cch_p[n]))
{
_bd(cch_p[n])=0;
YX_kill(cch_p[n]);
}
}
void XQ::xset(int n,YX yx)
{
xdel(n);
xdel(yx);
cch_p[n]=yx;
if(yx!=0xff) _bd(yx)=n;
}
void XQ::xset(int n,int y,int x)
{xset(n,YX_Set(y,x));}
void XQ::xmove(YX s,YX d)
{
xdel(d);
xset(_bd(s),d);
}
void XQ::init(YX pos[],int sc)
{
int i;
for(i=2;i<34;i++)
xset(i,pos[i-2]);
_bd(0)=0xff;
_bd(1)=0xff;
_MaxPly = _Ply = 0;
}
void XQ::Image(YX *pos,int &sc)
{int i;
sc=Turn();
for(i=2;i<34;i++)
  pos[i-2]=cch_p[i];
}
XQ::XQ()
{
init();
}
void XQ::Image(unsigned int *pos,int &sc)
{
#define M(a,b,c,d) (cch_p[a]<<24)|(cch_p[b]<<16)|(cch_p[c]<<8)|cch_p[d]
sc=Turn();
pos[3]=M(2,3,4,5);
pos[4]=M(6,7,8,9);
pos[2]=M(10,11,12,13);
pos[0]=M(14,15,16,17);
pos[1]=M(18,19,20,21);
pos[5]=M(22,23,24,25);
pos[6]=M(26,27,28,29);
pos[7]=M(30,31,32,33);
}
void XQ::init()
{int i,j;
  for (i = 0; i < maxy; i++)
	 for (j = 0; j < maxx; j++)
		bd(i,j) = nop;
  piece2 (king, 0, 4);
  piece4 (guard1, 0, 3);
  piece4 (minister1, 0, 2);
  piece4 (knight1, 0, 1);
  piece4 (rook1, 0, 0);
  piece4 (cannon1, 2, 1);
  piece2 (pawn1, 3, 4);
  piece4 (pawn2, 3, 2);
  piece4 (pawn4, 3, 0);
  cch_p[0]=cch_p[1]=0xff;
   _MaxPly = _Ply = 0;
}

int XQ::GotoStart ()
  { int i;
	 while (_Ply>0) i=Back ();
	 return i;

  };
	int XQ::GotoEnd ()
  {
  int i;
	 while(_Ply<_MaxPly) i=Forward();
	 return i;
  };

	int XQ::GotoPly (int n)
  {
  int i;
  GotoStart();
	 for(i=0;i<n;i++) Forward();
	 return 1;
  };

	int ra[] ={  1,  2,  4,  6,  8, 10, 12,17};
int XQ::pidO(int t,char c,char d)
{
int n,s,cmp=-1;
s=ra[t]*2+Turn();
if(c<='9'  && c>='1')
{
 if(Turn()) n=c-'1';	 else n='9'-c;
 if(YX_X(cch_p[s])==YX_X(cch_p[s+2]))
 {
if(d=='+' &&Turn()) cmp=1;
 if(d=='-' &&!Turn()) cmp=1;
 if(cmp==1) {if(YX_Y(cch_p[s])>YX_Y(cch_p[s+2])) return s;return s+2;}
  if(YX_Y(cch_p[s])>YX_Y(cch_p[s+2])) return s+2;return s;
 }
if(YX_X(cch_p[s])==n) return s;
return s+2;
}
if(c=='+' &&!Turn()) cmp=1;
 if(c=='-' &&Turn()) cmp=1;
 if(cmp==1) {if(YX_Y(cch_p[s])>YX_Y(cch_p[s+2])) return s;return s+2;}
  if(YX_Y(cch_p[s]) > YX_Y(cch_p[s+2]) ) return s+2;return s;
}
int XQ::pidS(int t,char c)
{
int n,s,e,i,j,j0,j1,j2,cmp=-1;;
s=24+Turn();
e=32+Turn();
if(c<='9'  && c>='1')
{
	 if(Turn()) n=c-'1';	 else n='9'-c;
for(i=s;i<=e;i+=2)
if(YX_X(cch_p[i])==n) {return i;}
return s;
}
if(c>='A' && c<='E')
{
n=c-'A';
for(i=8;i>=0;i--)
for(j=9;j>=0;j--)
if(bd(j,i)>=s)
 if(bd(j,i)<=e)
 if((bd(j,i)&1)==Turn())
{
if(n==0) return bd(j,i);
n--;
}
return s;
}/*A-E*/
#define CMP_SWAP(a,b) if(YX_Y(cch_p[a])<YX_Y(cch_p[b])) {j=a;a=b;b=j;}
j0=-1;
j1=-1;
j2=-1;
for(i=s;i<=e &&j0==-1;i+=2)
  for(j=i+2;j<=e;j+=2)
  if(YX_X(cch_p[i])==YX_X(cch_p[j]))
  {   if(j1==-1){  j0=i;	j1=j;}
  else {j2=j;跳出;}    	}
if(j2==-1) j2=j1;
CMP_SWAP(j1,j2);
CMP_SWAP(j0,j1);
CMP_SWAP(j1,j2);
cmp=-1;
if(c=='+' &&!Turn()) cmp=1;
 if(c=='-' &&Turn()) cmp=1;
 if(cmp==1) return j0;
  return j2;
}
static char translate_c2e(char *c)
{
int i;
char *c_table="將士象車馬" "包卒帥仕相"
              "硨傌炮砲兵" "前後進退平"
              "一二三四五" "六七八九" "1 "
              "2 3 4 5 6 " "7 8 9  1 2"
              " 3 4 5 6 7" " 8 9";
char *e_table="kgmrn" "cpkgm"
              "rnccp" "+-+-="
              "12345" "67891"
              "23456" "78912"
              "34567" "89";
for(i=0;i<strlen(c_table);i+=2)
if(!strncmp(c,&c_table[i],2)) return e_table[i/2];
return 0;
}
  int XQ::CmdMove(char *str)
  {
char ta[]={'k','g','m','n','r','c','p'};
  int t,i,pid,cmp=-1,n,c,d;
if(strlen(str)>=5 &&str[2]=='-'
 && (isalpha(str[0]) && isalpha(str[3]))
 && (isdigit(str[1]) && isdigit(str[4]))
)
{
return Move(str[1]-'0',tolower(str[0])-'a',
	    str[4]-'0',tolower(str[3])-'a');
}

else if(strlen(str)>=8 && !isalnum(str[0]))
{
char str2[5];
str2[4]=0;
for(i=0;i<4;i++)
str2[i]=translate_c2e(&str[2*i]);
if(strlen(str2)==4)
return CmdMove(str2);
}

  c=tolower(str[0]);
  for(t=0;t<7;t++) if(ta[t]==c) 跳出;
  if(t>=7) return 0;
  if(t==0)  pid=2+Turn();
  else if(t==6) pid=pidS(6,str[1]);
	else pid=pidO(t,str[1],str[2]);
	c=str[2];
	if(Turn()) n=str[3]-'1';	 else n='9'-str[3];
	if(c=='=') cmp=0;
	if(c=='+' &&!Turn()) cmp=1;
	if(c=='-' &&Turn()) cmp=1;
if(t==0 || t>= 4 )
{
d=(str[3]-'0')*cmp;
if(cmp==0) return Move(cch_p[pid],YX_Set(YX_Y(cch_p[pid]),n));
return
Move(YX_Y(cch_p[pid]),YX_X(cch_p[pid]),YX_Y(cch_p[pid])+d,YX_X(cch_p[pid]) );
}
if(t==1)
{
if(cmp==1) return Move(cch_p[pid],YX_Set(YX_Y(cch_p[pid])+1,n));
return Move(cch_p[pid],YX_Set(YX_Y(cch_p[pid])-1,n));
}
if(t==2)
{
if(cmp==1) return Move(cch_p[pid],YX_Set(YX_Y(cch_p[pid])+2,n));
return Move(cch_p[pid],YX_Set(YX_Y(cch_p[pid])-2,n));
}
if(t==3)
{
if(myabs(n-YX_X(cch_p[pid]))==1) d=2;
else d=1;
if(cmp==1) return Move(cch_p[pid],YX_Set(YX_Y(cch_p[pid])+d,n));
return Move(cch_p[pid],YX_Set(YX_Y(cch_p[pid])-d,n));
}
	return 1;
  }

int XQ::
Move (YX syx, YX dyx,int checkrule)
{
  char s , d;
//common rule
  s=_bd(syx);
  d=_bd(dyx);
#ifdef TRACE
printf("sy=%d sx=%d dy=%d dx=%d,ck=%d %d %d\n",sy,sx,dy,dx,checkrule,yx1,yx2);
#endif
if(checkrule)
{
  if (s == nop) return 0;
  if ( (s&1) != Turn ()) return 0;
if(!Rule(syx,dyx)) return 0;
}
  tr[_Ply].eat = d;
if(d) YX_kill(cch_p[d]);
  _bd(dyx) = s;
  _bd(syx) = nop;
cch_p[s]=dyx;
  tr[_Ply].src=syx;
  tr[_Ply].dst=dyx;
  _Ply++;
	 _MaxPly = _Ply;
  return 1;
}
