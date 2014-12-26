
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
#include ".\print_info.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "move.h"
#include "search.h"

static const char ChessName[14][4] = 
{
	"帥","仕","相","傌","俥","炮","兵", 
    "將","士","象","馬","車","包","卒"
};

static const char PostionName[2][9][4] = 
{ 
	{"九","八","七","六","五","四","三","二","一"},
	{"１","２","３","４","５","６","７","８","９"}
};
/*
static const char ChessName32[32][4] = 
{
	"帅","仕","相","马","车","炮","兵", 
    "将","士","象","马","车","炮","卒"
};*/

char* get_step_name(int move)
{
	int from,to;
	int piece,capture;
	int r0,f0,r1,f1;
	int me,opp,pos;
	
	static char step_name[12];	// 必须用静态变量，否则不能返回

    from = MOVE_FROM(move) ;
	to = MOVE_TO(move);


	piece = Square[from] & (~31);
	capture =Square[to] & (~31);

    r0 = (from >> 4) - 3;  //行
    f0 = (from & 0xf) - 3; //列
   
	r1 = (to >> 4) - 3;
    f1 = (to & 0xf) - 3;
    
    me=PIECE_COLOUR(piece);
	opp=COLOUR_OPP(me);

	pos=Square[from] & 31;

	pos=PieceTo32[pos];

	strcpy( step_name, ChessName[pos] );
	strcat( step_name, PostionName[me][f0] );
	
	//检查此列x0是否存在另一颗成对的棋子.
	int y,chess; //y:行
	for(y=0;y<10;y++)
	{
		chess = Square[(y+3)*16+f0+3]; 

		if(chess==0)											// 无子继续搜索
			continue;

        chess= chess & ~31;

		if(PIECE_IS_ADVISOR(piece) || PIECE_IS_BISHOP(piece))		// 象、士不用区分
			continue;
		if(y==r0)												// 是同一颗棋子, 不必搜索.
			continue;

		if(piece==chess)
		{
			if(me)			// 黑子
			{
				if(r0>y)
					strcpy( step_name, "前" );
				else
					strcpy( step_name, "后" );
			}
			else				// 红子
			{
				if(r0>y)
					strcpy( step_name, "后" );
				else
					strcpy( step_name, "前" );
			}

			strcat( step_name, ChessName[pos] );
			break;
		}
	}

	//int piece = PieceTypes[piece]-7*Player;

	//进, 退, 平
	if(r0==r1)
	{
		strcat( step_name, "平" );
		strcat( step_name, PostionName[me][f1]);					// 平，任何棋子都以绝对位置表示
	}
	else if((me && r0<r1) || (!me && r0>r1))
	{
		strcat( step_name, "进" );

		if(PIECE_IS_ADVISOR(piece) || PIECE_IS_BISHOP(piece) ||PIECE_IS_KNIGHT(piece))						// 马、象、士用绝对位置表示
			strcat( step_name, PostionName[me][f1] );			
		else if(me)												    // 将、车、炮、兵用相对位置表示
			strcat( step_name, PostionName[1][r1-r0-1] );			// 黑方
		else
			strcat( step_name, PostionName[0][9-r0+r1] );			// 红方
	}
	else
	{
		strcat( step_name, "退" );

		if(PIECE_IS_ADVISOR(piece) || PIECE_IS_BISHOP(piece) ||PIECE_IS_KNIGHT(piece))						// 马、象、士用绝对位置表示
			strcat( step_name, PostionName[me][f1] );			
		else if(me)												    // 将、车、炮、兵用相对位置表示
			strcat( step_name, PostionName[1][r0-r1-1] );			// 红方
		else
			strcat( step_name, PostionName[0][9-r1+r0] );			// 黑方		
	}

	return(step_name);
}


void print_board()
{
	int sq,pos,piece;
	uint16 move;
	//char f,t;

	printf("------------print-board-v0.3---------------\n");
   
	for(sq=0;sq<90;++sq)
	{
		move=SQUARE_FROM_90(sq);
		//f=(move & 0xf) + 'a' - 3;
		//t='9' + 3-(move>>4);
		printf("%x ",move);
		if( (((sq+1)%9)==0))
		{
			printf("\n");
		}
	}
	 printf("a  b  c  d  e  f  g  h  i\n");
    printf("\n");
	
	for(sq=0;sq<90;++sq)
	{

		piece=Square[SQUARE_FROM_90(sq)];// & 31;
		pos=Square[SQUARE_FROM_90(sq)] & 31;
		if(piece==0)
		{
			printf("0  ");
		}else
		{
            printf("%s ",ChessName[PieceTo32[pos]]);
		}
		//printf("%x=%.3d ",SQUARE_FROM_90(sq),Square[SQUARE_FROM_90(sq)]);
		if( (((sq+1)%9)==0))
		printf("\n");
	}
	printf("\n");
	/*
	for(sq=0;sq<90;++sq)
	{
		
		printf("%.3x ",Square[SQUARE_FROM_90(sq)]>>7);
		if( (((sq+1)%9)==0))
		printf("\n");
	}*//*
    for(sq=0;sq<32;++sq)
	{
		printf("%.2x,",Piece[sq]);
		if( (((sq+1)%16)==0))
		printf("\n");

	}
	printf("Key=%d\n",Key);
	printf("Lock=%d\n",Lock);
	*/
}



void print_list(list_t* list)
{
	int sq,move_str,size;
	//board_t bb[1];

	
    size=list->size;

	//
    //size=5;
	printf("-----------print-list-v0.1---------------\n");
	for(sq=0;sq<size;sq++)
	{	

		move_str=move_to_string(list->move[sq]);
		printf("%x=%.4s=%s  value=%d",list->move[sq],(const char *) &move_str,get_step_name(list->move[sq]),list->value[sq]);//);
		printf("\n");
	}
    
	printf("size=%d\n",list->size);

}
void print_move(int move)
{
	int move_str;
	move_str=move_to_string(move);
	printf("%.4s ",(const char *)&move_str);

}

void print_cn_move(int move)
{
	int move_str;
	move_str=move_to_string(move);
	printf("%.4s=%s ",(const char *)&move_str,get_step_name(move));
}
void save_file(char * file_name)
{
	int sq,pos,piece;
//	uint16 move;
	int move_str;
	mv_t * move_ptr;

	FILE *out = fopen(file_name, "w+");

	fprintf(out, "-------------------搜索信息-------------------\n\n");
    //fprintf(out,"搜索深度 %d\n",SearchCurrent->max_depth);
    //fprintf(out,"seldepth %d time %.0f nodes " S64_FORMAT " nps %.0f",SearchCurrent->max_depth,SearchCurrent->time*1000.0,SearchCurrent->node_nb,SearchCurrent->speed);
	//fprintf(out,"node_qs_n=%d",SearchCurrent->node_qs_nb);

	fprintf(out,"------------print-board-v0.3---------------\n");
   
	for(sq=0;sq<90;++sq)
	{

		piece=Square[SQUARE_FROM_90(sq)];// & 31;
		pos=Square[SQUARE_FROM_90(sq)] & 31;
		if(piece==0)
		{
			fprintf(out,"0  ");
		}else
		{
            fprintf(out,"%s ",ChessName[PieceTo32[pos]]);
		}
		if( (((sq+1)%9)==0))
		fprintf(out,"\n");
	}
	fprintf(out,"\n");

	fprintf(out,"info current depth %d seldepth %d time %.0f nodes " S64_FORMAT " nps %.0f\n",SearchBest->depth,SearchCurrent->max_depth,SearchCurrent->time*1000.0,SearchCurrent->node_nb,SearchCurrent->speed);
    fprintf(out,"----------主要变例输出-----------\n");
	
	move_ptr=SearchBest->pv;
	while (*move_ptr!= MoveNone)
	{
		  move_str=move_to_string(*move_ptr);
		  fprintf(out, " %.4s", (const char *)&move_str);
	      move_ptr++;
	 }
	fprintf(out,"\n");


	fclose(out);
}