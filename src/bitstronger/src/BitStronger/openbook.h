/*! \file openbook.h
@brief 开局库操作类（单件模块）
*/

#ifndef OPENBOOK_H
#define OPENBOOK_H

#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>
#include <vector>
#include "../utility/bool.h"
#include "constant.h"
#include "board.h"

///C++ std命名空间
/*!在这里用来引入 vector 类。 
*/ 
using namespace std;

///开局库条目结构 
typedef struct BookItem BookItem;

///引擎工作路径 
extern char workpath[256];
///保存开局库的文件所在路径 
extern char bookpath[256];

///是否已经加载？不重复加载 
static Bool loadedopen = FALSE;
///保存开局库的文件
static FILE * bookfile;

///内存中记录开局库的向量 
static vector<BookItem> * books;

///开局库条目结构 
/*!
　　开局库条目记录一个局面对应的多个着法，用long型存储起点到终点的4个字节（如“h0i2”）。
*/
struct BookItem
{
    ///局面的 Zobrist 数 
    HASHNUM zobrist;
    ///这一局面轮到谁走棋了？ 
    PLAYER player;
    ///该局面对应的多个着法
    vector<long> * tricks;
};

///开局库操作类（单件模块）
/*!
　　程序在开局或一些典型的中局时，先不进行博弈树搜索，而是在开局库中检索典型的应对着法。这样使避免了博弈树搜索方法在开局时因为局部很小的利益而忽视全局的发展，走出贻笑大方的局面的情况。便于我方快速出动子力，占据有利位置。

　　我们的开局库的来源是《象棋百科全书》网站上收录的8000多局全国象棋大赛棋谱，使用该网站提供的小程序将棋谱中的开局提取，按出现的频率和最终的胜率排序，然后使用自己编写的小程序将上面总结出来的文本格式的开局信息转为我们程序专用的二进制格式开局库文件（OpenBook.bin）。开局库文件中每13个字节为一段，分别用8个字节表示局面（哈希表示法），1个字节表示轮到谁下棋（0x00为红，0x01为黑），4个字节表示应对的着法（用代表起止位置的棋盘坐标字符串表示，如"h0i2"）。
*/
class OpenBook
{
public:
    ///初始化开局库 
    static void InitBook();
    ///清除开局库 
    static void ClearBook();
    ///在开局库中搜索某一局面，返回对应着法数目，随机选择一个着法写入str 
    static int Search(const Board & cur, char * str);
};

#endif
