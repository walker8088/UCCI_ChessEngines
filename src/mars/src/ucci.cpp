/* 文件：ucci.cpp
 * 内容：ElephantEye源程序第1部分——UCCI指令解释模块
 * 标识符约定：
 * c_：常量；
 * e_：选项；
 * g_：着法预产生数组；
 * p_：局面(ChessPosition)的成员变量；
 * s_：静态(全局)变量。
 */

/* 首先是行输入程序和数字识别工具，UCCI指令和浅红象棋协议解释器都使用这些工具
 * 其中最复杂的是检查是否有行输入的"CheckInput()"函数，Windows和Unix系统要区别对待
 * 这个函数基本上拷贝了Crafty的源程序，其实大多数国际象棋的WinBoard引擎和UCI引擎都是这么做的，这里就不详细说明了
 */

#ifdef _WIN32

#include <windows.h>

int /* bool */ CheckInput(int &BytesLeft) {
  static int /* bool */ s_Init = false;
  static BOOL s_ConsoleMode;
  static HANDLE s_InputHandle;
  DWORD data;
  if (BytesLeft > 0) {
    return true;
  }
  if (!s_Init) {
    s_Init = true;
    s_InputHandle = GetStdHandle(STD_INPUT_HANDLE);
    s_ConsoleMode = GetConsoleMode(s_InputHandle, &data);
    if (s_ConsoleMode) {
      SetConsoleMode(s_InputHandle, data & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
      FlushConsoleInputBuffer(s_InputHandle);
    }
  }
  if (s_ConsoleMode) {
    GetNumberOfConsoleInputEvents(s_InputHandle, &data);
    return data > 1;
  } else {
    if (!PeekNamedPipe(s_InputHandle, NULL, 0, NULL, &data, NULL)) {
      return true;
    }
    BytesLeft = data;
    return data != 0;
  }
}

#else

#include <sys/select.h>
#include <time.h>
#include <unistd.h>

int /* bool */ CheckInput(int &) {
  fd_set readfds;
  timeval tv;
  int /* bool */ data;
  FD_ZERO(&readfds);
  FD_SET(0, &readfds);
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  select(16, &readfds, 0, 0, &tv);
  data = FD_ISSET(0, &readfds);
  return data;
}

#endif

#include <stdio.h>
#include <string.h>
#include "ucci.h"

// 这段代码是idle.h中的代码，直接挪到这里************
//#include "idle.h"
#ifdef _WIN32
  //#include <windows.h>
  inline void Idle(void) {
    Sleep(1);
  }
#else
  //#include <time.h>
  inline void Idle(void) {
    timespec tv;
    tv.tv_sec = 0;
    tv.tv_nsec = 1000000;
    nanosleep(&tv, NULL);
  }
#endif
// **************************************************

/* 读取一行的程序，利用"CheckInput()"函数，所以Windows和Unix的代码就一样了
 * 输入行读取到缓冲区中，缓冲区被保存在静态数组"s_LineStr"中，有行输入时总是返回"s_LineStr"，否则返回NULL
 * 还有一个静态变量是"s_BytesLeft"，其实只用在Windows中，作用原理参阅"CheckInput()"函数
 */
char *ReadInput(void) {
  const int c_MaxInputBuff = 1024;
  static char s_LineStr[c_MaxInputBuff];
  static int s_BytesLeft = 0;
  char *RetVal;
  if (CheckInput(s_BytesLeft)) {
    RetVal = fgets(s_LineStr, c_MaxInputBuff, stdin);
    if (RetVal != NULL) {
      if (s_BytesLeft > 0) {
        s_BytesLeft -= (int)strlen(RetVal);
      }
      RetVal = strchr(s_LineStr, '\n');
      *RetVal = '\0';
      RetVal = s_LineStr;
    }
    return RetVal;
  } else {
    return NULL;
  }
}

/* 读取某串字符中的数字，同时移动字符串的指针，并可限定数字大小
 * 原理非常简单，这里不作说明
 */
int ReadDigit(const char *&LineStr, int MaxValue) {
  int RetValue;
  RetValue = 0;
  while (1) {
    if (*LineStr >= '0' && *LineStr <= '9') {
      RetValue *= 10;
      RetValue += *LineStr - '0';
      LineStr ++;
      if (RetValue > MaxValue) {
        RetValue = MaxValue;
      }
    } else {
      break;
    }
  }
  return RetValue;
}

/* 然后是三个UCCI指令解释器
 * 其中第一个解释器"BootLine()"最简单，只用来接收引擎启动后的第一行指令
 * 输入"ucci"时就返回"e_CommUcci"，否则一律返回"e_CommNone"
 * 前两个解释器都等待是否有输入，如果没有输入则执行待机指令"Idle()"
 * 而第三个解释器("BusyLine()"，只用在引擎思考时)则在没有输入时直接返回"e_CommNone"
 */
CommEnum BootLine(void) {
  const char *LineStr;
  LineStr = ReadInput();
  while (LineStr == NULL) {
    Idle();
    LineStr = ReadInput();
  }
  if (strcmp(LineStr, "ucci") == 0) {
    return e_CommUcci;
  } else {
    return e_CommNone;
  }
}

CommEnum IdleLine(CommDetail &Command, int /* bool */ Debug) {
  static long s_CoordList[256];
  int i;
  const char *LineStr;
  CommEnum RetValue;
  LineStr = ReadInput();
  while (LineStr == NULL) {
    Idle();
    LineStr = ReadInput();
  }
  if (Debug) {
    printf("info string %s\n", LineStr);
    fflush(stdout);
  }
  // "IdleLine()"是最复杂的UCCI指令解释器，大多数的UCCI指令都由它来解释，包括：

  // 1. "isready"指令
  if (strcmp(LineStr, "isready") == 0) {
    return e_CommIsReady;

  // 2. "setoption <option> [<arguments>]"指令
  } else if (strncmp(LineStr, "setoption ", 10) == 0) {
    LineStr += 10;

    // (i) "batch"选项
    if (strncmp(LineStr, "batch ", 6) == 0) {
      LineStr += 6;
      Command.Option.Type = e_OptionBatch;
      if (strncmp(LineStr, "on", 2) == 0) {
        Command.Option.Value.Check = e_CheckTrue;
      } else if (strncmp(LineStr, "true", 4) == 0) {
        Command.Option.Value.Check = e_CheckTrue;
      } else {
        Command.Option.Value.Check = e_CheckFalse;
      } // 由于"batch"选项默认是关闭的，所以只有设定"on"或"true"时才打开，下同

    // (ii) "debug"选项
    } else if (strncmp(LineStr, "debug ", 6) == 0) {
      LineStr += 6;
      Command.Option.Type = e_OptionDebug;
      if (strncmp(LineStr, "on", 2) == 0) {
        Command.Option.Value.Check = e_CheckTrue;
      } else if (strncmp(LineStr, "true", 4) == 0) {
        Command.Option.Value.Check = e_CheckTrue;
      } else {
        Command.Option.Value.Check = e_CheckFalse;
      }

    // (iii) "bookfiles"选项
    } else if (strncmp(LineStr, "bookfiles ", 10) == 0) {
      Command.Option.Type = e_OptionBookFiles;
      Command.Option.Value.String = LineStr + 10;

    // (iv) "egtbpaths"选项
    } else if (strncmp(LineStr, "egtbpaths ", 10) == 0) {
      Command.Option.Type = e_OptionEgtbPaths;
      Command.Option.Value.String = LineStr + 10;

    // (v) "hashsize"选项
    } else if (strncmp(LineStr, "hashsize ", 9) == 0) {
      LineStr += 9;
      Command.Option.Type = e_OptionHashSize;
      Command.Option.Value.Spin = ReadDigit(LineStr, 1024);

    // (vi) "threads"选项
    } else if (strncmp(LineStr, "threads ", 8) == 0) {
      LineStr += 8;
      Command.Option.Type = e_OptionThreads;
      Command.Option.Value.Spin = ReadDigit(LineStr, 32);

    // (vii) "drawmoves"选项
    } else if (strncmp(LineStr, "drawmoves ", 10) == 0) {
      LineStr += 10;
      Command.Option.Type = e_OptionDrawMoves;
      Command.Option.Value.Spin = ReadDigit(LineStr, 200);

    // (viii) "repetition"选项
    } else if (strncmp(LineStr, "repetition ", 11) == 0) {
      LineStr += 11;
      Command.Option.Type = e_OptionRepetition;
      if (strncmp(LineStr, "alwaysdraw", 10) == 0) {
        Command.Option.Value.Repetition = e_RepetitionAlwaysDraw;
      } else if (strncmp(LineStr, "checkban", 8) == 0) {
        Command.Option.Value.Repetition = e_RepetitionCheckBan;
      } else if (strncmp(LineStr, "asianrule", 9) == 0) {
        Command.Option.Value.Repetition = e_RepetitionAsianRule;
      } else if (strncmp(LineStr, "chineserule", 11) == 0) {
        Command.Option.Value.Repetition = e_RepetitionChineseRule;
      } else {
        Command.Option.Value.Repetition = e_RepetitionChineseRule;
      }

    // (ix) "pruning"选项
    } else if (strncmp(LineStr, "pruning ", 8) == 0) {
      LineStr += 8;
      Command.Option.Type = e_OptionPruning;
      if (strncmp(LineStr, "none", 4) == 0) {
        Command.Option.Value.Scale = e_ScaleNone;
      } else if (strncmp(LineStr, "small", 5) == 0) {
        Command.Option.Value.Scale = e_ScaleSmall;
      } else if (strncmp(LineStr, "medium", 6) == 0) {
        Command.Option.Value.Scale = e_ScaleMedium;
      } else if (strncmp(LineStr, "large", 5) == 0) {
        Command.Option.Value.Scale = e_ScaleLarge;
      } else {
        Command.Option.Value.Scale = e_ScaleLarge;
      }

    // (x) "knowledge"选项
    } else if (strncmp(LineStr, "knowledge ", 10) == 0) {
      LineStr += 10;
      Command.Option.Type = e_OptionKnowledge;
      if (strncmp(LineStr, "none", 4) == 0) {
        Command.Option.Value.Scale = e_ScaleNone;
      } else if (strncmp(LineStr, "small", 5) == 0) {
        Command.Option.Value.Scale = e_ScaleSmall;
      } else if (strncmp(LineStr, "medium", 6) == 0) {
        Command.Option.Value.Scale = e_ScaleMedium;
      } else if (strncmp(LineStr, "large", 5) == 0) {
        Command.Option.Value.Scale = e_ScaleLarge;
      } else {
        Command.Option.Value.Scale = e_ScaleLarge;
      }

    // (xi) "selectivity"选项
    } else if (strncmp(LineStr, "selectivity ", 12) == 0) {
      LineStr += 12;
      Command.Option.Type = e_OptionSelectivity;
      if (strncmp(LineStr, "none", 4) == 0) {
        Command.Option.Value.Scale = e_ScaleNone;
      } else if (strncmp(LineStr, "small", 5) == 0) {
        Command.Option.Value.Scale = e_ScaleSmall;
      } else if (strncmp(LineStr, "medium", 6) == 0) {
        Command.Option.Value.Scale = e_ScaleMedium;
      } else if (strncmp(LineStr, "large", 5) == 0) {
        Command.Option.Value.Scale = e_ScaleLarge;
      } else {
        Command.Option.Value.Scale = e_ScaleNone;
      }

    // (xii) "style"选项
    } else if (strncmp(LineStr, "style ", 6) == 0) {
      LineStr += 6;
      Command.Option.Type = e_OptionStyle;
      if (strncmp(LineStr, "solid", 5) == 0) {
        Command.Option.Value.Style = e_StyleSolid;
      } else if (strncmp(LineStr, "normal", 6) == 0) {
        Command.Option.Value.Style = e_StyleNormal;
      } else if (strncmp(LineStr, "risky", 5) == 0) {
        Command.Option.Value.Style = e_StyleRisky;
      } else {
        Command.Option.Value.Style = e_StyleNormal;
      }

    // (xiii) "loadbook"选项
    } else if (strncmp(LineStr, "loadbook", 8) == 0) {
      Command.Option.Type = e_OptionLoadBook;

    // (xiv) 无法识别的选项，有扩充的余地
    } else {
      Command.Option.Type = e_OptionNone;
    }
    return e_CommSetOption;

  // 3. "position {<special_position> | fen <fen_string>} [moves <move_list>]"指令
  } else if (strncmp(LineStr, "position ", 9) == 0) {
    LineStr += 9;

    // 首先判断是否是特殊局面(这里规定了5种)，是特殊局面就直接转换成对应的FEN串
    if (strncmp(LineStr, "startpos", 8) == 0) {
      Command.Position.FenStr = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";
    } else if (strncmp(LineStr, "midgamepos", 10) == 0) {
      Command.Position.FenStr = "2bakab1r/6r2/1cn4c1/p1p1p3p/9/2P3p2/PC2P1n1P/2N1B1NC1/R4R3/3AKAB2 w - - 0 1";
    } else if (strncmp(LineStr, "checkmatepos", 12) == 0) {
      Command.Position.FenStr = "4kar2/4a2rn/4bc3/RN1c5/2bC5/9/4p4/9/4p4/3p1K3 w - - 0 1";
    } else if (strncmp(LineStr, "zugzwangpos", 11) == 0) {
      Command.Position.FenStr = "3k5/4PP3/4r4/3P5/9/9/9/9/9/5K3 w - - 0 1";
    } else if (strncmp(LineStr, "endgamepos", 10) == 0) {
      Command.Position.FenStr = "4k4/4a4/4P4/9/9/9/9/4B4/9/4K4 w - - 0 1";
    // 然后判断是否指定了FEN串
    } else if (strncmp(LineStr, "fen ", 4) == 0) {
      Command.Position.FenStr = LineStr + 4;
    // 如果两者都不是，就立即返回
    } else {
      return e_CommNone;
    }
    // 然后寻找是否指定了后续着法，即是否有"moves"关键字
    while (*LineStr != '\0') {
      if (strncmp(LineStr, " moves ", 7) == 0) {
        LineStr += 7;        
        Command.Position.MoveNum = int((strlen(LineStr) + 1) / 5); // 提示："moves"后面的每个着法都是4个字符和1个空格
        for (i = 0; i < Command.Position.MoveNum; i ++) {
          s_CoordList[i] = *(long *) LineStr; // 4个字符可转换为一个"long"，存储和处理起来方便
          LineStr += 5;
        }
        Command.Position.CoordList = s_CoordList;
        return e_CommPosition;
      }
      LineStr ++;
    }
    Command.Position.MoveNum = 0;
    return e_CommPosition;

  // 4. "banmoves <move_list>"指令，处理起来和"position ... moves ..."是一样的
  } else if (strncmp(LineStr, "banmoves ", 9) == 0) {
    LineStr += 9;
    Command.BanMoves.MoveNum = int((strlen(LineStr) + 1) / 5);
    for (i = 0; i < Command.Position.MoveNum; i ++) {
      s_CoordList[i] = *(int *) LineStr;
      LineStr += 5;
    }
    Command.BanMoves.CoordList = s_CoordList;
    return e_CommBanMoves;

  // 5. "go [ponder] {infinite | depth <depth> | time <time> [movestogo <moves_to_go> | increment <inc_time>]}"指令
  } else if (strncmp(LineStr, "go ", 3) == 0) {
    LineStr += 3;
    // 首先判断到底是"go"还是"go ponder"，因为两者解释成不同的指令
    if (strncmp(LineStr, "ponder ", 7) == 0) {
      LineStr += 7;
      RetValue = e_CommGoPonder;
    } else {
      RetValue = e_CommGo;
    }
    // 然后判断到底是固定深度还是设定时限
    if (strncmp(LineStr, "time ", 5) == 0) {
      LineStr += 5;
      Command.Search.DepthTime.Time = ReadDigit(LineStr, 36000);
      // 如果是设定时限，就要判断是时段制还是加时制
      if (strncmp(LineStr, " movestogo ", 11) == 0) {
        LineStr += 11;
        Command.Search.Mode = e_TimeMove;
        Command.Search.TimeMode.MovesToGo = ReadDigit(LineStr, 100);
        if (Command.Search.TimeMode.MovesToGo < 1) {
          Command.Search.TimeMode.MovesToGo = 1;
        }
      } else if (strncmp(LineStr, " increment ", 11) == 0) {
        LineStr += 11;
        Command.Search.Mode = e_TimeInc;
        Command.Search.TimeMode.Increment = ReadDigit(LineStr, 600);
      // 如果没有说明是时段制还是加时制，就设定为步数是1的时段制
      } else {
        Command.Search.Mode = e_TimeMove;
        Command.Search.TimeMode.MovesToGo = 1;
      }
    } else if (strncmp(LineStr, "depth ", 6) == 0) {
      LineStr += 6;
      Command.Search.Mode = e_TimeDepth;
      Command.Search.DepthTime.Depth = ReadDigit(LineStr, c_MaxDepth - 1);
    // 如果没有说明是固定深度还是设定时限，就固定深度为"c_MaxDepth"
    } else {
      Command.Search.Mode = e_TimeDepth;
      Command.Search.DepthTime.Depth = c_MaxDepth - 1;
    }
    return RetValue;

  // 5. "stop"指令
  } else if (strcmp(LineStr, "stop") == 0) {
    return e_CommStop;

  // 6. "quit"指令
  } else if (strcmp(LineStr, "quit") == 0) {
    return e_CommQuit;

  // 7. 无法识别的指令
  } else {
    return e_CommNone;
  }
}

CommEnum BusyLine(int /* bool */ Debug) {
  const char *LineStr;
  LineStr = ReadInput();
  if (LineStr == NULL) {
    return e_CommNone;
  } else {
    if (Debug) {
      printf("info string %s\n", LineStr);
      fflush(stdout);
    }
    // "BusyLine"只能接收"isready"、"ponderhit"和"stop"这三条指令
    if (strcmp(LineStr, "isready") == 0) {
      return e_CommIsReady;
    } else if (strcmp(LineStr, "ponderhit") == 0) {
      return e_CommPonderHit;
    } else if (strcmp(LineStr, "stop") == 0) {
      return e_CommStop;
    } else {
      return e_CommNone;
    }
  }
}
