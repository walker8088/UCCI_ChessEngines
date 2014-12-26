/*! \file evaluation.cpp
@brief 局面评估类（单件模块）
*/

#include "evaluation.h"

int Evaluation::Eval(Board & cur, PLAYER who)
{
    int total = 0;
    //局面价值等于各种价值加权和 
    total += ValueAndPosition(cur, who);
    total += Agility(cur, who);
    if (cur.numOfChess < 14)
    {
        total += AttackAndProtect(cur, who);
    }
    //total += Cooperation(cur, who);
    return total;    
}

int Evaluation::ValueAndPosition(Board & cur, PLAYER who)
{
    int R_value = 0, B_value = 0;
    BDPOINT index;
    for (index = 0x33; index <= 0xcb; index++)
    {
        if (cur.position[index] == NONE || cur.position[index] == OUT)
        {
            continue;    
        }
        //棋子价值评估与子力位置评估
        if (IsRed(cur.position[index]))
        {
            if (cur.numOfChess >= 28)
            {
                R_value += kindValue[START_STATE][cur.position[index]];                 
            }
            else if (cur.numOfChess >= 14)
            {
                R_value += kindValue[MID_STATE][cur.position[index]];   
            }
            else
            {
                R_value += kindValue[END_STATE][cur.position[index]];
            }
            R_value += posValue[cur.position[index]][index];
        }
        else
        {
            if (cur.numOfChess >= 28)
            {
                B_value += kindValue[START_STATE][(CHESSMAN)(NONE - (cur.position[index]))];                 
            }
            else if (cur.numOfChess >= 14)
            {
                B_value += kindValue[MID_STATE][(CHESSMAN)(NONE - (cur.position[index]))];   
            }
            else
            {
                B_value += kindValue[END_STATE][(CHESSMAN)(NONE - (cur.position[index]))];
            }
            B_value += posValue[(CHESSMAN)(NONE - (cur.position[index]))][(BDPOINT)(0xfe - index)];
        }
    }   
    //返回双方棋子位置评估值的差值 
    if (who == RED)
    {
        return (R_value - B_value);
    }
    else
    {
        return (B_value - R_value);
    }
}

int Evaluation::Agility(Board & cur, PLAYER who)
{
    int sum = 0;
    CHESSMAN chess;
    for (chess = 1; chess < 8; chess++)
    {
        sum += (agilityValue[chess] * cur.numOfMove[chess]);
    }
    return sum;
}

int Evaluation::AttackAndProtect(Board & cur, PLAYER who)
{
	int sum = 0;
	int IsAttacked = 1, i;
	BDPOINT tar, leg, legback;
	//被威胁、保护模块的统一
	for (tar = cur.lastmove.to - 0x10; (tar & 0xf0) >= 0x30; tar -= 0x10) //上方 
	{
		if (cur.position[tar] == NONE) continue;
		else 
		{
			if ((tar == cur.lastmove.to - 0x10) && (cur.position[tar] == B_PAWN)) 
			{
				if (who == RED) //受卒威胁减分
				{
					sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked = 0; break;
				} 
				else //被卒保护加分 
				{
					sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); break;
				}
			} 
			else if ((who == RED) ? (cur.position[tar] == B_ROOK) : (cur.position[tar] == R_ROOK)) //受车威胁减分 
			{
				sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked = 0; break;
			} 
			else if ((who == BLACK) ? (cur.position[tar] == B_ROOK) : (cur.position[tar] == R_ROOK)) //被车保护加分 
			{
				sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); break;
			}
			else
			{
				for (tar -= 0x10; (tar & 0xf0) >= 0x30; tar -= 0x10) 
				{
					if (cur.position[tar] == NONE) continue;
					else
					{
						if ((who == RED) ? (cur.position[tar] == B_CANNON) : (cur.position[tar] == R_CANNON)) //受炮威胁减分 
						{
							sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked = 0; break;
						} 
						else if ((who == BLACK) ? (cur.position[tar] == B_CANNON) : (cur.position[tar] == R_CANNON)) //被炮保护加分
						{
							sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); break;
						} 
						else break;
					}
				}
				break;
			} 
		}
	}
	for (tar = cur.lastmove.to + 0x10; (tar & 0xf0) <= 0xc0; tar += 0x10) //下方 
	{
		if (cur.position[tar] == NONE) continue;
		else 
		{
			if ((tar == cur.lastmove.to + 0x10) && (cur.position[tar] == R_PAWN))
			{
				if (who == BLACK) //受兵威胁减分
				{ 
					sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked = 0; break;
				} 
				else //被兵保护加分
				{
					sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); break;     
				}
			} 
			else if ((who == RED) ? (cur.position[tar] == B_ROOK) : (cur.position[tar] == R_ROOK)) //受车威胁减分
			{
				sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked= 0; break;
			}
			else if ((who == BLACK) ? (cur.position[tar] == B_ROOK):(cur.position[tar] == R_ROOK)) //被车保护加分 
			{
				sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); break;
			}
			else
			{
				for (tar += 0x10; (tar & 0xf0) <= 0xc0; tar += 0x10) 
				{
					if (cur.position[tar] == NONE) continue;
					else
					{
						if ((who == RED) ? (cur.position[tar] == B_CANNON) : (cur.position[tar] == R_CANNON)) //受炮威胁减分
						{
							sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked = 0; break;
						}
						else if ((who == BLACK) ? (cur.position[tar] == B_CANNON) : (cur.position[tar] == R_CANNON)) //被炮保护加分
						{
							sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); break;
						} 
						else break;
					}
				}
				break;
			} 
		}
	}
	for (tar = cur.lastmove.to - 0x01; (tar & 0x0f) >= 0x03; tar -= 0x01) //左方 
	{
		if (cur.position[tar] == NONE) continue;
		else 
		{
			if (tar == cur.lastmove.to - 0x01) 
			{
				if ((!(tar & 0x80)) && (cur.position[tar] == R_PAWN)) //过河红兵
				{
					if (who == BLACK) //受红兵威胁减分
					{
						sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked = 0; break;
					}
					else //受红兵保护加分
					{
						sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); break;
					}
				} 
				else if ((tar & 0x80) && (cur.position[tar] == B_PAWN)) //过河黑卒
				{
					if (who == RED) //受黑卒威胁减分
					{
						sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked = 0; break;
					}
					else //受黑卒保护加分
					{
						sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); break;
					}
				}
			} 
			else if ((who == RED) ? (cur.position[tar] == B_ROOK) : (cur.position[tar] == R_ROOK)) //受车威胁减分 
			{
				sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked = 0; break;
			} 
			else if ((who == BLACK) ? (cur.position[tar] == B_ROOK) : (cur.position[tar] == R_ROOK)) //被车保护加分 
			{
				sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); break;
			}
			else
			{
				for (tar -= 0x01; (tar & 0x0f) >= 0x03; tar -= 0x01) 
				{
					if (cur.position[tar] == NONE) continue;
					else
					{
						if ((who == RED) ? (cur.position[tar] == B_CANNON) : (cur.position[tar] == R_CANNON)) //受炮威胁减分 
						{
							sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked = 0; break;
						} 
						else if ((who == BLACK) ? (cur.position[tar] == B_CANNON) : (cur.position[tar] == R_CANNON)) //被炮保护加分
						{
							sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); break;
						} 
						else break;
					}
				}
				break;
			} 
		}
	}
	for (tar = cur.lastmove.to + 0x01; (tar & 0x0f) <= 0x0b; tar += 0x01) //右方 
	{
		if (cur.position[tar] == NONE) continue;
		else 
		{
			if (tar == cur.lastmove.to + 0x01)
			{
				if ((!(tar & 0x80)) && (cur.position[tar] == R_PAWN)) //过河红兵
				{
					if (who == BLACK) //受红兵威胁减分
					{
						sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked = 0; break;
					}
					else //受红兵保护加分
					{
						sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); break;
					}
				} 
				else if ((tar & 0x80) && (cur.position[tar] == B_PAWN)) //过河黑卒
				{
					if (who == RED) //受黑卒威胁减分
					{
						sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked = 0; break;
					}
					else //受黑卒保护加分
					{
						sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); break;
					}
				} 
			} 
			else if ((who == RED) ? (cur.position[tar] == B_ROOK) : (cur.position[tar] == R_ROOK)) //受车威胁减分 
			{
				sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked = 0; break;
			} 
			else if ((who == BLACK) ? (cur.position[tar] == B_ROOK) : (cur.position[tar] == R_ROOK)) //被车保护加分 
			{
				sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); break;
			}
			else
			{
				for (tar += 0x01; (tar & 0xf0) <= 0x0b; tar += 0x01) 
				{
					if (cur.position[tar] == NONE) continue;
					else
					{
						if ((who == RED) ? (cur.position[tar] == B_CANNON) : (cur.position[tar] == R_CANNON)) //受炮威胁减分 
						{
							sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked = 0; break;
						} 
						else if ((who == BLACK) ? (cur.position[tar] == B_CANNON) : (cur.position[tar] == R_CANNON)) //被炮保护加分
						{
							sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); break;
						} 
						else break;
					}
				}
				break;
			} 
		}
	}
	//被马威胁、保护的判断
	for(i = 0; i < 8; i++) 
	{
		tar = cur.lastmove.to + cnKnightMoveTab[i];
		legback = cur.lastmove.to + cnHorseLegBackTab[i];
		if (((who == RED) ? (cur.position[tar] == B_KNIGHT) : (cur.position[tar] == R_KNIGHT)) && (cur.position[legback] == NONE)) //被马威胁减分 
		{
			sum -= GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]); IsAttacked = 0; 
		} 
		else if (((who == BLACK) ? (cur.position[tar] == B_KNIGHT) : (cur.position[tar] == R_KNIGHT)) && (cur.position[legback] == NONE)) //受马保护加分 
		{
			sum += GetAnPScore(cur.lastmove.to, cur.position[cur.lastmove.to]);
		}
	}

	//威胁对方模块 
	if (IsAttacked) //如果走法受对方威胁,则无需判断是否威胁对方 
	{
		switch (cur.position[cur.lastmove.to]) 
		{
		case R_PAWN:
			if (cur.lastmove.to & 0x80) //兵未过河
			{
				tar = cur.lastmove.to - 0x10;
				if (cur.position[tar] == NONE) break;
				else 
				{
					if (GetColor(tar) == RED)//保护加分 
					{
						sum += GetAnPScore(tar, cur.position[tar]);  break;
					}
					else //威胁加分
					{
						sum += GetAnPScore(tar, cur.position[tar]);  break;
					} 
				} 
            }
			else 
			{
				for (i = 0; i < 3; i++)
				{
					tar = cur.lastmove.to + R_PawnMoveTab[i];
					if (cur.position[tar] == NONE) continue;
					else
					{
						if (GetColor(tar) == RED) //保护加分 
						{
							sum += GetAnPScore(tar, cur.position[tar]);  break;
						}
						else //威胁加分
						{
							sum += GetAnPScore(tar, cur.position[tar]);  break;  
						} 
					}
				}
			}
			break;
		case B_PAWN:
			if (!(cur.lastmove.to & 0x80)) //兵未过河
			{
				tar = cur.lastmove.to + 0x10;
				if (cur.position[tar] == NONE) break;
				else 
				{
					if (GetColor(tar) == BLACK) //保护加分 
					{
						sum += GetAnPScore(tar, cur.position[tar]);  break; 
					}
					else //威胁加分
					{
						sum += GetAnPScore(tar, cur.position[tar]);  break; 
					} 
				}
			}
			else 
			{
				for (i = 0; i < 3;i++)
				{
					tar = cur.lastmove.to + B_PawnMoveTab[i];
					if (cur.position[tar] == NONE) continue;
					else 
					{
						if (GetColor(tar) == BLACK) //保护加分
						{
							sum += GetAnPScore(tar, cur.position[tar]);  break;
						} 
						else //威胁加分 
						{
							sum += GetAnPScore(tar, cur.position[tar]);  break; 
						} 
					}
				}
			}
			break;
		case R_ROOK:
		case B_ROOK:
			for (tar = cur.lastmove.to - 0x10; (tar & 0xf0) >= 0x30; tar -= 0x10) //上方 
			{
				if (cur.position[tar] == NONE) continue;
				else 
				{
					if ((who == RED) ? (GetColor(tar)) == RED : (GetColor(tar)) == BLACK) //保护加分
					{
						sum += GetAnPScore(tar, cur.position[tar]);  break;
					} 
					else //威胁加分 
					{
						sum += GetAnPScore(tar, cur.position[tar]);  break; 
					}  
				}
			}
			for (tar = cur.lastmove.to + 0x10; (tar & 0xf0) <= 0xc0; tar += 0x10) //下方 
			{
				if (cur.position[tar] == NONE) continue;
				else 
				{
					if ((who == RED) ? (GetColor(tar)) == RED : (GetColor(tar)) == BLACK) //保护加分
					{
						sum += GetAnPScore(tar, cur.position[tar]);  break;
					} 
					else //威胁加分 
					{
						sum += GetAnPScore(tar, cur.position[tar]);  break; 
					}  
				}
			}
			for (tar = cur.lastmove.to - 0x01; (tar & 0x0f) >= 0x03; tar -= 0x01) //左方
			{
				if (cur.position[tar] == NONE) continue;
				else 
				{
					if ((who == RED) ? (GetColor(tar) == RED) : (GetColor(tar)) == BLACK) //保护加分
					{
						sum += GetAnPScore(tar, cur.position[tar]);  break;
					} 
					else //威胁加分 
					{
						sum += GetAnPScore(tar, cur.position[tar]);  break; 
					}  
				}
			}
			for (tar = cur.lastmove.to + 0x01; (tar & 0x0f) <= 0x0b; tar += 0x01) //右方
			{
				if (cur.position[tar] == NONE) continue;
				else 
				{
					if ((who == RED) ? (GetColor(tar) == RED) : (GetColor(tar) == BLACK)) //保护加分
					{
						sum += GetAnPScore(tar, cur.position[tar]);  break;
					} 
					else //威胁加分 
					{
						sum += GetAnPScore(tar, cur.position[tar]);  break; 
					}  
				}
			}
			break;
		case R_CANNON:
		case B_CANNON:
			for (tar = cur.lastmove.to - 0x10; (tar & 0xf0) >= 0x30; tar -= 0x10) //上方 
			{
				if (cur.position[tar] == NONE) continue;
				else 
				{
					for (tar -= 0x10; (tar & 0xf0) >= 0x30; tar -= 0x10)
					{
						if (cur.position[tar] == NONE) continue;
						else 
						{
							if ((who == RED) ? (GetColor(tar) == RED) : (GetColor(tar) == BLACK)) //保护加分
							{
								sum += GetAnPScore(tar, cur.position[tar]);  break; 
							}
							else //威胁加分
							{
								sum += GetAnPScore(tar, cur.position[tar]);  break;    
							}
						}
					}
					break;
				}
			}
			for (tar = cur.lastmove.to + 0x10; (tar & 0xf0) <= 0xc0; tar += 0x10) //下方 
			{
				if (cur.position[tar] == NONE) continue;
				else 
				{
					for (tar += 0x10; (tar & 0xf0) <= 0xc0; tar += 0x10)
					{
						if (cur.position[tar] == NONE) continue;
						else 
						{
							if ((who == RED) ? (GetColor(tar) == RED) : (GetColor(tar) == BLACK)) //保护加分
							{
								sum += GetAnPScore(tar, cur.position[tar]);  break; 
							}
							else //威胁加分
							{
								sum += GetAnPScore(tar, cur.position[tar]);  break;    
							}
						}
					}
					break;
				}
			}
			for (tar = cur.lastmove.to - 0x01; (tar & 0x0f) >= 0x03; tar -= 0x01) //左方
			{
				if (cur.position[tar] == NONE) continue;
				else
				{
					for (tar -= 0x01; (tar & 0x0f) >= 0x03; tar -= 0x01)
					{
						if (cur.position[tar] == NONE) continue;
						else 
						{
							if ((who == RED) ? (GetColor(tar) == RED) : (GetColor(tar) == BLACK)) //保护加分
							{
								sum += GetAnPScore(tar, cur.position[tar]);  break;
							}
							else //威胁加分 
							{
								sum += GetAnPScore(tar, cur.position[tar]);  break;    
							}
						}
					}
					break;
				}
			}
			for (tar = cur.lastmove.to + 0x01; (tar & 0x0f) <= 0x0b; tar += 0x01) //右方
			{
				if (cur.position[tar] == NONE) continue;
				else
				{
					for(tar += 0x01; (tar & 0x0f) <= 0x0b; tar += 0x01)
					{
						if (cur.position[tar] == NONE) continue;
						else 
						{
							if ((who == RED) ? (GetColor(tar) == RED) : (GetColor(tar) == BLACK)) //保护加分 
							{ 
								sum += GetAnPScore(tar, cur.position[tar]);  break;
							}
							else //威胁加分 
							{
								sum += GetAnPScore(tar, cur.position[tar]);  break;    
							}
						}
					}
					break;
				}
			}
			break;
		case R_KNIGHT:
		case B_KNIGHT:
			for(i = 0; i < 8; i++) 
			{
				tar = cur.lastmove.to + cnKnightMoveTab[i];
				leg = cur.lastmove.to + cnHorseLegTab[i];
				if ((cur.position[tar] != NONE) && (cur.position[leg] == NONE))
				{
					if ((who == RED) ? (GetColor(tar) == RED) : (GetColor(tar) == BLACK))//保护加分
					{
						sum += GetAnPScore(tar, cur.position[tar]);  
					} 
					else //威胁加分
					{
						sum += GetAnPScore(tar, cur.position[tar]);  
					}  
				}
			}
			break;
		}
	}
	return sum;
} 

int Evaluation::Cooperation(Board & cur, PLAYER who) 
{
    int sum = 0;
    int flag;
    BDPOINT tar;
    switch (cur.position[cur.lastmove.to]) 
    {
    //--------过河兵手牵手--------//
    case R_PAWN:
         if ((cur.position[cur.lastmove.to + 0x01] == R_PAWN) || (cur.position[cur.lastmove.to - 0x01] == R_PAWN))
         {
             if (cur.lastmove.to & 0xf0 == 0x30) sum += SCORE_PAWN_BOTTOM_UNION; //兵沉底 
             else sum += SCORE_PAWN_UNION;
         }
         break;
    case B_PAWN:
         if ((cur.position[cur.lastmove.to + 0x01] == B_PAWN) || (cur.position[cur.lastmove.to - 0x01] == B_PAWN))
         {           
             if ((cur.lastmove.to & 0xf0) == 0xc0) sum += SCORE_PAWN_BOTTOM_UNION; //兵沉底 
             else sum += SCORE_PAWN_UNION;
         }   
         break;  
    //--------车的各种配合--------//
    case R_ROOK:
    //--------车沉底炮碾丹砂--------//
        flag = 1;
        if (((cur.lastmove.to & 0xf0) == 0x30) && (((cur.lastmove.to & 0x0f) >= 0x0a) || ((cur.lastmove.to & 0x0f) <= 0x04))) //车沉底 
        {
            if (((cur.kingpos[BLACK] & 0xff) == 0x37) && (cur.position[0x47] == B_ADVISOR)) //将在底线 
            {
                 if ((cur.lastmove.to == 0x3b) && (cur.position[0x3a] == R_CANNON) && (cur.position[0x39] == NONE) && (cur.position[0x38] == NONE)) 
                      sum += SCORE_ROOK_CANNON_BOTTOM;
                 else if ((cur.lastmove.to == 0x3b) && (cur.position[0x3a] == NONE) && (cur.position[0x39] == R_CANNON) && (cur.position[0x38] == NONE))
                      sum += SCORE_ROOK_CANNON_BOTTOM;
                 else if ((cur.lastmove.to == 0x3a) && (cur.position[0x39] == R_CANNON) && (cur.position[0x38] == NONE))
                      sum += SCORE_ROOK_CANNON_BOTTOM;
                 else if ((cur.lastmove.to == 0x33) && (cur.position[0x34] == R_CANNON) && (cur.position[0x35] == NONE) && (cur.position[0x36] == NONE))
                      sum += SCORE_ROOK_CANNON_BOTTOM;
                 else if ((cur.lastmove.to == 0x33) && (cur.position[0x34] == NONE) && (cur.position[0x35] == R_CANNON) && (cur.position[0x36] == NONE))
                      sum += SCORE_ROOK_CANNON_BOTTOM;
                 else if ((cur.lastmove.to == 0x34) && (cur.position[0x35] == R_CANNON) && (cur.position[0x36] == NONE))      
                      sum += SCORE_ROOK_CANNON_BOTTOM;
            }
        }
		else 
    //--------车炮将调车抽子--------//
        {
            if ((cur.lastmove.to & 0x0f) == (cur.kingpos[BLACK] & 0x0f)) //车和将在同一列
            {    
                for (tar = cur.lastmove.to - 0x10; tar > cur.kingpos[BLACK]; tar -= 0x10)
                {
                    if (cur.position[tar] != NONE) flag = 0;
					break;
                }  
                if (flag)
                {
                    for (tar = cur.lastmove.to + 0x10; (tar & 0xf0) <= 0xc0; tar += 0x10) 
                    {
                        if (cur.position[tar] == NONE) continue;
                        else 
                        {
                            if (cur.position[tar] == R_CANNON) sum += SCORE_ROOK_CANNON_KILL;
                            else break; 
                        }
                    }
                }
            }
        }
        break;
    case B_ROOK:
    //--------车沉底炮碾丹砂--------//
        flag = 1;
        if (((cur.lastmove.to & 0xf0) == 0xc0) && (((cur.lastmove.to & 0x0f) >= 0x0a) || ((cur.lastmove.to & 0x0f) <= 0x04))) //车沉底 
        {
            if (((cur.kingpos[RED] & 0xff) == 0xc7) && (cur.position[0xb7] == R_ADVISOR)) //帅在底线 
            {
                 if ((cur.lastmove.to == 0xcb) && (cur.position[0xca] == B_CANNON) && (cur.position[0xc9] == NONE) && (cur.position[0xc8] == NONE)) 
                      sum += SCORE_ROOK_CANNON_BOTTOM;
                 else if ((cur.lastmove.to == 0xcb) && (cur.position[0xca] == NONE) && (cur.position[0xc9] == B_CANNON) && (cur.position[0xc8] == NONE))
                      sum += SCORE_ROOK_CANNON_BOTTOM;
                 else if ((cur.lastmove.to == 0xca) && (cur.position[0xc9] == B_CANNON) && (cur.position[0xc8] == NONE))
                      sum += SCORE_ROOK_CANNON_BOTTOM;
                 else if ((cur.lastmove.to == 0xc3) && (cur.position[0xc4] == B_CANNON) && (cur.position[0xc5] == NONE) && (cur.position[0xc6] == NONE))
                      sum += SCORE_ROOK_CANNON_BOTTOM;   
                 else if ((cur.lastmove.to == 0xc3) && (cur.position[0xc4] == NONE) && (cur.position[0xc5] == B_CANNON) && (cur.position[0xc6] == NONE))
                      sum += SCORE_ROOK_CANNON_BOTTOM;
                 else if ((cur.lastmove.to == 0xc4) && (cur.position[0xc5] == B_CANNON) && (cur.position[0xc6] == NONE))      
                      sum += SCORE_ROOK_CANNON_BOTTOM;
            }
        }
		else 
    //--------车炮将调车抽子--------//
        {
            if ((cur.lastmove.to & 0x0f) == (cur.kingpos[RED] & 0x0f)) //车和帅在同一列
            {    
                for (tar = cur.lastmove.to + 0x10; tar < cur.kingpos[RED]; tar += 0x10)
                {
                    if (cur.position[tar] != NONE ) flag = 0;
					break;     
                }  
                if (flag)
                {
                    for (tar = cur.lastmove.to - 0x10; (tar & 0xf0) >= 0x30; tar -= 0x10) 
                    {
                        if (cur.position[tar] == NONE) continue;
                        else 
                        {
                            if (cur.position[tar] == B_CANNON) sum += SCORE_ROOK_CANNON_KILL;
                            else break;
                        }
                    }
                }
            }
        }
        break;
    }
    return sum;
}    

int Evaluation::GetAnPScore(BDPOINT point, CHESSMAN chess)
{
	int score;
	if (IsRed(chess))
    {
		score = posValue[chess][point];
	}
	else
	{
		score = posValue[(CHESSMAN)(NONE - chess)][(BDPOINT)(0xfe - point)];
	}
    //棋子威胁、保护评估值相对于子力位置评估值的大小
    score >>= 4;
	if (score < 5)
	{
		score = 5;
	}
    return score;   
}
