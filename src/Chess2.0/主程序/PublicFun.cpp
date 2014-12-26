#include "stdafx.h"
#include "Define.h"

//sort函数对CHESSMOVE排序时用到
bool UDgreater(CHESSMOVE cm1, CHESSMOVE cm2)
{
	return cm1.iScore>cm2.iScore;
}