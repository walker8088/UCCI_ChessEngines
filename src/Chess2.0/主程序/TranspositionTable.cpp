// TranspositionTable.cpp: implementation of the CTranspositionTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TranspositionTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTranspositionTable::CTranspositionTable()
{
	InitializeHashKey();//建立哈希表，创建随机数组
}

CTranspositionTable::~CTranspositionTable()
{
	//释放哈希表所用空间
	delete m_pTT[0];
	delete m_pTT[1];
}

void CTranspositionTable::CalculateInitHashKey(BYTE CurPosition[][9])
{
	int j,k,nChessType;

	m_HashKey32=0;
	m_HashKey32=0;
	//将所有棋子对应的哈希数加总
	for(j=0;j<10;j++)
		for(k=0;k<9;k++)
		{
			nChessType=CurPosition[j][k];
			if(nChessType!=NOCHESS)
			{
				m_HashKey32=m_HashKey32^m_nHashKey32[nChessType][j][k]; 
				m_HashKey64=m_HashKey64^m_ulHashKey64[nChessType][j][k]; 
			}
		}
}

void CTranspositionTable::Hash_MakeMove(CHESSMOVE *move, BYTE CurPosition[][9])
{
	BYTE nToID,nFromID;

	nFromID=CurPosition[move->From.y][move->From.x];
	nToID=CurPosition[move->To.y][move->To.x];

	//将要移动的棋子从中去除
	m_HashKey32=m_HashKey32^m_nHashKey32[nFromID][move->From.y][move->From.x]; 
	m_HashKey64=m_HashKey64^m_ulHashKey64[nFromID][move->From.y][move->From.x]; 

	if(nToID!=NOCHESS)//目标位置有棋子，也要从中去除
	{
		m_HashKey32=m_HashKey32^m_nHashKey32[nToID][move->To.y][move->To.x]; 
		m_HashKey64=m_HashKey64^m_ulHashKey64[nToID][move->To.y][move->To.x]; 
	}
	
	//将移动的棋子在目标位置的随机数添入
	m_HashKey32=m_HashKey32^m_nHashKey32[nFromID][move->To.y][move->To.x]; 
	m_HashKey64=m_HashKey64^m_ulHashKey64[nFromID][move->To.y][move->To.x];
}

void CTranspositionTable::Hash_UnMakeMove(CHESSMOVE *move, BYTE nChessID, BYTE CurPosition[][9])
{
	BYTE nToID;

	nToID=CurPosition[move->To.y][move->To.x];
	
	//将移动棋子在移动前位置上的随机数添入
	m_HashKey32=m_HashKey32^m_nHashKey32[nToID][move->From.y][move->From.x]; 
	m_HashKey64=m_HashKey64^m_ulHashKey64[nToID][move->From.y][move->From.x]; 

	//将移动棋子在现位置上的随机数从哈希值当中去除
	m_HashKey32=m_HashKey32^m_nHashKey32[nToID][move->To.y][move->To.x]; 
	m_HashKey64=m_HashKey64^m_ulHashKey64[nToID][move->To.y][move->To.x]; 
	
	if(nChessID)
	{
		//将被吃掉的棋子所对应的随机数恢复进哈希值
		m_HashKey32=m_HashKey32^m_nHashKey32[nChessID][move->To.y][move->To.x]; 
		m_HashKey64=m_HashKey64^m_ulHashKey64[nChessID][move->To.y][move->To.x]; 
	}
}

int CTranspositionTable::LookUpHashTable(int alpha, int beta, int depth, int TableNo)
{
	int x;
	HASHITEM* pht;

	//计算二十位哈希地址，如果读者设定的哈希表大小不是1M*2的,
	//而是TableSize*2，TableSize为读者设定的大小
	//则需要修改这一句为m_HashKey32%TableSize
	//下一个函数中这一句也一样
	x=m_HashKey32 & 0xFFFFF;
	pht=&m_pTT[TableNo][x];//取到具体的表项指针

    if(pht->depth>=depth && pht->checksum==m_HashKey64)
	{
		switch(pht->entry_type)//判断数据类型
		{
		case Exact://确切值
			return pht->eval;

		case LowerBound://下边界
			if(pht->eval>=beta)
				return pht->eval;

			break;

		case UpperBound://上边界
			if (pht->eval<=alpha)
				return pht->eval;

			break;
        }
	}

	return 66666;
}

void CTranspositionTable::EnterHashTable(ENTRY_TYPE entry_type, short eval, short depth, int TableNo)
{
	int x;
	HASHITEM* pht;

	x=m_HashKey32 & 0xFFFFF;//计算二十位哈希地址
	pht=&m_pTT[TableNo][x]; //取到具体的表项指针

	//将数据写入哈希表
	pht->checksum=m_HashKey64; //64位校验码
	pht->entry_type=entry_type;//表项类型
	pht->eval=eval;			   //要保存的值
	pht->depth=depth;		   //层次
}

void CTranspositionTable::InitializeHashKey()
{
	int i,j,k;
	
	srand((unsigned)time(NULL));
	
	//填充随机数组
	for(i=0;i<15;i++)
		for(j=0;j<10;j++)
			for(k=0;k<9;k++)
			{
				m_nHashKey32[i][j][k]=Rand32();
				m_ulHashKey64[i][j][k]=Rand64();
			}

	//申请置换表所用空间。1M*2个条目，读者也可指定其他大小
	m_pTT[0]=new HASHITEM[1024*1024];//用于存放取极大值的节点数据
	m_pTT[1]=new HASHITEM[1024*1024];//用于存放取极小值的节点数据
}

//生成64位随机数
LONGLONG CTranspositionTable::Rand64()
{
    return rand()^((LONGLONG)rand()<<15)^((LONGLONG)rand()<<30)^
		((LONGLONG)rand()<<45)^((LONGLONG)rand()<<60);
}

//生成32位随机数
LONG CTranspositionTable::Rand32()
{
    return rand()^((LONG)rand()<<15)^((LONG)rand()<<30);
}