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

#include "board.h"
#include <ctype.h>
#include <stdlib.h>
#ifdef MOREC
char cch_b[160];
YX cch_p[34];
#endif
int XQ::
Move ()
{
  tr[_Ply].eat = nop;
  YX_kill(tr[_Ply].src);
  YX_kill(tr[_Ply].dst);
  _Ply++;
/*  if (_Ply > _MaxPly)*/
	 _MaxPly = _Ply;
  return 1;
}
int XQ::
Rule(int sy, int sx, int dy, int dx)
{
  char s , d, cc;
  int lx,ly;
//common rule
  if (sx < 0 || sy < 0 || dy < 0 || dx < 0) return 0;
  if (sx >= maxx || sy >= maxy || dy >= maxy || dx >= maxx)
  return 0;
  s=bd(sy,sx);
  if (s == nop) return 0;
//  if ( (s&1) != Turn ()) return 0;
cc=s&1;
  d = bd(dy,dx);
  if (d)
	 if ( (d&1)  == cc) return 0;
// special rule
  lx = dx - sx;
  ly = dy - sy;
  if (!lx && !ly)
	 return 0;
  switch (s>>1)
	 {
	 case knight1:
	 case knight2:
		  if ((abs (lx) + abs (ly)) != 3)	return 0;
		if (!lx || !ly)           	return 0;
		if (bd(sy + ly / 2,sx + lx / 2)) 	return 0;
		break;
	 case minister1:
	 case minister2:
		if (abs (lx) != 2) 	return 0;
		if (abs (ly) != 2) 	return 0;
		if (!cc && dy > 4) 	return 0;
		if (cc && dy < 5)  	return 0;
		if (bd(sy + ly / 2,sx + lx /2)) 	return 0;
		break;
	 case guard1:
	 case guard2:
		if (abs (lx) != 1)	 return 0;
		if (abs (ly) != 1)  return 0;
		if (!cc && dy > 2) return 0;
		if (cc && dy < 7)   return 0;
		if (dx < 3 || dx > 5)   return 0;
		break;
	 case pawn1:
	 case pawn2:
	 case pawn3:
	 case pawn4:
	 case pawn5:
		if ((abs (lx) + abs (ly)) != 1)   	return 0;
if (ly)
	{ if (cc )	{if(ly > 0)		 return 0;}
	  else if (!cc){if(ly < 0)    return 0;}}
else
	{  if (cc){if(sy >= 5) return 0;}
	 else if (!cc){if( sy <= 4)  return 0;}
	}
		break;
	 case rook1:
	 case rook2:
		if (bcount (sy, sx, dy, dx) != 0)	return 0;
		break;
	 case cannon1:
	 case cannon2:
		if (d && (bcount (sy, sx, dy, dx) != 1))	return 0;
		if (!d && bcount (sy, sx, dy, dx) != 0) return 0;
		break;
	 case king:
		if ((d >>1) == king)
	{
	  if (bcount (sy, sx, dy, dx) != 0)
		 return 0;
	}
		else
	{
	  if ((abs (lx) + abs (ly)) != 1)
		 return 0;
	  if (!cc && dy > 2)
		 return 0;
	  if (cc && dy < 7)
		 return 0;
	  if (dx < 3 || dx > 5)
		 return 0;
	}
		break;
	 default:
		return 0;
	 }
  return 1;
}
