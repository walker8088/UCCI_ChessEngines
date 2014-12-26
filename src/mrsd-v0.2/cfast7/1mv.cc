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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mboard.h"
#include <math.h>
#include "bd.h"
#include "cbfile.h"
XQFile cf;
int t0,t1,ts=0;
main(int argc,char **argv)
{
int i,v,speed,j;
char buf[500];
int step= argc==1 ? 1 : atoi(argv[3]);
int level= argc==1 ? 1 : atoi(argv[4]);
BD *bd;
XQAI *B;
fflush(stdout);
B=new XQAI();
cf.SetFileName(argc==1 ? "rr.dat" : argv[1]);
cf.Load(*B);
B->GotoPly(argc==1 ? 14: atoi(argv[2]));
printf("%d\n",B->Ply());
t0=time(NULL);
for(i=0;i<step;i++)
{
v=B->automove(level,0);
if(B->info.time>0)
speed=B->info.TotalNodes/B->info.time;
printf("第%d步 %s (%5d) 以%3d秒搜尋%2d層共%dImage 每秒%dImage\n",
B->Ply(),B->Chinese_Notation(),v,B->info.time,B->InitDepth,B->info.TotalNodes,speed);
j=0;
while((bd=Get_HASH(B->hValue())) && B->_Move(bd->move) && j<20)
{
	printf("%s ",B->Chinese_Notation());
	j++;
}
if(j) printf("\n");
for(;j>0;j--) B->Back();
fflush(stdout);

ts+=B->info.TotalNodes;

}
t1=time(NULL)-t0;
printf("total time =%d, total searched=%d\n\
 average time=%d,average searched=%d average speed=%d\n",
t1,ts,t1/step,ts/step,(t1 ?ts/t1:-1));
}
