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

extern int PuzzleFlag;
#include <stdlib.h>
#include <stdio.h>
#ifndef VC
#include <unistd.h>
#include <sys/time.h>
#endif
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdarg.h>
#include "control.h"
//#include <FL/Fl_Font.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_file_chooser.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Box.H>
#ifdef VC
#define HAVE_PTHREAD_H 0 
#else
#define HAVE_PTHREAD_H 1
#endif
#include "threads.h"
#define NO 0
#define YES 1
#define BG -1
#define BG_DONE -2
int thinking=0;
int thinking_start_at=0;
#ifdef XGCCH
Fl_Thread Thinking_thread;
XQAI *MB;
extern time_t SOFT_TIMELIMIT,EXPECT_TIME;
#ifdef VC
#else
//#define printf(a,b...)
//#define fflush(a)
#endif
#endif
static int kcolor=-1;	
static XQControl *w1;
int AppendComment(const char *fmt,...)
{
char p[128];
va_list ap;
va_start(ap, fmt);
#ifndef VC
(void) vsnprintf(p, 128, fmt, ap);
#else
(void) vsprintf(p,  fmt, ap);
#endif
va_end(ap);
Fl::lock();
w1->Comment.insert(p);
w1->Comment.redraw();
Fl::unlock();
return 1;
}

extern void send_move(YX from,YX to,int flip);
extern void EditBoardDialog(XQControl *b);
extern void ClientBoardDialog();
#ifdef XGCCH
ushort guess_move=0;
ushort guess_reply=0;
ushort guess_move_next=0;
void *do_thinking(void *p)
{
//	printf("enter thinking\n");
	XQBoard *box=(XQBoard *)p;
	XQAI *B=MB;
	B->XQ::operator=(*(box->B));
	B->GotoStart();
/*	for(int i=2;i<34;i++) printf("%d,%d %d,%d\n",
			YX_Y_X(box->B->P(i)),YX_Y_X(B->P(i)));*/
  for(int i=0;i<box->B->Ply();i++)
	  B->Move(box->B->tr[i].src,box->B->tr[i].dst);
  if(thinking==BG && !B->_Move(guess_move))
  {
	  thinking--;
	  return NULL;
  }
  if(B->Turn()!=kcolor) {
	  thinking--;
	  printf("kcolorError!!!\n");
  	return NULL;}
  {
	  int AUTO_LEVEL,AUTO_TIME,speed,v;
	  if(thinking==BG) AUTO_TIME=9999;
	  else AUTO_TIME=int(w1->mtime.value());
	  AUTO_LEVEL=int(w1->depth.value());
	  PuzzleFlag=w1->Puzzle.value();
    v=B->automove(AUTO_LEVEL,AUTO_TIME);   
  if(B->Turn()==kcolor) {kcolor=-1;
	  AppendComment("no ki11egg\n");
	  if(thinking==YES)
	  {  guess_move=0; 
	    AppendComment("無法Move\n");
	  }
	  else 	  guess_move_next=0;
	  thinking--;
	 return NULL;}
//    if(B->info.time>0) speed=B->info.TotalNodes/B->info.time;
    if(thinking==YES)
    {
	AppendComment(
#ifndef GB
			"第%d步 %s (%5d) %3d秒 %d層\n",
#else
			"菴%d祭 %s (%5d) %3d鏃 %d脯\n",
#endif
	B->Ply(),B->Chinese_Notation(),v,time(NULL)-thinking_start_at,B->InitDepth);
	/*
	int j=0;
	for(j=0;j<3 && B->_Move(_pvline[j]);j++)
		AppendComment("%s ",B->Chinese_Notation());
	if(j) AppendComment("\n");
	for(;j>0;j--) B->Back();
	*/
    }
  	Fl::lock();
   if(thinking==YES)	
	guess_move=_pvline[0];
   else
	guess_move_next=_pvline[0];
  }
  if(thinking==YES){
	box->B->GotoEnd();  
  	box->B->Move(B->movepos()>>8,B->movepos()&0xff);
	w1->cf.SetFileName("temp.icc");
	w1->cf.Save(*(box->B),0);
	if(time(NULL)<thinking_start_at+2) usleep(600);
	send_move(B->movepos()>>8,B->movepos()&0xff,w1->box.Direction);
  }
  else guess_reply=B->movepos();
  fl_cursor(FL_CURSOR_DEFAULT);
  box->do_callback();
  box->redraw();
if(thinking==YES) 
#ifdef WIN32	
	Beep(600,50);
#else
	fl_beep();
#endif	
    thinking--;
  Fl::unlock();
  Fl::awake();
//	printf("leave thinking\n");
    return NULL;
}
#endif
#ifdef XGCCH

void XQControl::handle_thinking()
{
	box.handle_thinking();
}
void XQBoard::handle_thinking()
{
	int turn;
	if(kcolor==-1) {if(thinking==YES ||thinking==BG) MB->stop_thinking=1;
		else thinking=NO;
		return;}
	turn=B->MaxPly()&1;
if(thinking==BG && turn==kcolor && !MB->stop_thinking)
{
	Fl::lock();
        if(!guess_move)
		thinking=NO;
	else if(guess_move==B->movepos())
	{
		   w1->Comment.value("");
		   AppendComment("ki11egg start thinking with depth=%d\n",
				   int(w1->depth.value()));
		   thinking_start_at=time(NULL);
		   thinking=YES;
		   SOFT_TIMELIMIT=time(NULL);
		   if(EXPECT_TIME && EXPECT_TIME> SOFT_TIMELIMIT+20
				   && MB->InitDepth>w1->depth.value())
			 MB->stop_thinking=1;  
		#ifdef VC 
		#endif 
	}
	else 
	{
	//	printf("guess wrong, stopthinking\n");
		MB->stop_thinking=1;
	}
	Fl::unlock();
}
if(thinking==BG_DONE && turn==kcolor)
{
	thinking=NO;
//	printf("BG done, now my turn\n");
	if(guess_move==B->movepos())
	{
//		printf("guess right\n");
		if(guess_reply)
		{
		B->GotoEnd();
		if(!B->Move(guess_reply>>8,guess_reply&0xff))
				printf("guess_reply wrong\n");
		else
		{
		w1->cf.SetFileName("temp.icc");
		w1->cf.Save(*B,0);
		Fl::lock();
		printf("send move case 1\n");
		usleep(600);
		send_move(guess_reply>>8,guess_reply&0xff,w1->box.Direction);
		Fl::unlock();
		}
    		if(thinking==YES)
			AppendComment("第%dply %s (%5d) %3d秒 %d層\n",
			B->Ply(),B->Chinese_Notation(),0,-1,MB->InitDepth);
		guess_move=guess_move_next;
		guess_move_next=guess_reply=0;
		  do_callback();
		  redraw();
		  printf("\7");fflush(stdout);
		}
	}
}
if(thinking==NO && turn!=kcolor && kcolor!=-1)
{   
	guess_reply=0;
//toggle bg thinking at here
if(!w1->BGThink.value()) guess_move=0;
 	if(guess_move)
	{
		thinking=BG;
 		fl_create_thread(Thinking_thread,do_thinking,this);
	}
	else
		thinking=BG_DONE;
}
if( thinking==NO && turn==kcolor )
{
	thinking=YES;
	w1->Comment.value("");
	AppendComment("ki11egg start thinking with depth=%d\n",int(w1->depth.value()));
	 thinking_start_at=time(NULL);
	  fl_cursor(FL_CURSOR_HELP);
 fl_create_thread(Thinking_thread,do_thinking,this);
#ifdef VC 
// SetThreadPriority(Thinking_thread,THREAD_PRIORITY_ABOVE_NORMAL);
#endif 
}
}
#endif
int XQBoard::handle(int Event) {
int bx,by; 
  switch(Event) {
   case FL_KEYBOARD:
    if(when()&FL_KEYBOARD)
    {
	    if(Fl::event_state(FL_Alt_L)||Fl::event_state(FL_Alt_R)) 
                 return 0; 
    
    return 1;
    }
   break;
   case FL_PUSH:
	ControlMethod[0]=w1->ControlMethodswitch[0]->value();
	ControlMethod[1]=w1->ControlMethodswitch[1]->value();
      bx=Fl::event_x()-x();
      by=Fl::event_y()-y();
	Cursorx=(bx-6)/42;
	Cursory=int((by-6)/41.5);

	Cursorx=(Direction ? Cursorx : 8-Cursorx);
	Cursory=(Direction ? 9-Cursory :  Cursory);
        if(Cursorx<0) Cursorx=0;
	if(Cursorx>8) Cursorx=8;
	if(Cursory<0) Cursory=0;
	if(Cursory>9) Cursory=9;
        if(Fl::event_button()==1)
         {
        if(ControlMethod[B->Turn()] || thinking==YES) {redraw();return 1;}
        if(ControlMethod[1-B->Turn()] && B->Ply()!=B->MaxPly()) {
		B->GotoEnd();
		redraw();
		do_callback();
		return 1;
	}
				
		if( B->B(Cursory,Cursorx) && (Edit||((B->B(Cursory,Cursorx)&1)==B->Turn())) )
			{
	         Selectx=Cursorx;
	        Selecty=Cursory;
		Select=true;
	}
	else if(Edit &&Select)
	{	B->xmove(YX_Set(Selecty,Selectx),YX_Set(Cursory,Cursorx));
		Select=false;
	}
	else if(Select)
	{
		B->Move(Selecty,Selectx,Cursory,Cursorx);
		do_callback();
		Select=false;
	}
redraw();
}
else if(Fl::event_button()==3)
{
	if(Edit)
	 B->xdel(Cursory,Cursorx);
         redraw();
	 
}

return 1;
break;
   case FL_FOCUS:
     return 1;
   break;
   case FL_UNFOCUS:
     return 1;
   break;
  }
handle_thinking();
return 0;
}
//#ifndef XGCCH
char *XQControl::ExecCmd(char *str,int side)
{
static char rtn[500];
int i;
int parent=0;
printf("ExecCmd %s %d\n",str,side);
if(side==2) {parent=1;if(str[0]=='0') side=0;else side=1;}// problem!!
if(side<0 || side>1) return "!Er Internal error";
if(!parent) parent=(box.ControlMethod[side]==1 ? 1:0);
rtn[0]=0;
if(strlen(str)<2) return "!Er Command too short"; 
if(str[0]=='!' && !parent) {return "!Er You are not my parent.";}
else if(str[0]=='!')
{
if(!strncmp(str+2,"ctl",3)) {
	if(str[1]=='0') {box.ControlMethod[0]=1;return "!OK Now you control red";}
        if(str[1]=='1') {box.ControlMethod[1]=1;return "!OK Now you control black";}
        return "!Er I don'r know who you want to control";}
if(!strncmp(str+2,"rtn",3)) {
	if(str[1]=='0') {box.ControlMethod[0]=0;return "!OK Now I control red";}
        if(str[1]=='1') {box.ControlMethod[1]=0;return "!OK now I control black";}
        return "!Er I don'r know  which side";}
if(!strncmp(str,"!restart",8)) {box.B->GotoStart();redraw();return "!OK";}
if(!strncmp(str,"!exit",5)) {exit(1);}
return "!Er Sorry, I don't know how to do this";
}
else if(str[0]-side=='0')
{
/*if(!strncmp(str+1,"lose",4)) {
				}
if(!strncmp(str,"peace",5)) {
				}
	*/			
if(!strncmp(&str[1],"undo",4)) {B->Back();redraw();return "!OK";}
					
return "!Er I don't know how to do this";
			
}
else if(B->Turn()!=side)
{
return "!Er Not You turn";
}
else if(B->CmdMove(str))
{
redraw();
return "OK moved";
}
else
{
snprintf(rtn,490,"Error Move %s\n",str);
return rtn;
}
}
//#endif
static Fl_Shared_Image *board_xpm0;
Fl_Shared_Image *board_xpm;
Fl_Shared_Image *piece_xpm[34];
int bd_w,bd_h,pc_w,pc_h;
XQBoard::XQBoard(int x,int y,int Width,int Height,const char *Label)
: Fl_Widget(x,y,Width,Height,Label)
{
char blue[]="img/Blue0.xpm",red[]="img/Red0.xpm";
int i,j;
B=new XQ();
Direction=1;
Edit=false;
Select=false;
Cursorx=Cursory=0;
ControlMethod[0]=ControlMethod[1]=0;
if(board_xpm!=NULL) return;
//board_xpm0=Fl_Shared_Image::get("img/b.xpm");//useless image, but it seems
					 // first shared image will corrupt
board_xpm=Fl_Shared_Image::get("img/board.xpm");
//board_xpm=Fl_Shared_Image::get("img/P922000B.bmp");
//((Fl_Shared_Image *)board_xpm)->measure(bd_w,bd_h);

bd_w=((Fl_Shared_Image *)board_xpm)->w();
bd_h=((Fl_Shared_Image *)board_xpm)->h();
for(i=2;i<34;i+=2)
 {    
        blue[8]=red[7]='0'+B->id2type(i);
        piece_xpm[i]=Fl_Shared_Image::get(red);
        piece_xpm[i+1]=Fl_Shared_Image::get(blue);
        
 }
pc_h=((Fl_Shared_Image *)piece_xpm[2])->h();
pc_w=((Fl_Shared_Image *)piece_xpm[2])->w();

}
XQBoard::~XQBoard()
{
delete B; 
}

void XQBoard::draw()
{
int i,j;
int ti,tj,p;
if(board_xpm)
board_xpm->draw(x(),y(),bd_w,bd_h,0,0);
for(i=0;i<9;i++)
for(j=0;j<10;j++)
  {
	ti=(Direction ? i : 8-i);
	tj=(Direction ? 9-j :  j);
	if(p=B->B(tj,ti))
		piece_xpm[p]->draw(x()+posx(i),y()+posy(j),pc_w,pc_h,0,0); 
  }
fl_color(0,100,100);
	ti=(Direction ? Cursorx : 8-Cursorx);
	tj=(Direction ? 9-Cursory :  Cursory);
fl_rect(posx(ti)+x(),posy(tj)+y(),pc_w,pc_h);
if(Select) {
	fl_color(255,255,25);
	ti=(Direction ? Selectx : 8-Selectx);
	tj=(Direction ? 9-Selecty :  Selecty);
	fl_arc(posx(ti)+x(),posy(tj)+y(),pc_w,pc_h,0,360);
}
	fl_color(25,255,25);
	p=(B->movepos());
	if(p)
	{
	i=YX_X(p&0xff);
	j=YX_Y(p&0xff);
	ti=(Direction ? i : 8-i);
	tj=(Direction ? 9-j :  j);
	fl_arc(posx(ti)+x(),posy(tj)+y(),pc_w,pc_h,0,360);
	/*i=YX_X(p>>8);
	j=YX_Y(p>>8);
	ti=(Direction ? i : 8-i);
	tj=(Direction ? 9-j :  j);
	fl_arc(posx(ti)+x(),posy(tj)+y(),pc_w,pc_h,0,360);
	*/
	}
}
static char *ButtonLabel[]={
#ifndef GB	
	"Games","紅勝","黑勝","和棋",
#else
	
	"攫杅","綻吨","窪吨","睿め",
#endif
	"@|<","@<","@>","@>|","@2<->",
	"@square","good","bad","even","New","CLI","Edit",NULL
}; 		    
//開始定義XQControl的 Callback 函數們
static void Buttoncb(Fl_Widget *button,void *data) {
      ((XQControl *)data)->ButtonAction(button->label());
}
static void MoveTextcb(Fl_Widget *widget,void *data) {
      ((XQControl *)data)->MoveTextAction();
}
static void XQBoardcb(Fl_Widget *cb,void *data) {
      ((XQControl *)data)->MoveTextChanged();
	
}
static void open_cb(Fl_Widget *widget,void *data) {
	((XQControl *)data)->LoadFile();
	
}
static void about_cb(Fl_Widget *widget,void *data) {
fl_message(
#ifndef GB		
"撤蛋 (C)1995-2002 Tzer-jen Wei\n\
版本代號 夢入聖蛋/MRSD+ Dec 22 2002\n\
"
#else
"雪粥 (C)1995-2002 Tzer-jen Wei\n\
唳掛測瘍 襞�輮扔�/MRSD+ Dec 22 2002\n\
"
#endif
);	
}
static void save_cb(Fl_Widget *widget,void *data) {
	((XQControl *)data)->SaveFile();
}
//結束定義XQControl的 Callback 函數們

void XQControl::_SetMoveText(int ply){
	char buf[50];
	sprintf(buf,"@C%d@.%3d %s",ply&1 ? 4 : 1,(ply+2)/2,B->Chinese_Notation(ply));
	MoveText.add(buf);
}
	
void XQControl::LoadFile(){
	char *fn=fl_file_chooser("switchLoad檔案","*.icc",NULL);
	printf("Load%s\n",fn);
	cf.SetFileName(fn);
	if(!cf.Load(*B,0)) {fl_alert("無法Load檔案 %s!!\n",fn);return;}
	MoveText.clear();
	for(int i=0;i<B->MaxPly();i++) _SetMoveText(i);	
	MoveTextChanged();
	redraw();
}
void XQControl::SaveFile(){
	char *fn=fl_file_chooser("switchSave檔案","*.icc",NULL);
	printf("Save%s\n",fn);
	cf.SetFileName(fn);
	if(!cf.Save(*B,0)) {fl_alert("Save檔案%s時發生Error",fn);return;}
	redraw();
}
	
	
void XQControl::MoveTextChanged(){
          //懶惰而不好的作法，有的時候可能會有問題
	  char buf[500];
	  int n,i;
	  if(B->MaxPly()==MoveText.size()+1){
	     _SetMoveText(B->MaxPly()-1);
		printf("%s\n",B->movepos(buf));
	  }
	  else if(B->MaxPly()!=MoveText.size())
	  {
		  MoveText.clear();
		  for(int i=0;i<B->MaxPly();i++)
			  _SetMoveText(i);
	  }
	  MoveText.select(B->Ply());
Information.clear();
//XQ B2=*B;
XQDB *db=new XQDB();
ImageNode p,p2;
YX s,d;
db->FindNode(B,p);
if(p.Games)
{
sprintf(buf,"\t%d\t%2.1f%%\t%2.1f%%\t%2.1f%%\t  ",
		p.Games < 1000 || p.Result[2]==-1 ? p.Games: 1000,
		(B->Turn() ? p.Result[1]:p.Result[0])*100.0/p.Games,
		(B->Turn() ? p.Result[0]:p.Result[1])*100.0/p.Games,
		p.Result[2]*100.0/p.Games);
Information.add(buf);
}
#if 0
n=0;
for(i=0;i<n;i++)
{s=0;
 d=0;
B.Move(s,d);
db->FindNode(&B2,p2);
sprintf(buf,"%s\t%d\t%2.1f%%\t%2.1f%%\t%2.1f%%\t@.%c%c",B2.Chinese_Notation(),p2.Games,
		p2.Result[0]*100.0/p2.Games,p2.Result[1]*100.0/p2.Games,
		p2.Result[2]*100.0/p2.Games,' '+s,' '+d);
Information.add(buf);
B2.到退();
        }
#endif
delete db;
//	  Comment.value(cf.LoadComment(B->Ply()));
	  Information.redraw();
	  Comment.redraw();
	  MoveText.redraw();
}
void XQControl::MoveTextAction(){
	B->GotoPly(MoveText.value());
	MoveTextChanged();
	box.redraw();
}
void XQControl::ButtonAction(const char *Label){
	int i;
	XQDB *db;
	ImageNode p;
	if(Label==NULL) return;
	for(i=0;ButtonLabel[i]!=NULL && strcmp(ButtonLabel[i],Label);i++);
	if(thinking==YES && i<8) return;
	switch(i){
		case 4:
			B->GotoStart();
		break;
		case 5:
			B->Back();
		break;
		case 6:
			B->Forward();
		break;
		case 7:
			B->GotoEnd();
		break;
		case 8:
			box.ToggleDirection();
		redraw();
		      return;
#ifdef XGCCH		      
		case 9:
		      if(thinking==YES || thinking==BG)
		      {
		      MB->stop_thinking=1;
		      if(thinking==BG) guess_move=0;
		      }
		      return;
#endif		      
		case 10:
                case 11:
		case 12:
		      db=new XQDB();
		      YX s,d;
		      db->FindNode(B,p);
		      if(p.Result[2]!=-1) p.Result[0]=p.Result[1]=0;
		      p.Result[2]=-1;
		      if(i==10){ p.Result[1]= p.Result[0] ? 1000:p.Result[1]+1000;
			         p.Result[0]=0;}
		      if(i==11){ p.Result[0]= p.Result[1] ? 1000:p.Result[0]+1000;
			         p.Result[1]=0;}
		      if(i==12) {if(p.Result[0]!= p.Result[1]) 
			      p.Result[0]=p.Result[1]=0;
			      else {p.Result[0]+=1;p.Result[1]+=1;}}
		      db->SetNode(B,p);
		      delete db;
		 break;     
		case 13:
		 B->NewGame();
		 break;
		case 14:
		 ClientBoardDialog();
		 break;
		case 15:
		 EditBoardDialog(this);
		 break;
		 default:
			return;
		break;
	}
	MoveTextChanged();
	redraw();
}
		
	
static void red_egg(Fl_Widget *widget,void *data) {
	if(kcolor!=0)
	{
	kcolor=0;
	MB->stop_thinking=1;
	}
	AppendComment("Set red ki11egg\n");
}
static void black_egg(Fl_Widget *widget,void *data) {
	if(kcolor!=1)
	{
	kcolor=1;
	AppendComment("Set black ki11egg\n");
	}
	MB->stop_thinking=1;
}
static void no_egg(Fl_Widget *widget,void *data) {
	if(kcolor!=-1)	kcolor=-1;
	AppendComment("Set no ki11egg\n");
	MB->stop_thinking=1;
}

static char *ControlMethodMenuText[]={"GUI","父程序","撤蛋"};
static int wd[]={80,80,60,60,60,10,0,0,0,0};
XQControl::XQControl():
#ifndef GB
	Fl_Double_Window(800,500,"MRSD+/夢入聖蛋"),
#else	
	Fl_Double_Window(800,500,"MRSD+/襞�輮扔�"),
#endif	
                     box(2,30),
		     Menu(0,0,400,30),
		     MoveText(410,10,120,200),
		     Information(410,250,380,20),
		     Comment(530,10,250,200),
		     depth(450,280,60,20,"depth"),
		     mtime(550,280,60,20,"time"),
		     Puzzle(450,380,60,20,"Puzzle"),
		     BGThink(550,380,60,20,"BGthink")

       {
	int i,s;
	Fl_Button *tmpB;
	Information.column_widths(wd);
	for(s=wd[0],i=0;i<4;s+=wd[++i]) {
		tmpB=new Fl_Button(410+s,225,wd[i+1],25,ButtonLabel[i]);
		tmpB->callback(Buttoncb,(void *)this);
		tmpB->box(FL_UP_BOX);
	}
	for(;ButtonLabel[i]!=NULL;i++) {
		tmpB=new Fl_Button(2+i*40,465,40,33,ButtonLabel[i]);
		tmpB->callback(Buttoncb,(void *)this);
		tmpB->labeltype(FL_SYMBOL_LABEL);
		tmpB->box(FL_UP_BOX);
	}
		
	Menu.add("&File",FL_ALT+'f',0,0,FL_SUBMENU);
	Menu.add("&File/&Open",FL_ALT+'o',open_cb,(void *)this);
	Menu.add("&File/&Save",FL_ALT+'s',save_cb,(void *)this);
	Menu.add("&Ki11egg",FL_ALT+'k',0,0,FL_SUBMENU);
	Menu.add("&Ki11egg/&Red Ki11egg",FL_ALT+'r',red_egg,(void *)this);
	Menu.add("&Ki11egg/&Black Ki11egg",FL_ALT+'b',black_egg,(void *)this);
	Menu.add("&Ki11egg/&No Ki11egg",FL_ALT+'n',no_egg,(void *)this);
	Menu.add("&Help",FL_ALT+'h',0,0,FL_SUBMENU);
	Menu.add("&Help/&About",0,about_cb,(void *)this);
	box.box(FL_UP_BOX);
	box.callback(XQBoardcb,(void *)this);
	MoveText.format_char('@');
	MoveText.column_char(0);
//	MoveText.textfont(FL_COURIER);
	MoveText.callback(MoveTextcb,(void *)this);
	  ControlMethodswitch[0]=new Fl_Choice(450,330,50,20,"??");
         ControlMethodswitch[1]=new Fl_Choice(550,330,50,20,"??");
         for(i=0;i<3;i++){
		 ControlMethodswitch[0]->add(ControlMethodMenuText[i]);
		 ControlMethodswitch[1]->add(ControlMethodMenuText[i]);
	 }
		ControlMethodswitch[0]->value(0);
		ControlMethodswitch[1]->value(0);
	end();
	depth.type(FL_SIMPLE_COUNTER);
	depth.step(1.0);
	depth.precision(0);
	depth.bounds(5.0,15.0);
	depth.value(8.0);
	mtime.type(FL_SIMPLE_COUNTER);
	mtime.step(1.0);
	mtime.precision(0);
	mtime.bounds(0.0,60.0);
	mtime.value(3.0);
	Puzzle.value(0);
	BGThink.value(1);
	B=box.B;
	MoveTextChanged();
}	
XQControl::~XQControl()
{
		fl_beep();
}
//#ifndef XGCCH
void XQControl::stdin_cb(){
	char buf[500];
	int l;
	if(fgets(buf,490,stdin)<=0)
	{
		exit(1);
		return;
	}
box.ControlMethod[0]=ControlMethodswitch[0]->value();
box.ControlMethod[1]=ControlMethodswitch[1]->value();
//	printf("[%s]\n",buf);
//if(box.ControlMethod[B->Turn()]==1)
	printf("%s\n",ExecCmd(buf,B->Turn()));
/*else
printf("%s\n",ExecCmd(buf,2));*/
ControlMethodswitch[0]->value(box.ControlMethod[0]);
ControlMethodswitch[1]->value(box.ControlMethod[1]);
fflush(stdout);
handle_thinking();
}
	
void stdin_cb(int f,void *data){
	((XQControl *)data)->stdin_cb();
}
//#endif
void callback(void* ) {
  w1->handle_thinking();
  Fl::repeat_timeout(2.0,callback);
}

int main(int argc, char **argv) {
#if 0	
  fd_set rfds;
  struct timeval tv;
  int retval;
#endif  
  XQControl *window =new XQControl();
  window->show(1,argv);
  
#ifndef XGCCH
  Fl::add_fd(0,stdin_cb,(void *)window);
  return Fl::run();
  //原來該是如下寫法，但不靈
#if 0  
 while(1) {
    FD_ZERO(&rfds);
    FD_SET(0,&rfds);
    tv.tv_sec=0;
    tv.tv_usec=0;
    retval=select(1,&rfds,NULL,NULL,&tv);
    if(retval) if(FD_ISSET(0,&rfds)) {
         stdin_cb(0,(void *)window); 
         }
    Fl::wait(0.01);
   }
#endif 
#else   
  Fl::add_fd(0,stdin_cb,(void *)window);
 MB=new XQAI();
  w1=window;
#ifndef VC  
  msg_cb=AppendComment;
#endif 
  Fl::lock();
  Fl::add_timeout(1.0,callback);
  return  Fl::run();
#endif
}

