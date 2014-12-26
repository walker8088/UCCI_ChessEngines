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
XQDB odb("output");
main(int argc,char **argv)
{
int i,v,speed;
YX s,d;
char buf[500];
XQ *B;
ImageNode p;
printf("xxx\n");
odb.Cut(argv[1]);
}
