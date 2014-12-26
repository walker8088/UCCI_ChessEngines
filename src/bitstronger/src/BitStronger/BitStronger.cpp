/*! \file BitStronger.cpp
@brief 主程序（main函数所在） 
*/
 
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../utility/bool.h"
#include "ucci.h"
#include "constant.h"
#include "board.h"
#include "searcher.h"
#include "hashnum.h"
#include "openbook.h"

#ifdef _WIN32

#include <windows.h>

char workpath[256], hashpath[256], bookpath[256];
///操作系统的路径分隔符 
const int BACK_SLASH_CHAR = '\\';
///得到引擎工作路径 
void getpath()
{
    GetModuleFileName(NULL, workpath, sizeof(workpath) - 1);
    *(strrchr(workpath, BACK_SLASH_CHAR) + 1) = 0;
    strcpy(hashpath, workpath);
    strcat(hashpath, "Zobrist.bin");
    strcpy(bookpath, workpath);
    strcat(bookpath, "OpenBook.bin");
}

#else

char workpath[256], hashpath[256], bookpath[256];
///操作系统的路径分隔符 
const int BACK_SLASH_CHAR = '/';
///得到引擎工作路径 
void getpath()
{
    workpath[0] = '.';
    workpath[1] = BACK_SLASH_CHAR;
    workpath[2] = '\0';
    strcpy(hashpath, workpath);
    strcat(hashpath, "Zobrist.bin");
    strcpy(bookpath, workpath);
    strcat(bookpath, "OpenBook.bin");
}

#endif

///主控函数
/*!
　　主控模块维护着一个全局唯一的当面局面实例。它的核心是消息循环处理机制，通过调用UCCI通讯模块实时地监听界面程序的各种指令，依指令要求调用引擎各个模块协调工作，然后将引擎的反馈输出给界面程序。
*/ 
int main(int argc, char *argv[])
{
    //UCCI指令类型 
    UcciCommEnum uce;
    //UCCI指令内容 
    UcciCommStruct ucs;
    //当前局面 
    Board curboard;
    
    getpath();
        
    if (BootLine() == UCCI_COMM_UCCI)
    {
        printf("id name BitStronger 0.06\n");
        fflush(stdout);
        printf("id copyright 2006 B.I.T.\n");
        fflush(stdout);
        printf("id author BitStronger Authors\n");
        fflush(stdout);
        printf("id user Our Users\n");
        fflush(stdout);
        printf("ucciok\n");
        fflush(stdout);
        
        Hashnum::InitHash();
        OpenBook::InitBook();

        // 以下是接收指令和提供对策的循环体
        uce = UCCI_COMM_NONE;
        while (uce != UCCI_COMM_QUIT)
        {
            uce = IdleLine(ucs, TRUE);
            switch (uce)
            {
            case UCCI_COMM_ISREADY:
                printf("readyok\n");
                fflush(stdout);
                break;
            case UCCI_COMM_STOP:
                printf("nobestmove\n");
                fflush(stdout);
                break;
            case UCCI_COMM_POSITION:
                curboard.FromFen(ucs.Position.szFenStr, ucs.Position.nMoveNum, ucs.Position.lpdwCoordList);
                break;
            case UCCI_COMM_BANMOVES:
                //Nothing
                break;
            case UCCI_COMM_SETOPTION:
                //Nothing
                break;
            case UCCI_COMM_GO:
            case UCCI_COMM_GOPONDER:
                Searcher::GetBestmove(curboard, curboard.player);
                break;
            }
        }
    }
    printf("bye\n");
    fflush(stdout);
    
    Hashnum::ClearHash();
    OpenBook::ClearBook();
    
    return 0;
}
