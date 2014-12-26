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
#include <memory.h>
#ifndef ull
#include "tools.h"
#endif
#define popcount_m1 0x55555555
#define popcount_m2 0x33333333
inline int popcount(const unsigned b)
{
	unsigned a=b-((b>>1)&popcount_m1);
	unsigned c=(a&popcount_m2)+((a>>2)&popcount_m2);
	c=(c&0x07070707)+((c>>4)&0x07070707);
	c=(c&0xffff)+c>>16;
	return (c&0xff)+c>>8;
}
#ifndef VC
static __inline__ int  myffz(unsigned  word)
{
	__asm__("bsfl %1,%0"
		:"=r" (word)
		:"r" (~word));
	return word;
}
static __inline__ int myfrz(unsigned  word)
{
	__asm__("bsrl %1,%0"
		:"=r" (word)
		:"r" (~word));
	return word;
}
static __inline__ int  myffs(unsigned  word)
{
	__asm__("bsfl %1,%0"
		:"=r" (word)
		:"r" (word));
	return word;
}
static __inline__ int  myfrs(unsigned  word)
{
	__asm__("bsrl %1,%0"
		:"=r" (word)
		:"r" (word));
	return word;
}
#endif
class U96 {
	public:
		union{
		unsigned a[4];
		ull b[2];
		};
		U96(){}
		U96(unsigned *as){memcpy(a,as,12);}
		void set(unsigned *as){memcpy(a,as,12);}
		void set(unsigned a0,unsigned a1=0,unsigned a2=0){
			a[0]=a0;
			a[1]=a1;
			a[2]=a2;
		}
		U96(unsigned a0,unsigned a1=0,unsigned a2=0){set(a0,a1,a2);}
		U96(const U96 &s){memcpy(a,s.a,12);}
		void clear(){a[0]=a[1]=a[2]=0;}
		U96& _and(const U96& s1,const U96& s2){
			a[0]=s1.a[0]&s2.a[0];
			a[1]=s1.a[1]&s2.a[1];
			//b[0]=s1.b[0]&s2.b[0];
			a[2]=s1.a[2]&s2.a[2];
			return *this;
		}
		U96 & _or(const U96 &s1,const U96 &s2){
			a[0]=s1.a[0]|s2.a[0];
			a[1]=s1.a[1]|s2.a[1];
			a[2]=s1.a[2]|s2.a[2];
			return *this;
		}
		U96& operator&(const U96 &s){	return U96()._and(*this,s);}
		U96& operator|(const U96 &s){	return U96()._or(*this,s);}
		U96& operator=(const U96 &s){
			memcpy(a,s.a,12);
			return *this;	
		}
		U96& operator&=(const U96 &s) {
			a[0]&=s.a[0];
			a[1]&=s.a[1];
			a[2]&=s.a[2];
			return *this;
		}
		U96 operator~()
		{
			return U96(~a[0],~a[1],~a[2]);
		}
		U96& operator^=(const U96 &s)	{
			a[0]^=s.a[0];
			a[1]^=s.a[1];
			a[2]^=s.a[2];
			return *this;
		}
		U96& operator|=(const U96 &s)
		{
			a[0]|=s.a[0];
			a[1]|=s.a[1];
			a[2]|=s.a[2];
			return *this;
		}
		bool operator==(const U96 &s)
		{
			return a[0]==s.a[0] &&
			       a[1]==s.a[1] &&
			       a[2]==s.a[2];
		}
		void show()
		{
			
			printf(" %x %x %x\n",a[0],a[1],a[2]);
		}
		bool operator&&(const U96 &s)
		{
			return (a[0]&s.a[0]) ||
			       (a[1]&s.a[1]) ||
			       (a[2]&s.a[2]);
		}
		bool operator!=(const U96 &s)
		{
			return !operator==(s);
		}
#ifndef VC
		int and_ffs(const U96 &s)
		{
			return a[0]&s.a[0] ? myffs(a[0]&s.a[0])    :
			       a[1]&s.a[1] ? myffs(a[1]&s.a[1])|32 :
			       a[2]&s.a[2] ? myffs(a[2]&s.a[2])|64 :
			       65;
		}
		int ffs()
		{
			return a[0] ? myffs(a[0])    :
			       a[1] ? myffs(a[1])|32 :
			       a[2] ? myffs(a[2])|64 :
			       65;
		}
		int and_frs(const U96 &s)
		{
			return a[2]&s.a[2] ? myfrs(a[2]&s.a[2])|64 :
			       a[1]&s.a[1] ? myfrs(a[1]&s.a[1])|32 :
			       a[0]&s.a[0] ? myfrs(a[0]&s.a[0])    :
			       -1;
		}
		int frs()
		{
			return a[2] ? myfrs(a[2])|64 :
			       a[1] ? myfrs(a[1])|32 :
			       a[0] ? myfrs(a[0])    :
			       -1;
		}
#endif
		void setbit(int i){a[i>>5]|=(1UL<<(i&31));}
		void clearbit(int i){a[i>>5]&=~(1UL<<(i&31));}
		void changebit(int i){a[i>>5]^=(1UL<<(i&31));}
		int bit(int i){	return a[i>>5] & (1UL<< (i&31) );}
		bool is2power()
		{
			if(a[0])
				return (!a[1]) && (!a[2]) && !(a[0]&(a[0]-1));
			else if(a[1])
				return  (!a[2]) && !(a[1]&(a[1]-1));
			else if(a[2]) return !(a[2]&(a[2]-1));
			else return 0;
		}
		bool and_is2power(const U96 &s)
		{
			register ull t0;
			register unsigned t2;
			t2=a[2]&s.a[2];
			t0=b[0]&s.b[0];
			if(t0)
				return !(t2 || (t0&(t0-1)));
			else if(t2) return !(t2&(t2-1));
			else return 0;
		}
	
};
