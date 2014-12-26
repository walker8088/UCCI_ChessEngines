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
#include <string.h>
#include <time.h>
#include <stdio.h>
int XQAI::RelativeRule(YX syx, YX dyx)
{
  char s,d,cc;
  if (syx==dyx) return 0;
  d =_bd(dyx);
  cc=(s=_bd(syx))&1;
  if (d && ( (d&1)  == cc) )return 0;
  switch (id2type(s))
	 {
	 case Rook:
	        return !(bmap&&line[syx][dyx]);
	 case Cannon:
		 if(d)
		 {
			 register U96 t;
			 t._and(line[syx][dyx],bmap);
			return t.is2power();
		 }
	 	else
	 	       return !(bmap&&line[syx][dyx]);
	 case Knight:
		 return !_bd(KnightLeg(syx,dyx));
	 case Minister:
		 return !_bd(MinisterEye(syx,dyx));
	 case King:
		return (d>>1) == king ? !(bmap&&line[syx][dyx]) 
		                    : (cc ? dyx>=45 : dyx<45);
	 }
  return 1;
}
