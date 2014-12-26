#include "control.h"
#include <stdio.h>
#include <unistd.h>
#include <FL/fl_draw.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Hold_Browser.H>
#ifdef WIN32
class sim_area:public Fl_Widget
{
	public:
	sim_area(int,int);
	void draw();	
	int handle(int);
};
sim_area::sim_area(int x,int y)
	:Fl_Widget(x,y,100,100)
{
}
class client_board_dialog: public Fl_Window
{
	public:
	client_board_dialog();
	void save();
	sim_area box;
	Fl_Hold_Browser prog;
	Fl_Check_Button canset;
	Fl_Check_Button dnd;
};
static	client_board_dialog *dialog=NULL;
static unsigned char *img=NULL;
static int img_width=0,img_height=0;
static int dx=0,dy=0,isdnd=0;
static int bx1=0,by1=0,bx2=100,by2=100;
static char title_text[256]="Chinese Chess Learner";
BOOL CALLBACK sm_func(HWND hwnd,LPARAM data)
{
	char s[256];
	RECT rc,drc;
	int isflip;
	s[0]=0;
	GetWindowText(hwnd,s,256);
	if(strcmp(s,title_text)) return TRUE;
	YX from=data>>8,to=data&0xff;
	int x1,y1,x2,y2,swidth,sheight;
	isflip=data>>16;
	x1=YX_X(from);
	y1=YX_Y(from);
	x2=YX_X(to);
	y2=YX_Y(to);
	printf("%d %d %d %d isflip=%d\n",x1,y1,x2,y2,isflip);
	if(!isflip) {x1=8-x1;x2=8-x2;y1=9-y1;y2=9-y2;}
	printf("%d %d %d %d\n",x1,y1,x2,y2);
	x1=min(bx1,bx2)+abs(bx2-bx1)*x1/8;
	x2=min(bx1,bx2)+abs(bx2-bx1)*x2/8;
	y1=min(by1,by2)+abs(by2-by1)*(9-y1)/9;
	y2=min(by1,by2)+abs(by2-by1)*(9-y2)/9;
	printf("%d %d %d %d\n",x1,y1,x2,y2);
		GetWindowRect(GetDesktopWindow(),&drc);    
		swidth=drc.right;
		sheight=drc.bottom;
		GetWindowRect(hwnd,&rc);
		printf("b1 %d %d\n",rc.left,rc.top);
		SetForegroundWindow(hwnd);
		printf("a1\n");
		mouse_event(
		MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE|MOUSEEVENTF_LEFTDOWN,
				65536*(rc.left+x1)/swidth,
				65536*(rc.top+y1)/sheight,0,0);
if(!isdnd){	mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
		mouse_event(
		MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE|MOUSEEVENTF_LEFTDOWN,
				65536*(rc.left+x2)/swidth,
				65536*(rc.top+y2)/sheight,0,0);	
		mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
	}
else
		mouse_event(
		MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE|MOUSEEVENTF_LEFTUP,
				65536*(rc.left+x2)/swidth,
				65536*(rc.top+y2)/sheight,0,0);	
	
		printf("a2\n");
	return FALSE;
}
void send_move(YX from,YX to,int flip)
{
	printf("send move %d\n",flip);
	EnumWindows(sm_func,(LPARAM)((flip<<16)|(from<<8)|to));
	printf("exit send move");
}

extern void ClientBoardDialog();
static void prog_cb(Fl_Widget *pg,void *data)
{
	strcpy(title_text,dialog->prog.text(dialog->prog.value()));
	ClientBoardDialog();
	dialog->save();
}
static void dnd_cb(Fl_Widget *pg,void *data)
{
	isdnd=dialog->dnd.value();
	dialog->save();
}
client_board_dialog::client_board_dialog():
	Fl_Window(500,300,"client board setup"),
	box(200,0),
	prog(0,30,200,150),
	canset(1,3,50,10,"Set"),
	dnd(0,16,50,10,"DnD")

{
	prog.callback(prog_cb,NULL);
	dnd.callback(dnd_cb,NULL);
	FILE *f=fopen("client.sav","r");
	if(!f) return;
	fgets(title_text,256,f);
	for(int i=strlen(title_text)-1;
	i>=0 &&(title_text[i]=='\n' || title_text[i]=='\r');i--)
		title_text[i]=0;
	printf("[%s]\n",title_text);
	fscanf(f,"%d %d %d %d %d\n",&bx1,&by1,&bx2,&by2,&isdnd);
	printf("%d %d %d %d %d\n",bx1,by1,bx2,by2,isdnd);
	fclose(f);
	dnd.value(isdnd);
}
void client_board_dialog::save()
{
	FILE *f=fopen("client.sav","w");
	if(!f) return;
	fprintf(f,"%s\n",title_text);
	fprintf(f,"%d %d %d %d %d\n",bx1,by1,bx2,by2,isdnd);
	fclose(f);
}
int sim_area::handle(int event)
{
	static int ss=0;
	switch(event)
	{
		case FL_PUSH:
			if(dialog->canset.value())
			{
			if(ss) {bx1=dx+Fl::event_x()-200;
				by1=dy+Fl::event_y();}
			else
				{bx2=dx+Fl::event_x()-200;
				by2=dy+Fl::event_y();}
			printf("%d,%d  %d,%d\n",bx1,by1,bx2,by2);
			if(ss) {dialog->canset.value(0);dialog->save();}
			ss=!ss;
			dialog->box.redraw();
			}
			return 1;
		case FL_FOCUS:
		case FL_UNFOCUS:
			return 1;
	}
	return 0;
}
void sim_area::draw()
{
	if(img)
	fl_draw_image(img,x(),y(),img_width,img_height);
	fl_color(0,255,0);
	fl_arc(x()+bx1-3-dx,y()+by1-3-dy,6,6,0,360);
	fl_arc(x()+bx2-3-dx,y()+by2-3-dy,6,6,0,360);
}
BOOL CALLBACK t_func(HWND hwnd,LPARAM data)
{
	char s[256];
	RECT rc,drc;
	unsigned color;
	if(!IsWindowVisible(hwnd)) return TRUE;	
	s[0]=0;
	GetWindowText(hwnd,s,256);
	WINDOWINFO winfo;
	winfo.cbSize=sizeof(WINDOWINFO);
	GetWindowInfo(hwnd,&winfo);
	if(winfo.rcClient.bottom<0) return TRUE;
	dialog->prog.add(s);
	if(strcmp(s,title_text)) return TRUE;
	SetForegroundWindow(hwnd);
	sleep(1);
	dx=winfo.rcClient.left-winfo.rcWindow.left;
	dy=winfo.rcClient.top-winfo.rcWindow.top;
	img_height=winfo.rcWindow.bottom-winfo.rcWindow.top;
	img_width=winfo.rcWindow.right-winfo.rcWindow.left;
	printf("%d %d\n",img_width,img_height);
	SetForegroundWindow(hwnd);
	HDC hdc=GetDC(hwnd);
	HDC memdc=CreateCompatibleDC(hdc);
	HGDIOBJ membm = CreateCompatibleBitmap(hdc, 
                     GetDeviceCaps(hdc, HORZRES), 
                     GetDeviceCaps(hdc, VERTRES)); 
	SelectObject ( memdc, membm );
	img_height=winfo.rcClient.bottom-winfo.rcClient.top;
	img_width=winfo.rcClient.right-winfo.rcClient.left;
	if(img) delete img;
	img=new unsigned char[3*img_height*img_width];
	BitBlt(memdc,0,0,img_width,img_height,hdc,0,0,SRCCOPY);
	for(int y=0;y<img_height;y++)
	for(int x=0;x<img_width;x++)
	{
		color=GetPixel(memdc,x,y);
		img[3*img_width*y+3*x]=color&0xff;
		img[3*img_width*y+3*x+1]=(color>>8)&0xff;
		img[3*img_width*y+3*x+2]=(color>>16)&0xff;
		
	}
	DeleteDC(memdc);
	DeleteObject(membm);
	dialog->resize(dialog->x(),dialog->y(),200+img_width,max(img_height,200));
	dialog->box.resize(200,0,img_width,img_height);
	dialog->box.redraw();
	dialog->redraw();
	return TRUE;
}
void ClientBoardDialog()
{
	printf("test\n");
if(!dialog)	
	dialog=new client_board_dialog();
	dialog->prog.clear();
	EnumWindows(t_func,(LPARAM)0);
	dialog->show();
	printf("exit test");
}
#else
void ClientBoardDialog()
{
}

void send_move(YX from,YX to,int flip)
{
}
#endif
