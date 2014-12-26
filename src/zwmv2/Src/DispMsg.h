#ifndef _DISPMSG_H_
#define _DISPMSG_H_

//ÏÔÊ¾ÏûÏ¢
#define DISPMSG1(f, m1)								f << m1;
#define DISPMSG2(f, m1, m2)							f << m1 << m2;
#define DISPMSG3(f, m1, m2, m3)						f << m1 << m2 << m3;
#define DISPMSG4(f, m1, m2, m3, m4)					f << m1 << m2 << m3 << m4;
#define DISPMSG5(f, m1, m2, m3, m4, m5)				f << m1 << m2 << m3 << m4 << m5;
#define DISPMSG6(f, m1, m2, m3, m4, m5, m6)			f << m1 << m2 << m3 << m4 << m5 << m6;
#define DISPMSG7(f, m1, m2, m3, m4, m5, m6, m7)		f << m1 << m2 << m3 << m4 << m5 << m6 << m7;

#define DISPMSG0_ENDL(f)											f << endl;
#define DISPMSG1_ENDL(f, m1)										f << m1 << endl;
#define DISPMSG2_ENDL(f, m1, m2)									f << m1 << m2 << endl;
#define DISPMSG3_ENDL(f, m1, m2, m3)								f << m1 << m2 << m3 << endl;
#define DISPMSG4_ENDL(f, m1, m2, m3, m4)							f << m1 << m2 << m3 << m4 << endl;
#define DISPMSG5_ENDL(f, m1, m2, m3, m4, m5)						f << m1 << m2 << m3 << m4 << m5 << endl;
#define DISPMSG6_ENDL(f, m1, m2, m3, m4, m5, m6)					f << m1 << m2 << m3 << m4 << m5 << m6 << endl;
#define DISPMSG7_ENDL(f, m1, m2, m3, m4, m5, m6, m7)				f << m1 << m2 << m3 << m4 << m5 << m6 << m7 << endl;
#define DISPMSG8_ENDL(f, m1, m2, m3, m4, m5, m6, m7, m8)			f << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8 << endl;
#define DISPMSG9_ENDL(f, m1, m2, m3, m4, m5, m6, m7, m8, m9)		f << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8 << m9 << endl;

#define DISPMSG10_ENDL(f, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10)			f << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8 << m9 << m10 << endl;
#define DISPMSG11_ENDL(f, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11)		f << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8 << m9 << m10 << m11 << endl;

#endif