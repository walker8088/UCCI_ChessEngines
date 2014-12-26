// TranspositionTable.h: interface for the CTranspositionTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSPOSITIONTABLE_H__C6292E32_89E6_46D7_8A85_E335AD8C4BBA__INCLUDED_)
#define AFX_TRANSPOSITIONTABLE_H__C6292E32_89E6_46D7_8A85_E335AD8C4BBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Define.h"

//定义了枚举型的数据类型，精确，下边界，上边界
enum ENTRY_TYPE{Exact,LowerBound,UpperBound};

//哈希表中元素的结构定义
struct HASHITEM
{
	LONGLONG checksum;	  //64位校验码
	ENTRY_TYPE entry_type;//数据类型
	short depth;		  //取得此值时的层次
	short eval;			  //节点的值
};

//置换表类
class CTranspositionTable  
{
public:
	CTranspositionTable();
	virtual ~CTranspositionTable();

public:
	void EnterHashTable(ENTRY_TYPE entry_type,short eval,short iDepth,int TableNo);
																    //将当前节点的值存入哈希表
	int LookUpHashTable(int alpha, int beta, int depth,int TableNo);//查询哈希表中当前节点数据
	void Hash_UnMakeMove(CHESSMOVE* move,BYTE nChessID,BYTE CurPosition[10][9]);  
																	//撤销所给走法的哈希值，还原成走过之前的
	void Hash_MakeMove(CHESSMOVE* move,BYTE CurPosition[10][9]);	//根据所给走法，增量生成新的哈希值
	void CalculateInitHashKey(BYTE CurPosition[10][9]);				//计算给定棋盘的哈希值
	void InitializeHashKey();										//初始化随机数组，创建哈希表

protected:
	LONGLONG Rand64();//产生64位的随机数
	LONG Rand32();	  //产生32位的随机数

public:	
	UINT m_nHashKey32[15][10][9];	   //32位随机数组，用以生成32位哈希值
	ULONGLONG m_ulHashKey64[15][10][9];//64位随机数组，用以生成64位哈希值
	HASHITEM *m_pTT[10];			   //置换表头指针
	UINT m_HashKey32;				   //32位哈希值
	LONGLONG m_HashKey64;			   //64位哈希值
};

#endif // !defined(AFX_TRANSPOSITIONTABLE_H__C6292E32_89E6_46D7_8A85_E335AD8C4BBA__INCLUDED_)
