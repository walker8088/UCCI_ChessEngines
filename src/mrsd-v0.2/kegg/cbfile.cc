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

#include "cbfile.h"
#include <ctype.h>
char *XQFile::KeyEng[]={"GAME","RESULT","START","TIME","DATE","RED","BLACK","PLACE","INIT"};
char *XQFile::KeyChn[]={"棋局","Result","開始","用時","時間","紅方","黑方","地點","設定"};
static char *ResultText[]={"time","Resign","Checkmate"};
XQFile::XQFile(int n)
{
	XQFile("\xff");
	SetFileName(n);
}
XQFile::XQFile(char *FN){
int i;
if(FN==NULL) FN=NONE;
MaxRemark=0;
Comment[0]=NONE;
file=NULL;
for(i=0;i<KEYS;i++)KeyWord[i][0]=0;
SetFileName(FN);
Result=3;
}
void XQFile::SetFileName(int n)
{
	SetFileName("\xff\xff\xff\xff");
	memcpy(&FileName[4],&n,sizeof(int));
}
void XQFile::SetFileName(const char *s)
{if(file!=NULL) fclose(file);
file=NULL;
strncpy(FileName,s,256);
}
void XQFile::PrintComment(int i)
{
int j;
if(LoadComment(i)==NONE) return;
fprintf(file,"# ");
	int NL=0;
	for(j=0;j<strlen(Comment[i]);j++)
	if(Comment[i][j]!='\r')
		{
		if(NL) {fprintf(file,"# ");NL=0;}
		fprintf(file,"%c",Comment[i][j]);
		if(Comment[i][j]=='\n') NL=1;
		}
	if(!NL) fprintf(file,"\n");

}

int XQFile::Save(XQ &bd,int cnt,int style)
{
int i,j;
YX s,d;
char *ptr;
if(file!=NULL && !cnt)
{fclose(file);
file=NULL;
}
if(FileName[0]=='\xff') return 0;
if(file==NULL || !cnt)
file=fopen(FileName,"w+");
if(file==NULL) return 0;
fprintf(file,"GAME\n");
for(i=0;i<KEYS;i++)
    if(KeyWord[i][0]) 
      fprintf(file,"%.10s %s\n",KeyEng[i],KeyWord[i]);
      switch(Result&0xf)
      {
        case  0:
	fprintf(file,"Result 1-0");
	break;
	case 1:
	fprintf(file,"Result 0-1");
	break;
	case 2:
	fprintf(file,"Result 0.5-0.5");
	break;
	default:
	fprintf(file,"Result 0-0");
	}	
      switch(Result>>4)
      {
        case  0:
	fprintf(file,"(Forfeit on time)\n");
	break;
	case 1:
	fprintf(file,"(Resign)\n");
	break;
	case 2:
	fprintf(file,"(Checkmate)\n");
	break;
	default:
	fprintf(file,"\n");
	}	

fprintf(file,"START{\n");
for(i=0;i<bd.MaxPly();i+=2)
{
PrintComment(i);
bd.movepos(s,d,i);
fprintf(file,"%d. %c%d-%c%d",
i/2+1,'A'+YX_X(s),YX_Y(s),'A'+YX_X(d),YX_Y(d));
if(i+1 >=bd.MaxPly()) break;
bd.movepos(s,d,i+1);
if(LoadComment(i+1)!=NONE)
	{fprintf(file," ... \n");
	PrintComment(i+1);
	fprintf(file,"%d.  ... %c%d-%c%d\n"
,i/2+1,'A'+YX_X(s),YX_Y(s),'A'+YX_X(d),YX_Y(d));
	}
else 
	fprintf(file," %c%d-%c%d\n",
	'A'+YX_X(s),YX_Y(s),'A'+YX_X(d),YX_Y(d));		


}
PrintComment(bd.MaxPly());
fprintf(file,"}\n");
fclose(file);
file=NULL;
return 1;
}


static char *StripHeadTail(char *str)
{
int i;
for(i=strlen(str)-1;isspace(str[i]) && i>=0 ;i--) str[i]=0;
for(i=0;isspace(str[i]) & i;i++);
return &str[i];

}
//VIM Hack
int XQFile::Load(XQ &bd,int cnt)
{
int i,start=0,j,undos=0;
char str[CF_LINEMAX+1],*ptr;
if(file!=NULL && !cnt)
{fclose(file);file=NULL;}
if(file==NULL || !cnt)
{
	if(FileName[0]!='\xff')
		file=fopen(FileName,"rb");
	else{
		file=fopen("collect.1","rb");
	}
}
if(file==NULL) return 0;
if(FileName[0]=='\xff')
{
int nn=*(int *)(&FileName[4]);
char c;
unsigned short n,m;
	if(fseek(file,0,0)) return 0;
	if(fread(&i,4,1,file)<=0) return 0;
	if(nn>=i) return 0;
	if(fseek(file,4*(nn+1),0)) return 0;
	if(fread(&i,4,1,file)<=0) return 0;
	if(fseek(file,i,0)) return 0;
	if(fread(&c,1,1,file)<=0) return 0;
	if(fread(&n,2,1,file)<=0) return 0;
	if(c=='R') Result=0x10;
	else if(c=='B') Result=0x11;
	else if(c=='T') Result=0x12;
	else {printf("記錄Error\n");return 0;}
//	printf("n=%d c=%c\n",n,c);
	bd.GotoStart();
	for(;n>0;n--)
	{
		fread(&m,2,1,file);
		bd.Move(m>>8,m&0xff);
//		printf("%x %d,%d %d,%d\n",m,YX_Y_X(m>>8),YX_Y_X(m&0xff));
	}
	return 1;
}
else if(strlen(FileName)>4 && strstr(FileName,".easy"))
{
	char c;
	unsigned short n,m;
	fread(&c,1,1,file);
	fread(&n,2,1,file);
	printf("n=%d\n",n);
	bd.GotoStart();
	for(;n>0;n--)
	{
		fread(&m,2,1,file);
		bd.Move(m>>8,m&0xff);
		printf("%x %d,%d %d,%d\n",m,YX_Y_X(m>>8),YX_Y_X(m&0xff));
	}
	return 1;
	
}
while(fgets(str,CF_LINEMAX,file)!=NULL)
{	
//	printf("Ply%d:%s",bd.Ply(),str);
	if(!start)
	{
	ptr=strtok(str,"{ \n");
	if(ptr!=NULL)
	for(i=0;i<KEYS;i++) if(!strcasecmp(ptr,KeyEng[i])) break;
	if(i <KEYS)
		{
			switch(i)
			{
				case 0:bd.init(); break;
				case 1:{float r,b;
					ptr=strtok(NULL," \n");
					if(ptr!=NULL)
					{sscanf(ptr,"%f-%f",&r,&b);
					ptr=strtok(NULL,"\n");
					}
					if(ptr==NULL)
					ptr=NONE;
				if(r==1) Result=0;
				else if(b==1) Result=1;
				else if(r==0.5 && b==0.5) Result=2;
				else Result=3;
				for(j=0;j<3 && !strstr(ptr,ResultText[j]);j++);
					Result|=(j<<4);
				//fprintf(stderr,"Result=%x\n",Result);
				}
				break;
			case 2:start=1;break;
			default:
				ptr=strtok(NULL,"\n");
				strncpy(KeyWord[i],StripHeadTail(ptr),256);
			//fprintf(stderr,"[%s]\n",KeyWord[i]);
			}/*switch*/
		}
#ifndef VC
		else fprintf(stderr,"%s:unknow Command %s\n",FileName,ptr);
#endif		
	}/*if start*/
	else if(str[0]=='#')
		AddComment( &str[1], bd.Ply() );
	else
	{
	ptr=strtok(str," \t\n");
	if(ptr[0]=='}') {start=0;return 2;}
	else if(!strcmp("GAME",ptr)){bd.init();start=0;
#ifndef VC		
		fprintf(stderr,"%s:GAME hack!!\n",FileName);
#endif
		return 2;}
		else 
		{
		SL_HACK:
		if(atoi(ptr) !=bd.Ply()/2+1+undos)		
#ifndef VC
			fprintf(stderr,"%s:ErrorPly%d:%s",FileName,bd.Ply(),ptr);
#else
			printf("%s:ErrorPly%d:%s",FileName,bd.Ply(),ptr);
#endif		
			 ptr=strtok(NULL," \n"); 
			 if(ptr!=NULL)
	 			{
	 			if(ptr[0]=='}') return 2;
	 			if(strlen(ptr)>=5)
	 				{
					if(!bd.Move(ptr[1]-'0',ptr[0]-'A',ptr[4]-'0',ptr[3]-'A'))
#ifndef VC						
					fprintf(stderr,"%s:%d:Move Err\n",FileName,bd.Ply());
#else					
					printf("%s:%d:Move Err\n",FileName,bd.Ply());
#endif
					
					}
					else if(!strcasecmp(ptr,"undo"))
						{
						bd.Back();
						//fprintf(stderr,"%s:undo move!!\n",FileName);
						undos++;
						}
					ptr=strtok(NULL," \n"); 
					if(ptr!=NULL)	
			 		{
			 		if(ptr[0]=='}') return 2;
					 		if(strlen(ptr)>=5)
	 						{
						if(!bd.Move(ptr[1]-'0',ptr[0]-'A',ptr[4]-'0',ptr[3]-'A'))
						{
#ifndef VC							
						fprintf(stderr,"%s:%d:Move Err\n",FileName,bd.Ply());
#endif						
						}
							}
						else if(!strcasecmp(ptr,"undo"))
						{
						bd.Back();
						//fprintf(stderr,"%s:undo move!!\n",FileName);
						undos++;
						}
						else if(atoi(ptr)>0)
						{
#ifndef VC							
						fprintf(stderr,"%s:same lin hack!!\n",FileName);
#endif
						goto SL_HACK;
						}
						/*if(strlen(ptr)>=5)*/
					}/*if(ptr!=NULL)*/
				}/*if(ptr!=NULL)*/
					
		}/*else*/
	}/*else*/	
	}/*while*/
fclose(file);
file=NULL;
bd.GotoStart();
return start;
}

void XQFile::CleanComment(int start)
{
int i;
for(i=start;i<MaxRemark;i++)
if(Comment[i]!=NONE){
  delete[] Comment[i];
  Comment[i]=NONE; }
 MaxRemark=start;
 Comment[start]=NONE;
}
void XQFile::SetComment(char *s,int n)
{
if(n>1499) n=1499;
if(n>=MaxRemark)
for(;MaxRemark<=n;MaxRemark++)
Comment[MaxRemark]=NONE;

	if(s==NULL) s=NONE;
		if(strlen(s)==0) return;
	if(Comment[n]!=NONE) delete Comment[n];
	  Comment[n]=new char[strlen(s)+1];
				 strcpy(Comment[n],s);
}
void XQFile::AddComment(char *s,int n)
{
char *ptr;
	if(s==NULL) s=NONE;
		if(strlen(s)==0) return;
if(n>1499) n=1499;
if(n>=MaxRemark)
for(;MaxRemark<=n;MaxRemark++)
Comment[MaxRemark]=NONE;

    ptr=Comment[n];
	  Comment[n]=new char[strlen(s)+strlen(ptr)+1];
	      strcpy(Comment[n],ptr);
             strcpy(&Comment[n][strlen(ptr)],s);
		if(ptr!=NONE)	 delete[] ptr;
}

char * XQFile::LoadComment(int n)
{
if(n>=MaxRemark) return NONE;
return Comment[n];
}

char * XQFile::GetKeyWord(int n)
{
if(n>=KEYS) return NONE;
return KeyWord[n];
}
char * XQFile::GetKeyWord(const char *kname)
{
int i;
for(i=0;i<KEYS;i++)
if(!strcasecmp(kname,KeyEng[i])) return KeyWord[i];
for(i=0;i<KEYS;i++)
if(!strcmp(kname,KeyChn[i])) return KeyWord[i];
return NONE;
}
