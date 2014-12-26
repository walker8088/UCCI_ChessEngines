#ifndef UCCI_H
#define UCCI_H

/* 文件：ucci.h
 * 内容：ElephantEye源程序第1部分——UCCI指令解释模块
 * 标识符约定：
 * c_：常量；
 * e_：选项；
 * g_：着法预产生数组；
 * p_：局面(ChessPosition)的成员变量；
 * s_：静态(全局)变量。
 */

const int c_MaxDepth = 32; // UCCI引擎思考的极限深度

// 和UCCI指令中关键字有关的选项
enum OptionEnum {
  e_OptionNone, e_OptionBatch, e_OptionDebug, e_OptionBookFiles, e_OptionEgtbPaths, e_OptionHashSize, e_OptionThreads, e_OptionDrawMoves,
  e_OptionRepetition, e_OptionPruning, e_OptionKnowledge, e_OptionSelectivity, e_OptionStyle, e_OptionLoadBook
}; // 由"setoption"指定的选项
enum CheckEnum {
  e_CheckFalse, e_CheckTrue
}; // 选项类型为"check"的设定值，只有关(off或false)或开(on或true)两种
enum RepetitionEnum {
  e_RepetitionAlwaysDraw, e_RepetitionCheckBan, e_RepetitionAsianRule, e_RepetitionChineseRule
};
enum ScaleEnum {
  e_ScaleNone, e_ScaleSmall, e_ScaleMedium, e_ScaleLarge
};
enum StyleEnum {
  e_StyleSolid, e_StyleNormal, e_StyleRisky
}; // 选项style的设定值
enum TimeEnum {
  e_TimeDepth, e_TimeMove, e_TimeInc
}; // 由"go"指令指定的时间模式，分别是固定深度(无限深相当于深度为"c_MaxDepth")、时段制(几秒钟内必须完成几步)和加时制(剩余时间多少，走完这步后加几秒)
enum CommEnum {
  e_CommNone, e_CommPonderHit, e_CommStop, e_CommUcci, e_CommIsReady, e_CommSetOption, e_CommPosition, e_CommBanMoves, e_CommGo, e_CommGoPonder, e_CommQuit
}; // UCCI指令类型

// UCCI指令可以解释成以下这个抽象的结构
union CommDetail {

  /* 可得到具体信息的UCCI指令只有以下4种类型
   *
   * 1. "setoption"指令传递的信息，适合于"e_CommSetOption"指令类型
   *    "setoption"指令用来设定选项，因此引擎接受到的信息有“选项类型”和“选项值”
   *    例如，setoption batch on，选项类型就是"e_OptionDebug，值(Value.Spin)就是"e_CheckTrue"
   */
  struct {
    OptionEnum Type;             // 选项类型
    union {                      // 选项值
      int Spin;                  // "spin"类型的选项的值
      CheckEnum Check;           // "check"类型的选项的值
      RepetitionEnum Repetition;
      ScaleEnum Scale;
      StyleEnum Style;           // "combo"类型的选项的值
      const char *String;        // "string"类型的选项的值
    } Value;                     // "button"类型没有值
  } Option;

  /* 2. "position"指令传递的信息，适合于"e_CommPosition"指令类型
   *    "position"指令用来设置局面，包括初始局面连同后续着法构成的局面
   *    例如，position startpos moves h2e2 h9g8，FEN串就是"startpos"代表的FEN串，着法数(MoveNum)就是2
   */
  struct {
    const char *FenStr; // FEN串，特殊局面(如"startpos"等)也由解释器最终转换成FEN串
    int MoveNum;        // 后续着法数
    long *CoordList;    // 后续着法，指向程序"IdleLine()"中的一个静态数组，但可以把"CoordList"本身看成数组
  } Position;

  /* 3. "banmoves"指令传递的信息，适合于"e_CommBanMoves"指令类型
   *    "banmoves"指令用来设置禁止着法，数据结构时类似于"position"指令的后续着法，但没有FEN串
   */
  struct {
    int MoveNum;
    long *CoordList;
  } BanMoves;

  /* 4. "go"指令传递的信息，适合于"e_CommGo"或"e_CommGoPonder"指令类型
   *    "go"指令让引擎思考(搜索)，同时设定思考模式，即固定深度、时段制还是加时制
   */
  struct {
    TimeEnum Mode;   // 思考模式
    union {          
      int Depth;     // 如果是固定深度，则表示深度(层)
      int Time;      // 如果是限定时间，则表示时间(秒)
    } DepthTime;
    union {
      int MovesToGo; // 如果是加时制，则限定时间内要走多少步棋
      int Increment; // 如果是时段制，则表示走完该步后限定时间加多少秒
    } TimeMode;
  } Search;
};

// 下面两个函数可被UCCI指令解释器和浅红象棋协议解释器调用
char *ReadInput(void);                             // 读取一行
int ReadDigit(const char *&LineStr, int MaxValue); // 读取某串字符中的数字

// 下面三个函数用来解释UCCI指令，但适用于不同场合
CommEnum BootLine(void);                                      // UCCI引擎启动的第一条指令，只接收"ucci"
CommEnum IdleLine(CommDetail &Command, int /* bool */ Debug); // 引擎空闲时接收指令
CommEnum BusyLine(int /* bool */ Debug);                      // 引擎思考时接收指令，只允许接收"stop"和"ponderhit"

int /* bool */ QhInputLine(int /* bool */ Busy); // 浅红象棋协议的接收过程

#endif
