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
#include <time.h>
#define UNKNOWN -777777
//#define DEBUG_MESSAGE
#ifdef DEBUG_MESSAGE
#define DEBUG(a,arg...) {printf(a,## arg);fflush(stdout);}
int debug=0;
#else
//#define DEBUG(a,arg...)
#endif
#ifdef __BORLANDC__
#define VC 1
#endif
#if defined(VC)
#define ull unsigned __int64
#define LL1 1ui64
#else 
#define ull unsigned long long
#define LL1 1ULL
#endif
#define check(a,n) 
//#define check(a,n) if(a>=n || a<0) {printf("error!!! %s >=%d in %s %d",#a,n,__FUNCTION__,__LINE__);exit(1);}
//tools for 90bits data
#include "u96.h"
//tools for initgame
#ifdef CONSTANT_BD
#ifndef VC
#define _1lbd 0x20100 
#define _0lbd 0x8040201008040201LL
#define _1rbd 0x2010080 
#define _0rbd 0x4020100804020100LL
#define _1lbd2 0x60301 
#define _0lbd2 0x80c06030180c0603LL
#define _1rbd2 0x30180c0 
#define _0rbd2 0x6030180c06030180LL
#define _1eveny 0x1ff00 
#define _0eveny 0x7fc01ff007fc01ffLL
#define _1zoo 0x880088 
#define _0zoo 0x8008844004440044LL
#define _1palace 0x70381c 
#define _0palace 0xe07038LL
#endif
#endif

template <class T>
inline void swap (T& a, T& b) {
   T tmp = a;
   a = b;
   b = tmp;
}
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#ifdef VC
template <class T> 
inline T max(const T a,const T b)
{ return (a>b) ? a: b;}
template <class T>
inline T min(const T a, const T b){ return a<b ? a: b;}
#else
#define max(a,b) ((a)>? (b))
#define min(a,b) ((a)<? (b))
#endif
