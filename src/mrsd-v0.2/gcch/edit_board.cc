#include "control.h"
#include <stdio.h>
#include <unistd.h>
#include <FL/fl_draw.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Hold_Browser.H>
static XQControl *XQCTL;
extern Fl_Shared_Image *board_xpm;
extern Fl_Shared_Image *piece_xpm[34];
extern int bd_w,bd_h,pc_w,pc_h;
class edit_area:public Fl_Widget
{
	public:
	edit_area(int,int);
	void draw();	
	int handle(int);
	int Cursorx,Cursory;
	int Selectx,Selecty;
	int Select;
	XQ *B;
};
edit_area::edit_area(int x,int y)
	:Fl_Widget(x,y,600,600)
{
}
class edit_board_dialog: public Fl_Window
{
	public:
	edit_board_dialog();
	~edit_board_dialog();
	void save();
	edit_area box;
	Fl_Button update;
	Fl_Button reset;
	XQ *B;
};
static	edit_board_dialog *dialog=NULL;
extern void EditBoardDialog(XQ *BD);
static void resetcb(Fl_Widget *pg,void *data)
{
	dialog->B->NewGame();
	dialog->redraw();
}
static void updatecb(Fl_Widget *pg,void *data)
{
	printf("update\n");
	*(XQCTL->B)=*(dialog->B);
	XQCTL->redraw();
	dialog->hide();
	}
edit_board_dialog::edit_board_dialog():
	Fl_Window(bd_w+200,bd_h,"client board setup"),
	box(200,0),
	update(1,3,80,20,"update"),
	reset(1,30,80,20,"reset")
{
	B=new XQ();
	box.B=B;
	update.callback(updatecb,NULL);
	reset.callback(resetcb,NULL);
}
edit_board_dialog::~edit_board_dialog()
{
	delete B;
}
int edit_area::handle(int Event)
{
int bx,by; 
int Direction=1;
  switch(Event) {
   case FL_PUSH:
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
	if( B->B(Cursory,Cursorx))
			{
	         Selectx=Cursorx;
	        Selecty=Cursory;
		Select=true;
	}
	else if(Select)
	{	B->xmove(YX_Set(Selecty,Selectx),YX_Set(Cursory,Cursorx));
		Select=false;
	}
}
else if(Fl::event_button()==3)
{
	 B->xdel(Cursory,Cursorx);
}

	 redraw();
return 1;
break;
   case FL_FOCUS:
     return 1;
   break;
   case FL_UNFOCUS:
     return 1;
   break;
  }
return 0;
}
void edit_area::draw()
{
int i,j;
int ti,tj,p;
int Direction=1;	
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
}
void EditBoardDialog(XQControl *ctl)
{

	printf("Edit\n");
	XQCTL=ctl;
if(!dialog)	
	dialog=new edit_board_dialog();
	dialog->show();
	dialog->box.Select=0;
	printf("exit edit");
}

