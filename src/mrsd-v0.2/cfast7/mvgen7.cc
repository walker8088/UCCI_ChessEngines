/*
Modify it under the terms of the GNU General Public License
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
#include "bd.h"
#include <stdio.h>
#include <stdlib.h>
#define NULLSTEP
#define CFLEVEL 3
#define D(x)
//#define D(x) {x;fflush(stdout);}
static BD *bh=new BD[HASHSIZE];
static BD *bh2=new BD[HASHSIZE];
static int MoveOrder[16]={rook1<<1,rook2<<1,cannon1<<1,cannon2<<1,
                          knight1<<1, knight2<<1,pawn1<<1,pawn2<<1,
                          pawn3<<1,pawn4<<1,pawn5<<1,king<<1,
                          minister1<<1,minister2<<1,guard1<<1,guard2<<1};
BD *Get_HASH(ull sig)
{
	BD *b1;
	b1=&bh[sig&HASHMASK];
	if(b1->signiture==sig)
		return b1;
	return NULL;

}
int PuzzleFlag=0;
void  HashClean()
{
for(int i=0;i<HASHSIZE;i++)
{
  bh[i].Clean();
  bh2[i].Clean();
}  
}
enum {NoUse,UseUpperBound,UseLowerBound,AdjustBounds,Replaceable,AsHint,Usable};
inline static int HashLookUp(BD *b1,XQAI *b,int LowerBound,int UpperBound,int s)
{	
	if(b1->color==-1) return Replaceable;
	if(!(*b1 == *b))
	 return ( s > b1->sdepth /*|| 
		(s==b1->sdepth && UpperBound-LowerBound>200 && UpperBound-LowerBound>b1->ub-b1->lb)*/)
		? Replaceable : NoUse;
//	有殺情形
	if(b1->ub<-8000 && LowerBound>=b1->ub) return UseUpperBound;
	if(b1->lb> 8000 && UpperBound<=b1->lb) return UseLowerBound;
	if(s > b1->sdepth) return b1->move ? Usable : Replaceable;
	if(b1->ub<=LowerBound) return UseUpperBound;
	if(b1->lb>=UpperBound) return UseLowerBound;
	if(b1->ub == b1->lb) return UseUpperBound;
	if(s < b1->sdepth) return b1->move ? AsHint : NoUse;
	return b1->move ? Usable : AdjustBounds;
}
int NumberOfPieces[2];
inline void XQAI::addgm(ushort mv)
{
BestMove[_Ply]=mv;
#ifdef PVLINE
pline[_Ply&63]=pline[(_Ply+1)&63];
#endif
}
#define _mm_7(p,a,b) (p ? mm_7(p,a,b):mm_quiet(p,a,b))
#define wsdepth sdepth
int OrigBound,OrigTurn;

MoveNode root_move[114];
int root_move_n;
#if 0
MoveNode root1_move[114];
int root1_move_n;
static void backup_list()
{
	for(int i=0;i<root_move_n;i++)
		root1_move[i]=root_move[i];
		root1_move_n=root_move_n;
}
static void restore_list()
{
	for(int i=0;i<root1_move_n;i++)
		root_move[i]=root1_move[i];
		root_move_n=root1_move_n;
}
#endif
void up_list(int n)
{
for(int i=n;i>0;i--)
	if(root_move[i]>root_move[i-1])
		swap(root_move[i],root_move[i-1]);
	else return;
}
static int ch[300];
int XQAI::IsCheck(int BackPly)
{
int i;
if(BackPly && ch[_Ply-BackPly]) return ch[_Ply-BackPly]!=-1;
for(i=0;i<BackPly;i++) Back();
YX OpponetKing=cch_p[Turn()+2];
//	XQ::Rule doesn't check who's turn
for(int j=(knight1<<1)+(!Turn());j<34;j+=2)
	if(CaptureRule(j,OpponetKing)) {
for(i=0;i<BackPly;i++) Forward();
		return ch[_Ply-BackPly]=1;
		//(cch_p[j]<<8)|OpponetKing);
	}
for(i=0;i<BackPly;i++) Forward();
ch[_Ply-BackPly]=-1;
return 0;
}
int XQAI::mm_7(int sdepth,int alpha,int beta)
{
  int used_mv=0,v,bv=-20000,CallPly=sdepth-1,yy=0,ov,mp;
  ushort bmv=0;
  MoveList ml;
  YX yx,pyx,UsePVS=0;
  int clr=Turn(),oldalpha=alpha;
  BD *b1=NULL,*b2=NULL;
  ov=v= clr ? -Value() : Value();
  if(abs(v)>=8000 || stop_thinking) {info.TotalNodes++;return v;}
  beta=min(beta,10000-_Ply-1);

D(printf("%d [%s] %d %d %d %d\n",sdepth,Chinese_Notation(),v,alpha,beta,_Ply));
/*IsCheck和吃王*/
YX Opponent=cch_p[2|(!clr)];
for(int i0=0;i0<12;i0++)
	if(CaptureRule(MoveOrder[i0]|clr,Opponent))
			return 10000-_Ply-1;
if(yy=IsCheck()) {
	CallPly= sdepth;//sdepth<InitDepth-1 ?sdepth:CallPly;
}
/*處裡連將殺*/
else if(PuzzleFlag)
{
	if(clr==1) return 8000;
}
//if(!yy && v< alpha-400) return v+400;
/*檢查重複Image*/
  if(_Ply>=in_nullstep+4 && hvalue[_Ply]==hvalue[_Ply-4])
  {
  	if(yy && IsCheck(2))
		return (IsCheck(1)&&IsCheck(3)) ? 0 : 10000-_Ply+5;
	else if(IsCheck(1) && IsCheck(3))
		return -10000+_Ply-6;
	else 
		return OrigTurn==Turn() ? -6000 : 6000;
	// 判斷Rule很難，自認倒楣。
	// 這裡只有判斷long將，long捉，一將一捉等等會判斷Error。
  }
/*HASH*/
  if((sdepth>=2 || beta-alpha==OrigBound) && !in_nullstep)
  {
	b1=&bh[hValue()&HASHMASK];
	b2=&bh2[hValue()&HASHMASK];
  	switch(HashLookUp(b1,this,alpha,beta,wsdepth)) {
		case Replaceable:break;
		case NoUse:
			b1=NULL;
			break;
		case UseUpperBound:
			info.TotalNodes++;
			return b1->ub;
		case UseLowerBound:
			info.TotalNodes++;
			return b1->lb;
		case AdjustBounds:
			alpha=max(alpha,int(b1->lb));
			beta =min(beta,int(b1->ub));
		case Usable:
			used_mv=b1->move;
			break;
		case AsHint:
			used_mv=b1->move;
			b1=NULL;
			break;
	}
	if(b1==NULL && b2!=NULL) 
	{
  	switch(HashLookUp(b2,this,alpha,beta,wsdepth)) {
		case Replaceable:break;
		case NoUse:break;
		case UseUpperBound:
			info.TotalNodes++;
			return b2->ub;
		case UseLowerBound:
			info.TotalNodes++;
			return b2->lb;
		case AdjustBounds:
			alpha=max(alpha,int(b2->lb));
			beta =min(beta,int(b2->ub));
		case Usable:
			used_mv=b2->move;
			break;
		case AsHint:
			used_mv=b2->move;
			break;
	}
	}
  }
/*殘局*/
	/*N/A*/
/*NULL MOVE*/
  if(!yy && NumberOfPieces[clr] > 5 && !in_nullstep && sdepth >=5  && beta<8000) {
   	Move();
   	in_nullstep=_Ply;
  	v=-mm_7(sdepth-3,-beta,1-beta);
   	in_nullstep=0;
	Back();
  	if(v>=beta) {
		if(b1) b1->Set(this,oldalpha,beta,sdepth,v,0);
		return v;
	}
	//if(v<-40000)printf("催殺\n");
  }
//新增
//if(!CallPly && v+500<alpha) return alpha;
#if 1 
if(!CallPly)
{
mp=YX_isalive(cch_p[(rook1<<1)|(!clr)])
		  ||YX_isalive(cch_p[(rook1<<2)|(!clr)])
		  ? 420: 192;
if(v+mp<alpha)
{
	D(printf("futile cut %d\n",v+mp));
	return v+mp;
}
}
#endif
//結束新增
/*Try Some Good Move First*/
#if 0
  if( !yy && sdepth==InitDepth-1 && InitDepth>7 && beta-alpha> 50)
  {
  	backup_list();
	gen_list(this);
	v=mm_pv(min(6,sdepth-2) ,alpha,beta);
	v=mm_pv(sdepth,alpha,beta);
	restore_list();
	if(b1) b1->Set(this,oldalpha,beta,wsdepth,v,BestMove[_Ply]);
	return v;
  }
#endif
/*如果在PV中，且沒有 hash_move 試試看淺層搜尋的Result*/
  if(!used_mv && beta-alpha==OrigBound && sdepth>=3) {
D(printf("try shallow\n"));
  	v=_mm_7(min(6 ,sdepth-2),alpha,beta);
  }
#ifdef VC
  used_mv=used_mv ? used_mv : BestMove[_Ply];
#else
  used_mv= used_mv ? : BestMove[_Ply];
#endif
  if(_Move(used_mv,3))
  {
//	if(yy) BestMove[_Ply]=yy;
D(printf("try used mv	\n"));
	v=-_mm_7(CallPly,-beta,-alpha);
D(printf("return2 %d [%s] %d %d %d %d\n",sdepth,Chinese_Notation(),v,alpha,beta,_Ply));
	Back();
	UsePVS=1;
	if(v>bv){
		bmv=used_mv;
		if((bv=v)>alpha) {
		if((alpha=v)>=beta){
			addgm(bmv);
			if(b1) 
			{
			*b2=*b1;
			b1->Set(this,oldalpha,beta,sdepth,v,bmv);
			}
			else if(b2)
			b2->Set(this,oldalpha,beta,sdepth,v,bmv);

			return v;
		}
		UsePVS=1;
		}
	}
  }
  else used_mv=0;
/*Normal*/
//這個部分也不是完美的，58+5是最大的Move差距(兵卒)，如果現在的值比alpha小很多，
//那應該需要吃子才能避免被cut,這部份沒問題，但最後return的值會比理想值小一點。
//但是如果沒有好的吃子步，v又比alpha小很多，可以while成一種不好的case來扣分。
//下面是一些嘗試性的解法
int cf_flag=0;
AGAIN:  
#if 0
  if(!cf_flag && !CallPly && ov+58<alpha && !yy)
{
gam_cf(ml);
cf_flag=1;
}
  else
#endif  
{
  cf_flag=0;
  gam_easy(ml);
}
  for(int eat_level=0;eat_level<6;eat_level++)
  	for(int t=0;t<ml.lmoves[eat_level];t++)
	   {
	        register unsigned int temp=ml.lmv[eat_level][t];
		if( used_mv != temp )
		{
		  pyx=temp>>8;
		  yx=temp&0xff;	
		if( _bd(pyx)>=16 || RelativeRule(pyx,yx) )  {
			Move(pyx,yx,2);
			if(sdepth>1 && UsePVS && beta-alpha>2) {
				v=-_mm_7(CallPly,-alpha-1,-alpha);
				if(v>alpha && v<beta){
				v= max(v,-_mm_7(CallPly,-beta,-v));
				}
			}
			else 	v=-_mm_7(CallPly,-beta,-alpha);
D(printf("return3 %d [%s] %d %d %d %d\n",sdepth,Chinese_Notation(),v,alpha,beta,_Ply));
			Back();
			if(v>bv){
				bmv=(pyx<<8)|yx;
				if((bv=v)>alpha) {UsePVS=1;
				if((alpha=v)>=beta){
					addgm(bmv);
					if(b1) 
					{
					    *b2=*b1;
					    b1->Set(this,oldalpha,beta,
					    sdepth,v,bmv);
					}
					else if (b2)
					    b2->Set(this,oldalpha,beta,
					    sdepth,v,bmv);
					
					return v;
				}
				}
			}
		}}//if
	   }//for
	if(bv==-20000)
		{
			if(cf_flag==1){
				/*if(NumberOfPieces[clr]>5)*/ return ov+5;
				//cf_flag=-1;
				//goto AGAIN;
			}
			return -10000+_Ply;
		}
		
	if(bv<-9000)
		{
			if(cf_flag==1){
				goto AGAIN;
			}
		}
	    
	   if(bmv) addgm(bmv);
	   if(b1) {*b2=*b1;
		   b1->Set(this,oldalpha,beta,sdepth,bv,bmv);}
	   else if(b2) b2->Set(this,oldalpha,beta,sdepth,bv,bmv);
	   return bv;
}
int XQAI::mm_pv(int sdepth,int alpha,int beta)
{
  int v,bv=-20000;
  ushort bmv=0,org_move[114];
  for(int i=0;i<root_move_n;i++) org_move[i]=root_move[i].move; 
  for(int t=0;t<root_move_n;t++) {
	_Move(org_move[t],2);
D(printf("%d [%d %s] %d %d %d\t",sdepth,t,Chinese_Notation(),alpha,beta,_Ply));
	if(sdepth>5 && t>0 && beta-alpha>2) {
		v=-mm_7(sdepth-1,-alpha-1,-alpha);
		if(v>alpha && v<beta) 
			v=max(v,-mm_7(sdepth-1,-beta,-v));
			
	}
	else 	v=-mm_7(sdepth-1,-beta,-alpha);
D(printf("return %d \n",v));
	Back();
	if(stop_thinking)
	{
		if(bmv) addgm(bmv);
		return bv;
	}
	if(v>alpha) root_move[t].ub=root_move[t].lb=v;
		else {root_move[t].ub=v;root_move[t].lb=-20000;}
	up_list(t);
	if(v>bv) {
		bv=v;
		bmv=org_move[t];
		alpha=max(v ,alpha);
		if(alpha>=beta) {
 			addgm(bmv);
			return v;
		}
	}
  }
 if(bmv) addgm(bmv);
 return alpha;
}

int XQAI::mm_root(int sdepth,int alpha,int beta)
{
  int i;
  for(i=0;i<300;i++) ch[i]=0;
  for(NumberOfPieces[0]=NumberOfPieces[1]=0,i=2;i<34;i++) 
  	if(YX_isalive(cch_p[i]))
		NumberOfPieces[i&1]++;
  InitDepth=sdepth;
  OrigBound=beta-alpha;
  OrigTurn=Turn();
  return mm_pv(sdepth,alpha,beta);
}

