/*
Ki11egg , Xiangqi(Chinese Chess) engine
Copyright (C) Tzer-jen Wei

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _CONTROL_HH
#  define _CONTROL_HH
#define XQControl   XQControl 
#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Counter.H>
#ifdef XGCCH
#include "../cfast7/mboard.h"
#else
#include <board.h>
#endif
#include <cbfile.h>
#include <cbdb.h>
//類別 XQControl;
class XQBoard  : public  Fl_Widget
{
protected:
int handle(int );
int Cursorx,Cursory;
int Selectx,Selecty;
public:
  int ControlMethod[2]; //0:GUI 1:stdio 2:pipe plugin
  int Direction;
  void ToggleDirection(){Direction=!Direction;}
  bool Edit; 
  bool Select;
  XQ *B;
  XQBoard (int,int,int Width=400,int Height=433,const char *Label=NULL);
  void draw();
  void handle_thinking();
  ~XQBoard();
};


class XQControl : public Fl_Double_Window
{
protected:
Fl_Menu_Bar Menu;
Fl_Hold_Browser MoveText;
Fl_Select_Browser Information;
void _SetMoveText(int);
public:
XQ *B;
Fl_Choice *ControlMethodswitch[2];
XQFile cf;
XQBoard box;
Fl_Counter depth;
Fl_Counter mtime;
Fl_Check_Button Puzzle;
Fl_Check_Button BGThink;
Fl_Multiline_Output Comment;
XQControl();
//主要資料
int player_ctl[2];
//void draw();
void LoadFile();
void SaveFile();
void MoveTextChanged();
~XQControl();
void ButtonAction(const char *);
void MoveTextAction();
void CommentAction();
void stdin_cb();
char *ExecCmd(char *,int);
void handle_thinking();
//gint idle_main();
//玩家ControlMethod模式switch
//int has_computer_player();
//int clear_computer_player();
//char *ExecCmd(char *str);
};
inline int posx(int i) {
 return i<0 ? 0 : i>10 ? 400: i*42+6;
}
inline int posy(int i) { 
	return i<0 ? 0 : i>11 ? 433 : i*41.5+6;
}
#endif
