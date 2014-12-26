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

#define ndef
#include <stdio.h>
#include <time.h>
#include "board.h"
#include <math.h>
#include "cbfile.h"
#include "cbdb.h"
XQFile cf;
int t0,t1,ts=0;
XQDB odb;
main(int argc,char **argv)
{
int i,v,speed;
YX s,d;
char na[100];
XQ *B;
ImageNode p;
B=new XQ();
for(i=atoi(argv[1]);i<atoi(argv[2]);i++)
{
cf.SetFileName(i);
if(cf.Load(*B))
{
odb.Add(B,cf.Result&0xf,60);
B->GotoStart();
if((i+1)%10==0)
{	
odb.FindNode(B,p);
printf("Games=%d ¬õ³Ó%d ¶Â³Ó%d ©M%d\n",
	p.Games,p.Result[0],p.Result[1],p.Result[2]);
if((i+1)%50==0) odb.sync();
}
}
else exit(1);
}
}
