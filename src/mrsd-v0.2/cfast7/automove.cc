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
#include "bd.h"
#include <cbdb.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef VC
#define msg_cb 
#define fflush
#else
int (*msg_cb)(const char *,...)=printf;
#endif
int MAXLEVEL=5; 
int L1=5,M1=100,M2=100,S1=1;
time_t SOFT_TIMELIMIT=0,EXPECT_TIME=0;
ushort _pvline[30];
extern void mv_init(XQAI *);
int XQAI::automove(int AUTO_LEVEL,int AUTO_TIME)
{
int v,v2,i,j,c;
int FullDepth; 
time_t sec1,sec2;
int alpha,beta;
stop_thinking=0;
bmap.clear();
for(i=0;i<90;i++) if(_bd(i)) bmap.setbit(i);
info.TotalNodes=0;
//for(i=0;i<20;i++) cch_ngms[i]=0;
	usegm=-1;
	savegm=-1;

	useext=0;
	usepvs=100;
	trace=100;usefast=-2;
        in_nullstep=0;
	EXPECT_TIME=0;
for(j=0;j<7;j++) for(i=0;i<20;i++) info.sa[i][j]=0;;

info.hung=0;
usefind=0;
mv_init(this);
if(_FollowBook()) return 1;
sec1=time(NULL);
	v=v2=mm_root(3);
#if 1
	v=mm_root(4);
if(S1==2 && (L1&1)) v=v2;
		FullDepth=L1;
//msg_cb("%d %d\n",v,v2);
do{
usepvs=2;trace=100;
in_nullstep=0;
if(!(S1&1)) v2=v;
alpha=v2-24;
//alpha=120;
//beta=130;
beta=v2+24;
//msg_cb("step=%d alpha=%d beta=%d time=%d\n",FullDepth,alpha,beta,time(NULL)-sec1);
	v2=v;
	int tstep=((FullDepth+1)&0xfffe)+1;
v=mm_root(FullDepth,alpha,beta);
if(v<=alpha && v>-8000)
{
//msg_cb("low fail at level %d alpha=%d beta=%d v=%d\n",FullDepth,alpha,beta,v);
   v=mm_root(FullDepth,-20000,alpha);
  // msg_cb("-20000 %d v=%d\n",alpha,v);
}
else
if(v>=beta && v<8000)
{
//msg_cb("high fail at level %d alpha=%d beta=%d v=%d\n",FullDepth,alpha,beta,v);
   v=mm_root(FullDepth,beta,20000);
}

	sec2=time(NULL);
FullDepth+=S1;
info.time=sec2-sec1;
EXPECT_TIME=info.time*4+sec2;
if(_Move(BestMove[_Ply]))
{
//msg_cb("%d,%d Best­È=%d  %s \n",InitDepth,info.time,v,Chinese_Notation());
Back();
}
//else
//msg_cb("%d,%d Best­È=%d  Error \n",InitDepth,info.time,v);
}
while(abs(v)<8000 &&
((info.time < AUTO_TIME && (sec2< SOFT_TIMELIMIT || !SOFT_TIMELIMIT))
 || FullDepth<=AUTO_LEVEL) && !stop_thinking);
#endif
if(stop_thinking && abs(v)>10000) v=v2;
DoMove:
_pvline[0]=0;
if(abs(v)<=10000)
{
if(!_Move(BestMove[_Ply]))
{
	msg_cb("Can't Move\n");
}
else{
	BD *bd;
	int j=0;
	while((bd=Get_HASH(hValue())) && j<7 && _Move(bd->move) )
		_pvline[j++]=bd->move;
	_pvline[j]=0;
	for(;j>0;j--) Back();
}
}
SOFT_TIMELIMIT=0;
stop_thinking=1;
return v;
}
extern MoveNode root_move[114];
extern int root_move_n;
int XQAI::_FollowBook()
{
int list[200],sum=0;
ushort mv[100];
int hand_set=0;
XQDB *db;
ImageNode p,p2;
MoveList ml;
YX pyx,yx;
int a,i;
time_t l;
static int srand_init=0;
root_move_n=0;
db=new XQDB();
//if(!db) return 0;
//if(!db->FindNode(this,p)) {delete db;db=NULL;}
//if(p.Games<5) {delete db;db=NULL;}
gam_easy(ml);
i=0;
  for(int eat_level=0;eat_level<6;eat_level++)
	  for(int t=0;t<ml.lmoves[eat_level];t++)
		  
{        register unsigned int temp=ml.lmv[eat_level][t];
		  pyx=temp>>8;
		  yx=temp&0xff;	
	if( _bd(pyx)<16 && !RelativeRule(pyx,yx) )  continue;
	root_move[root_move_n++].move=temp;
	if(!db) continue;
	Move(pyx,yx,2);
	list[i]=0;
	if(!db->FindNode(this,p2)) p2.Games=0;
	Back(); 
	if(p2.Games && p2.Result[2]==-1 && p2.Result[0]>0 && p2.Result[1]==0)
	{
		root_move_n--;		
	}
	else if(p2.Games && p2.Result[2]==-1 && p2.Result[1]>0 && p2.Result[0]==0)
	{
			if(!hand_set) {i=0;hand_set=1;
				list[0]=sum=p2.Result[1];}
			else sum+=(list[i]=p2.Result[1]);
		      mv[i]=temp;
		      i++;
	}
	else if(!hand_set && p2.Games>=5 &&
			(a=p2.Result[1]*2+p2.Result[2])>=p2.Games*0.80)
	{
		      sum+=(list[i]=(a*5)/p2.Games+p2.Games*2);
		      mv[i]=temp;
		      i++;
	}

}
if(db) delete db;
else return 0;
if(!sum) return 0;
#ifndef VC
if(!srand_init)
#endif	
{
srand(time(NULL));
srand_init=1;
}
ull ll=rand() ^ ((ull)rand() << 15) ^ ((ull)rand() << 30) ^
        ((ull)rand() << 45) ^ ((ull)rand() << 60);
//printf("a %d %llu %llu %d\n",sum,ll,ll%sum,rand());
ll=ll%sum;
int n=i;
for(i=0,a=0;i<n;i++)
{
	a+=list[i];
//	printf("%d %d %d\n",i,a,sum);
	BestMove[_Ply]=mv[i];
	if(a>ll)	return _Move(mv[i]);
}
	return 0;
}
extern void HashClean();
void mv_init(XQAI *B)
{
HashClean();
B->stop_thinking=0;
}
