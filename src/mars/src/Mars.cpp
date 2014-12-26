/********************************************************************

	----------------------------------------------------------------
    软件许可证 － GPL
	版权所有 (C) 2005-2008 极速人工智能研究
	----------------------------------------------------------------
	这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用公共许
	可证条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根
	据你的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用公共许可证。

    你应该已经和程序一起收到一份GNU通用公共许可证的副本（本目录
	GPL.txt文件）。如果还没有，写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA
	----------------------------------------------------------------
	如果你在使用本软件时有什么问题或建议，请用以下地址与我们取得联系：

			http://www.jsmaster.com

	或发信到：

			jschess##163.com
	----------------------------------------------------------------
	本文件用途：	略
	
	  
	本文件编写人：	
				顾剑辉			jschess##163.com
		
	本文件版本：	03
	最后修改于：	2006-1-16
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		  2006-1		第一版发布

********************************************************************/
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ucci.h"
#include "pregen.h"
#include "board.h"
#include "fen.h"
#include "list.h"
#include "move_gen.h"
#include "move.h"
#include "attack.h"
#include "move_do.h"
#include "piece.h"
#include "square.h"
#include "move_evasion.h"
#include "move_check.h"
#include "search.h"
#include "sort.h"
#include "trans.h"
#include "material.h"
#include "print_info.h"
#include <time.h>

//board_t g_board[1];
list_t  g_list[1];
sort_t  g_sort[1];
undo_t  g_undo[1];
//attack_t g_attack[1];

//PieceTo32

FILE * OutFile;

int main(int argc, char * argv[])
{
	const char *BoolValue[2] = { "false", "true" };
    printf("Mars V0.3 UCCI based on Fruit 2.1 by Fabien Letouzey\n");
	printf("作者：顾剑辉\n");
    printf("网站：www.jsmaster.com\n");
	printf("请键入ucci指令......\n");
    //position fen rnbakab2/9/2c1c1n2/p1p1p3p/6p2/2PN3r1/P3P1P1P/1C2C1N2/9/1RBAKAB1R w - - 0 7   
	//position fen 1rbakab2/9/1cn5n/pR4p1p/2p1p4/4c1P2/P1P4rP/2N1C1NC1/4A4/2B1KABR1 w - - 0 10
	//9/3ka4/3R5/5r2p/p5p2/4N4/P5P1P/9/4A2c1/2BK2B2 b - - 0 36
	CommEnum IdleComm;
	CommDetail Command;
	int move,n;
	undo_t undo[1];
 
	if(BootLine() == e_CommUcci)
	{

		zobrist_gen();
	    pre_move_gen();
	    trans_init(Trans);
	    trans_alloc(Trans);
        material_init();
        material_alloc(); 
		OutFile = stdout;
        board_from_fen("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r - - 0 1");
		//board_from_fen("3k5/9/9/9/9/4c4/9/9/4n4/4K4 r - - 0 1");
		//board_from_fen("3k5/9/9/9/9/4c4/3C5/2n1n4/4K4/9 r - - 0 1");
		printf("position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r - - 0 1\n\n");
		print_board();
		printf("\n");
		printf("id name Mars v0.3\n");
		fflush(stdout);
		printf("id copyright 版权所有(C) 2005-2008\n");
		fflush(stdout);
		printf("id author 顾剑辉\n");
		fflush(stdout);
		printf("id user 未知\n\n");
		fflush(stdout);
/*
				printf("option batch type check default %s\n", BoolValue[ThisSearch.bBatch]);
		fflush(stdout);

		// option debug 让引擎输出详细的搜索信息，并非真正的调试模式。
		printf("option debug type check default %s\n", BoolValue[ThisSearch.Debug]);
		fflush(stdout);

		// 指定开局库文件的名称，可指定多个开局库文件，用分号“;”隔开，如不让引擎使用开局库，可以把值设成空
		ThisSearch.bUseOpeningBook = ThisSearch.m_Hash.LoadBook(BookFile);
		if(ThisSearch.bUseOpeningBook)
			printf("option bookfiles type string default %s\n", BookFile);
		else
			printf("option bookfiles type string default %s\n", 0);
		fflush(stdout);

		// 残局库名称
		printf("option egtbpaths type string default null\n");
		fflush(stdout);

		// 显示Hash表的大小
		printf("option hashsize type spin default %d MB\n", ThisSearch.m_Hash.nHashSize*2*sizeof(CHashRecord)/1024/1024);
		fflush(stdout);

		// 引擎的线程数
		printf("option threads type spin default %d\n", 0);
		fflush(stdout);

		// 引擎达到自然限着的半回合数
		printf("option drawmoves type spin default %d\n", ThisSearch.NaturalBouts);
		fflush(stdout);

		// 棋规
		printf("option repetition type spin default %d 1999年版《中国象棋竞赛规则》\n", e_RepetitionChineseRule);
		fflush(stdout);

		// 空着裁减是否打开
		printf("option pruning type check %d\n", ThisSearch);
		fflush(stdout);

		// 估值函数的使用情况
		printf("option knowledge type check %d\n", ThisSearch);
		fflush(stdout);

		// 指定选择性系数，通常有0,1,2,3四个级别。给估值函数加减一定范围内的随机数，让引擎每次走出不相同的棋。
		printf("option selectivity type spin min 0 max 3 default %d\n", ThisSearch.nSelectivity);
		fflush(stdout);

		// 指定下棋的风格，通常有solid(保守)、normal(均衡)和risky(冒进)三种
		printf("option style type combo var solid var normal var risky default %s\n", ChessStyle[ThisSearch.nStyle]);
		fflush(stdout);		
*/
		// copyprotection 显示版权检查信息(正在检查，版权信息正确或版权信息错误)。 
		printf("copyprotection ok\n\n");
		fflush(stdout);

		// ucciok 这是ucci指令的最后一条反馈信息，表示引擎已经进入用UCCI协议通讯的状态。
		printf("ucciok\n\n");
		fflush(stdout);
        
		//FILE * out=fopen("info.txt","w+");
		do 
		{
			IdleComm = IdleLine(Command, 0);
			switch (IdleComm) 
			{
				// isready 检测引擎是否处于就绪状态，其反馈信息总是readyok，该指令仅仅用来检测引擎的“指令接收缓冲区”是否能正常容纳指令。
				// readyok 表明引擎处于就绪状态(即可接收指令的状态)，不管引擎处于空闲状态还是思考状态。
				case e_CommIsReady:
					//fprintf(out,"readyok\n");
					printf("readyok\n");
					fflush(stdout);
					break;

				// stop 中断引擎的思考，强制出着。后台思考没有命中时，就用该指令来中止思考，然后重新输入局面。
				case e_CommStop:
					//ThisSearch.bStopThinking = 1;
					printf("nobestmove\n");
					printf("score 0\n");
					fflush(stdout);
					break;

				// position fen 设置“内置棋盘”的局面，用fen来指定FEN格式串，moves后面跟的是随后走过的着法
				case e_CommPosition:
					// 将界面传来的Fen串转化为棋局信息
					// board_clear();
					//fprintf(out,"%s\n",Command.Position.FenStr);
                    //fprintf(out,"%d",Command.Position.MoveNum);
					//fprintf(out,"%s\n",Command.Position.CoordList);
					 //trans_clear(Trans);
					 board_from_fen(Command.Position.FenStr);
					 print_board();
					 for(n=0; n<Command.Position.MoveNum; n++)
					{
						move = move_from_string(Command.Position.CoordList[n]);
						//fprintf(out,"%x ",move);
						if( !move )
							break;

						move_do(move,undo);
						//ThisSearch.StepRecords[ThisSearch.nCurrentStep-1] |= ThisSearch.Checking(ThisSearch.Player) << 24;
					}
					// 将局面走到当前，主要是为了更新着法记录，用于循环检测。
					break;

				// banmoves 为当前局面设置禁手，以解决引擎无法处理的长打问题。当出现长打局面时，棋手可以操控界面向引擎发出禁手指令。
				case e_CommBanMoves:
					break;

				// setoption 设置引擎各种参数
				case e_CommSetOption:
					switch(Command.Option.Type) 
					{
						// setoption batch %d
						case e_OptionBatch:
							batch=(Command.Option.Value.Check == e_CheckTrue);
							printf("option batch type check default %s\n", BoolValue[batch]);
							fflush(stdout);
							break;

						// setoption debug %d 让引擎输出详细的搜索信息，并非真正的调试模式。
						case e_OptionDebug:
							
							break;

						// setoption bookfiles %s  指定开局库文件的名称，可指定多个开局库文件，用分号“;”隔开，如不让引擎使用开局库，可以把值设成空
						case e_OptionBookFiles:
							
							break;

						// setoption egtbpaths %s  指定残局库文件的名称，可指定多个残局库路径，用分号“;”隔开，如不让引擎使用残局库，可以把值设成空
						case e_OptionEgtbPaths:
							// 引擎目前不支持开局库
							
							break;

						// setoption hashsize %d  以MB为单位规定Hash表的大小，-1表示让引擎自动分配Hash表。1～1024MB
						// 象堡界面有个Bug，每次设置引擎时，这个命令应在开局库的前面
						case e_OptionHashSize:
							// -1MB(自动), 0MB(自动), 1MB(16), 2MB(17), 4MB(18), 8MB(19), 16MB(20), 32MB(21), 64MB(22), 128MB(23), 256MB(24), 512MB(25), 1024MB(26)
							if( Command.Option.Value.Spin <= 0)
								n = 22;		// 缺省情况下，引擎自动分配(1<<22)*16=64MB，红与黑两各表，双方各一半。
							else
							{
								n = 15;											// 0.5 MB = 512 KB 以此为基数
								while( Command.Option.Value.Spin > 0 )
								{
									Command.Option.Value.Spin >>= 1;			// 每次除以2，直到为0
									n ++;
								}
							}								

							break;

						// setoption threads %d	      引擎的线程数，为多处理器并行运算服务
						case e_OptionThreads:
							// ThisSearch.nThreads = Command.Option.Value.Spin;		// 0(auto),1,2,4,8,16,32
							printf("option drawmoves type spin default %d\n", 0);
							fflush(stdout);
							break;

						// setoption drawmoves %d	  达到自然限着的回合数:50,60,70,80,90,100，象堡已经自动转化为半回合数
						case e_OptionDrawMoves:
							
							break;

						// setoption repetition %d	  处理循环的棋规，目前只支持“中国象棋棋规1999”
						case e_OptionRepetition:
							// ThisSearch.nRepetitionStyle = Command.Option.Value.Repetition;
							// e_RepetitionAlwaysDraw  不变作和
							// e_RepetitionCheckBan    禁止长将
							// e_RepetitionAsianRule   亚洲规则
							// e_RepetitionChineseRule 中国规则（缺省）
							
							break;

						// setoption pruning %d，“空着向前裁剪”是否打开
						case e_OptionPruning:
							//ThisSearch.bPruning = Command.Option.Value.Scale;
							//printf("option pruning type check %d\n", ThisSearch);
							//fflush(stdout);
							break;

						// setoption knowledge %d，估值函数的使用
						case e_OptionKnowledge:
							//ThisSearch.bKnowledge = Command.Option.Value.Scale;
							//printf("option knowledge type check %d\n", ThisSearch);
							//fflush(stdout);
							break;

						// setoption selectivity %d  指定选择性系数，通常有0,1,2,3四个级别
						case e_OptionSelectivity:
							/*switch (Command.Option.Value.Scale)
							{
								case e_ScaleNone:
									ThisSearch.SelectMask = 0;
									break;
								case e_ScaleSmall:
									ThisSearch.SelectMask = 1;
									break;
								case e_ScaleMedium:
									ThisSearch.SelectMask = 3;
									break;
								case e_ScaleLarge:
									ThisSearch.SelectMask = 7;
									break;
								default:
									ThisSearch.SelectMask = 0;
									break;
							}
							printf("option selectivity type spin min 0 max 3 default %d\n", ThisSearch.SelectMask);
							fflush(stdout);*/
							break;

						// setoption style %d  指定下棋的风格，通常有solid(保守)、normal(均衡)和risky(冒进)三种
						case e_OptionStyle:
							//ThisSearch.nStyle = Command.Option.Value.Style;
							//printf("option style type combo var solid var normal var risky default %s\n", ChessStyle[Command.Option.Value.Style]);
							//fflush(stdout);
							break;						

						// setoption loadbook  UCCI界面ElephantBoard在每次新建棋局时都会发送这条指令
						case e_OptionLoadBook:
							/*ThisSearch.m_Hash.ClearHashTable();
							ThisSearch.bUseOpeningBook = ThisSearch.m_Hash.LoadBook(BookFile);
							
							if(ThisSearch.bUseOpeningBook)
								printf("option loadbook succeed. %s\n", BookFile);		// 成功
							else
								printf("option loadbook failed! %s\n", "Not found file BOOK.DAT");				// 没有开局库
							fflush(stdout);
							printf("\n\n");
							fflush(stdout);
							*/
							break;

						default:
							break;
					}
					break;

				// Prepare timer strategy according to "go depth %d" or "go ponder depth %d" command
				case e_CommGo:
				case e_CommGoPonder:
					switch (Command.Search.Mode)
					{
						// 固定深度Command.Search.DepthTime.Depth
						case e_TimeDepth:
							ponder = 2;
							search(Command.Search.DepthTime.Depth,0,0);
							break;

						// 时段制： 分配时间 = 剩余时间 / 要走的步数
						case e_TimeMove:
							ponder = (IdleComm == e_CommGoPonder ? 1 : 0);
							search(32,Command.Search.DepthTime.Time * 1000 / Command.Search.TimeMode.MovesToGo, Command.Search.DepthTime.Time * 1000);
						//	ThisSearch.Ponder = (IdleComm == e_CommGoPonder ? 1 : 0);
						//	printf("%d\n", Command.Search.TimeMode.MovesToGo);
						//	ThisSearch.MainSearch(127, Command.Search.DepthTime.Time * 1000 / Command.Search.TimeMode.MovesToGo, Command.Search.DepthTime.Time * 1000);
							break;

						// 加时制： 分配时间 = 每步增加的时间 + 剩余时间 / 20 (即假设棋局会在20步内结束)
						case e_TimeInc:
                            ponder = (IdleComm == e_CommGoPonder ? 1 : 0);
							search(32,(Command.Search.DepthTime.Time + Command.Search.TimeMode.Increment * 20) * 1000 / 20, Command.Search.DepthTime.Time * 1000);
						 //	ThisSearch.Ponder = (IdleComm == e_CommGoPonder ? 1 : 0);
						 //	ThisSearch.MainSearch(127, (Command.Search.DepthTime.Time + Command.Search.TimeMode.Increment * 20) * 1000 / 20, Command.Search.DepthTime.Time * 1000);
							break;

						default:
							break;
					}
					break;
			}
		} while (IdleComm != e_CommQuit);
        //fprintf(out,"bye");
		//fclose(out);
		printf("bye\n");
		fflush(stdout);
		//getchar();
	}

	//getchar();
	return 0;
}