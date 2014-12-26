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

#include "cbdb.h"
static void R(YX *img)
{
int i;
for(i=0;i<32;i++)
img[i]=YX_R(img[i]);
}

int ImageNode::Add(int r,YX s,YX d)
{
int i; if(r>=3 || r<0) {
#ifndef VC
fprintf(stderr,"ErrorResult:%d\n",r);
#endif
return 0;}			if(Result[2]>=0) Result[r]++;
				Games++;
				return 1;}

 XQDB::XQDB(char *name) 
{
	file = my_connect (file,name);
}
XQDB::~XQDB()
{
my_close(file);
}
int XQDB::Normalize(YX *img)
{
YX tmp;
#define SWAP(a,b) {tmp=img[a];img[a]=img[b];img[b]=tmp;}
#define CSWAP(a,b) if(img[a]>img[b]) SWAP(a,b)
#define CSP(a,b) CSWAP(2*a,2*b)  CSWAP(2*a+1,2*b+1)
CSP(1,2)
CSP(3,4)
CSP(5,6)
CSP(7,8)
CSP(9,10)
CSP(11,12)
CSP(11,13)
CSP(11,14)
CSP(11,15)
CSP(12,13)
CSP(12,14)
CSP(12,15)
CSP(13,14)
CSP(13,15)
CSP(14,15)
return 1;
}
//code for 車馬炮(Rook/Knight/Cannon)
inline unsigned long long  num(unsigned a,unsigned b)
{
	if(a== 0xff && b==0xff) return 0;
	if(b==0xff) return a+1;
	return 91+b*(b-1)/2+a;
}
//code for 帥(King)
inline unsigned long long  num0(YX a)
{
	if(a== 0xff) return 0;
	return YX_Y(a)*3+YX_X(a)-2;
}
//code for 士(Guard)
inline unsigned num1_1(YX a)
{ 
	return YX_Y(a)==1 ? 4 :(YX_X(a)-3)/2+2*(YX_Y(a)/2);
}
inline unsigned long long  num1(YX a,YX b)
{
	unsigned t1,t2;
	if(a== 0xff && b==0xff) return 0;
	if(b==0xff) return num1_1(a)+1;
	t1=num1_1(a);
	t2=num1_1(b);
	return t1>t2? t1*(t1-1)/2+t2+6: t2*(t2-1)/2+t1+6;
}
//code for 象(Minister)
inline unsigned num2_1(YX a)
{ 
	return YX_Y(a)==2 ? 4+YX_X(a)/4 :(YX_X(a)-2)/4+2*(YX_Y(a)/4);
}
inline unsigned long long  num2(YX a,YX b)
{
	unsigned t1,t2;
	if(a== 0xff && b==0xff) return 0;
	if(b==0xff) return num2_1(a)+1;
	t1=num2_1(a);
	t2=num2_1(b);
	return t1>t2? t1*(t1-1)/2+t2+6: t2*(t2-1)/2+t1+8;
}
//code for 兵(Pawn)
inline unsigned long num5_1(YX a)
{
	return a < 45 ? (a-26)/2 : a-35;
}
#define L1 1+55
#define L2 L1+55*54/2
#define L3 L2+55*54*53/6
#define L4 L3+55*54*53*52/24
#define L5 L4+55*54*53*52*51/120
inline unsigned long long num5(YX a,YX b,YX c,YX d,YX e)
{
	unsigned t1,t2,t3,t4,t5;
	if(a==0xff) return 0;
	t1=num5_1(a);
	if(b==0xff) return t1+1;
	t2=num5_1(b);
	if(c==0xff) return t2*(t2-1)/2+t1+L1;
	t3=num5_1(c);
	if(d==0xff) return t3*(t3-1)*(t3-2)/6+t2*(t2-1)/2+t1+L2;
	t4=num5_1(d);
	if(e==0xff) return t4*(t4-1)*(t4-2)*(t4-3)/24+
		t3*(t3-1)*(t3-2)/6+t2*(t2-1)/2+t1+L3;
	t5=num5_1(e);
	return  t5*(t5-1)*(t5-2)*(t5-3)*(t5-4)/120+
	        t4*(t4-1)*(t4-2)*(t4-3)/24+
		t3*(t3-1)*(t3-2)/6+t2*(t2-1)/2+t1+L4;
}
int XQDB::img2key(YX *img1,uchar *bdd,int color)
{
int i,lr,j;
YX img2[32],tmp;
YX *img;
img=img1;
for(i=1;i<32;i+=2) if(img[i]!=0xff) img[i] =YX_T(img[i]);
if(color) 
	for(i=0;i<16;i++) SWAP(2*i,2*i+1);
for(i=0;i<32;i++) img2[i]=YX_R(img1[i]);
Normalize(img1);
Normalize(img2);
unsigned long long n1,n2;
//忽略左右差異
lr=0;
for(i=0;i<32 && img1[i]==img2[i];i++);
if(i==32) lr=0;
else	lr= img1[i]<img2[i] ? -1 : 1;
img = lr<0 ? img2 :img1;
for(i=0;i<32;i++)
	bdd[i]=img[i];
n1=num5(img[22],img[24],img[26],img[28],img[30]);
n1=n1*29+num2(img[6],img[8]);
n1=n1*9+num0(img[0]);
n1=n1*16+num1(img[2],img[4]);
n1=n1*4096+num(img[10],img[12]);
n1=n1*4096+num(img[14],img[16]);
n1=n1*4096+num(img[18],img[20]);

n2=num5(img[23],img[25],img[27],img[29],img[31]);
n2=n2*29+num2(img[7],img[9]);
n2=n2*9+num0(img[1]);
n2=n2*16+num1(img[3],img[5]);
n2=n2*4096+num(img[11],img[13]);
n2=n2*4096+num(img[15],img[17]);
n2=n2*4096+num(img[19],img[21]);
for(i=0;i<8;i++,n1>>=8)
	bdd[i]=n1&0xff;
for(i=0;i<8;i++,n2>>=8)
	bdd[i+8]=n2&0xff;
return lr;
}
int XQDB::GetNode(uchar * key,ImageNode &p)
{uchar  data[1500];
int *a;
int l;
int i;
uchar *b;
p.Clean();
if(my_fetch(file,key,data,&l))
{
if(l<12) {
#ifndef VC	
fprintf(stderr,"out get node l=%d<12\n",l);
#endif
return 0;}
a=(int *)data;
for(i=0;i<3;i++) p.Result[i]=a[i];
p.Games=a[0]+a[1]+a[2];
}
else return 0;
return 1;
}
int XQDB::SaveNode(uchar *key,ImageNode &p)
{
uchar data[1500];
int *a;
int i,ret,l;
uchar *b;
l=12;
a=(int *)data;
for(i=0;i<3;i++) a[i]=p.Result[i];
ret = my_store(file, key, data,l);  
return ret;
}

int XQDB::Add(YX *img,int color,int Result,YX s,YX d)
{
int lr;
ImageNode p;
uchar key[70];
lr=img2key(img,key,color);
GetNode(key,p);
p.Add((color && Result!=2) ? 1-Result : Result,0,0);
SaveNode(key,p);
return 1;
}
int XQDB::SetNode(XQ *bd,ImageNode &p)
{
int lr,color;
YX img[32];
uchar key[70];
bd->Image(img,color);
lr=img2key(img,key,color);
SaveNode(key,p);
return 1;
}
int XQDB::Add(XQ *bd,int Result,int Ply)
{
int i;
YX yx[32];
YX s,d;
int color;
int n=bd->Ply();
if (bd->MaxPly()<Ply) Ply=bd->MaxPly();
if(!Ply) return 0;
bd->GotoStart();
do
{
bd->Image(yx,color);
Add(yx,color,Result,s,d);
}while(bd->Forward() && bd->Ply()< Ply);
bd->GotoPly(n);
return 1;
}
int XQDB::Cut(char *fn1)
{FILE *f1;
uchar buf[KL+8+DL];
int *data;
int recn,i;
f1=fopen(fn1,"rb");
fseek(f1,0,SEEK_END);
recn=ftell(f1)/(KL+4+DL);
printf("總共 %d Image\n",recn);
fseek(f1,0,SEEK_SET);
for(i=0;i<recn;i++)
{	
	fread(buf,KL+4+DL,1,f1);
	data=(int *)(&buf[KL+4]);
	if(data[1]+data[2]+data[0]>=7)
	{
		printf("%d %d\n",i,*data);
		my_store(file,buf,&buf[KL+4],DL);
	}
}
return 1;	
}


int XQDB::FindNode(YX *img,int color,ImageNode &p)
{
int i,lr,ret;
uchar key[70];
YX tmp;
lr=img2key(img,key,color);
ret=GetNode(key,p);
if(!ret) return 0;
return ret;
}

int XQDB::FindNode(XQ *B,ImageNode &p)
{
int color;
YX img[32];
B->Image(img,color);
return FindNode(img,color,p);
}


