/*! \file ucci.h
@brief UCCI通讯模块（第三方提供，遵循GPL许可）

　　作者信息：黄晨 复旦大学化学系表面化学实验室 E-mail：webmaster at elephantbase dot net 
*/

#ifndef UCCI_H
#define UCCI_H

#include "../utility/bool.h"
const int UCCI_MAX_DEPTH = 32; // UCCI引擎思考的极限深度

// 和UCCI指令中关键字有关的选项
enum UcciOptionEnum {
  UCCI_OPTION_NONE, UCCI_OPTION_BATCH, UCCI_OPTION_DEBUG, UCCI_OPTION_USEMILLISEC, UCCI_OPTION_BOOKFILES, UCCI_OPTION_HASHSIZE, UCCI_OPTION_THREADS, UCCI_OPTION_DRAWMOVES,
  UCCI_OPTION_REPETITION, UCCI_OPTION_PRUNING, UCCI_OPTION_KNOWLEDGE, UCCI_OPTION_SELECTIVITY, UCCI_OPTION_STYLE, UCCI_OPTION_LOADBOOK
}; // uo, 由"setoption"指定的选项
enum UcciRepetEnum {
  UCCI_REPET_ALWAYSDRAW, UCCI_REPET_CHECKBAN, UCCI_REPET_ASIANRULE, UCCI_REPET_CHINESERULE
}; // ur
enum UcciGradeEnum {
  UCCI_GRADE_NONE, UCCI_GRADE_SMALL, UCCI_GRADE_MEDIUM, UCCI_GRADE_LARGE
}; // ug
enum UcciStyleEnum {
  UCCI_STYLE_SOLID, UCCI_STYLE_NORMAL, UCCI_STYLE_RISKY
}; // us, 选项style的设定值
enum UcciTimeEnum {
  UCCI_TIME_DEPTH, UCCI_TIME_MOVE, UCCI_TIME_INC
}; // ut, 由"go"指令指定的时间模式，分别是固定深度(无限深相当于深度为"c_MaxDepth")、时段制(几秒钟内必须完成几步)和加时制(剩余时间多少，走完这步后加几秒)
enum UcciCommEnum {
  UCCI_COMM_NONE, UCCI_COMM_UCCI, UCCI_COMM_ISREADY, UCCI_COMM_PONDERHIT, UCCI_COMM_STOP, UCCI_COMM_SETOPTION, UCCI_COMM_POSITION, UCCI_COMM_BANMOVES, UCCI_COMM_GO, UCCI_COMM_GOPONDER, UCCI_COMM_QUIT
}; // uce, UCCI指令类型

///UCCI通讯模块（第三方提供，遵循GPL许可） 
/*!
　　我们直接使用UCCI开发人员提供的通用通讯模块（遵循GPL许可）。

　　具体实现和功能说明参见：http://www.elephantbase.net/protocol/cchess_ucci.htm。
*/
// UCCI指令可以解释成以下这个抽象的结构
union UcciCommStruct {

  /* 可得到具体信息的UCCI指令只有以下4种类型
   *
   * 1. "setoption"指令传递的信息，适合于"UCCI_COMM_SETOPTION"指令类型
   *    "setoption"指令用来设定选项，因此引擎接受到的信息有“选项类型”和“选项值”
   *    例如，"setoption batch on"，选项类型就是"UCCI_OPTION_DEBUG"，值(Value.bCheck)就是"TRUE"
   */
  struct {
    UcciOptionEnum uoType;   // 选项类型
    union {                  // 选项值
      int nSpin;             // "spin"类型的选项的值
      Bool bCheck;           // "check"类型的选项的值
      UcciRepetEnum urRepet; // "combo"类型的选项"repetition"的值
      UcciGradeEnum ugGrade; // "combo"类型的选项"pruning"、"knowledge"和"selectivity"的值
      UcciStyleEnum usStyle; // "combo"类型的选项"style"的值
      const char *szString;  // "string"类型的选项的值
    } Value;                 // "button"类型的选项没有值
  } Option;

  /* 2. "position"指令传递的信息，适合于"e_CommPosition"指令类型
   *    "position"指令用来设置局面，包括初始局面连同后续着法构成的局面
   *    例如，position startpos moves h2e2 h9g8，FEN串就是"startpos"代表的FEN串，着法数(MoveNum)就是2
   */
  struct {
    const char *szFenStr; // FEN串，特殊局面(如"startpos"等)也由解释器最终转换成FEN串
    int nMoveNum;         // 后续着法数
    long *lpdwCoordList;  // 后续着法，指向程序"IdleLine()"中的一个静态数组，但可以把"CoordList"本身看成数组
  } Position;

  /* 3. "banmoves"指令传递的信息，适合于"e_CommBanMoves"指令类型
   *    "banmoves"指令用来设置禁止着法，数据结构时类似于"position"指令的后续着法，但没有FEN串
   */
  struct {
    int nMoveNum;
    long *lpdwCoordList;
  } BanMoves;

  /* 4. "go"指令传递的信息，适合于"e_CommGo"或"e_CommGoPonder"指令类型
   *    "go"指令让引擎思考(搜索)，同时设定思考模式，即固定深度、时段制还是加时制
   */
  struct {
    UcciTimeEnum utMode; // 思考模式
    union {          
      int nDepth, nTime; 
    } DepthTime; // 如果是固定深度，则表示深度(层)，如果是限定时间，则表示时间(秒)
    union {
      int nMovesToGo, nIncrement;
    } TimeMode;  // 如果是加时制，则限定时间内要走多少步棋，如果是时段制，则表示走完该步后限定时间加多少秒
  } Search;
}; // ucs

// 下面三个函数用来解释UCCI指令，但适用于不同场合
UcciCommEnum BootLine(void);                                    // UCCI引擎启动的第一条指令，只接收"ucci"
UcciCommEnum IdleLine(UcciCommStruct &ucsCommand, Bool bDebug); // 引擎空闲时接收指令
UcciCommEnum BusyLine(Bool bDebug);                             // 引擎思考时接收指令，只允许接收"stop"和"ponderhit"
void ExitLine(void);

#endif
