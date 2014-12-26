/*! \file movemaker.cpp
@brief 着法生成类（单件模块）
*/

#include "movemaker.h"

int MoveMaker::Make(Board & cur, PLAYER who, Movement move[])
{
    int i;
    Bool next;
    BDPOINT index, tar, leg;
    int kind = 0;
    Bool chk;
    
    for (i = 0; i < 8; i++)
    {
        cur.numOfMove[i] = 0;
    }
    
    //遍历棋盘 
    for (index = 0x33; index <= 0xcb; index++)
    {
        if (cur.position[index] == NONE || cur.position[index] == OUT)
        {
            continue;    
        }
        //不同的子，用不同的方法尽可能快速地找到可行的着法 
        switch (cur.position[index])
        {
            //马（红黑方算法一致） 
            //相对位置模板法 
            case R_KNIGHT:
            case B_KNIGHT:
                if (who == GetColor(cur.position[index]))
                {
					for(i = 0; i < 8; i++)
					{
						tar = index + cnKnightMoveTab[i];
						leg = index + cnHorseLegTab[i];
						if (((who == BLACK) ? IsRedOrNone(cur.position[tar]) : IsBlackOrNone(cur.position[tar])) && cur.position[leg] == NONE)
						{
							if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[4])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
                        }
					}
                }
                break; 
            //车（红黑方算法一致） 
            //四个方向分别循环找可行着法 
            case R_ROOK:
            case B_ROOK:
                if (who == GetColor(cur.position[index]))
                {
                    for (tar = index - 0x10; tar > 0x30; tar -= 0x10)
                    {
						if(cur.position[tar] != NONE)
						{
                            if (GetColor(cur.position[tar]) != who)
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[5])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
							break;							
						} 
						else
						{
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[5])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
						}
                    }
                    for (tar = index + 0x10; tar < 0xd0; tar += 0x10)
                    {
						if(cur.position[tar] != NONE)
						{
                            if (GetColor(cur.position[tar]) != who)
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[5])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
							break;							
						} 
						else
						{
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[5])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
						}
                    }
                    for (tar = index - 0x01; (tar & 0x0f) > 0x02; tar -= 0x01)
                    {
						if(cur.position[tar] != NONE)
						{
                            if (GetColor(cur.position[tar]) != who)
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[5])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
							break;							
						} 
						else
						{
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[5])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
						}
                    }
                    for (tar = index + 0x01; (tar & 0x0f) < 0x0c; tar += 0x01)
                    {
						if(cur.position[tar] != NONE)
						{
                            if (GetColor(cur.position[tar]) != who)
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[5])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
							break;							
						} 
						else
						{
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[5])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
						}
                    }
                }
                break; 
            //炮（红黑方算法一致） 
            //四个方向分别循环找可行着法，对吃子和非吃子着法分别判断 
            case R_CANNON:
            case B_CANNON:
                if (who == GetColor(cur.position[index]))
                {
                    for (next = FALSE, tar = index - 0x10; tar > 0x30; tar -= 0x10)
                    {
                        if ((next == FALSE) && (cur.position[tar] == NONE))
                        {
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[6])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
                            continue;
                        }
                        if ((next == FALSE) && (cur.position[tar] != NONE))
                        {
                            next = TRUE;
                            continue;
                        }
                        if ((next == TRUE) && ((who == BLACK) ? IsRed(cur.position[tar]) : IsBlack(cur.position[tar])))
                        {
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[6])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
                            break;
                        }
                        if ((next == TRUE) && ((who == BLACK) ? IsBlack(cur.position[tar]) : IsRed(cur.position[tar])))
                        {
                            break;
                        }
                    }
                    for (next = FALSE, tar = index + 0x10; tar < 0xd0; tar += 0x10)
                    {
                        if ((next == FALSE) && (cur.position[tar] == NONE))
                        {
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[6])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
                            continue;
                        }
                        if ((next == FALSE) && (cur.position[tar] != NONE))
                        {
                            next = TRUE;
                            continue;
                        }
                        if ((next == TRUE) && ((who == BLACK) ? IsRed(cur.position[tar]) : IsBlack(cur.position[tar])))
                        {
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[6])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
                            break;
                        }
                        if ((next == TRUE) && ((who == BLACK) ? IsBlack(cur.position[tar]) : IsRed(cur.position[tar])))
                        {
                            break;
                        }
                    }
                    for (next = FALSE, tar = index - 0x01; (tar & 0x0f) > 0x02; tar -= 0x01)
                    {
                        if ((next == FALSE) && (cur.position[tar] == NONE))
                        {
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[6])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
                            continue;
                        }
                        if ((next == FALSE) && (cur.position[tar] != NONE))
                        {
                            next = TRUE;
                            continue;
                        }
                        if ((next == TRUE) && ((who == BLACK) ? IsRed(cur.position[tar]) : IsBlack(cur.position[tar])))
                        {
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[6])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
                            break;
                        }
                        if ((next == TRUE) && ((who == BLACK) ? IsBlack(cur.position[tar]) : IsRed(cur.position[tar])))
                        {
                            break;
                        }
                    }
                    for (next = FALSE, tar = index + 0x01; (tar & 0x0f) < 0x0c; tar += 0x01)
                    {
                        if ((next == FALSE) && (cur.position[tar] == NONE))
                        {
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[6])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
                            continue;
                        }
                        if ((next == FALSE) && (cur.position[tar] != NONE))
                        {
                            next = TRUE;
                            continue;
                        }
                        if ((next == TRUE) && ((who == BLACK) ? IsRed(cur.position[tar]) : IsBlack(cur.position[tar])))
                        {
                            if (PreAdd(cur, who, index, tar, chk))
                            {
                                Movement mv(index, tar, cur.position[tar], chk);
                                move[kind++] = mv;
								(cur.numOfMove[6])++;
                                if (kind >= SEARCH_WIDTH)
                                {
                                    return kind;
                                }
                            }
                            break;
                        }
                        if ((next == TRUE) && ((who == BLACK) ? IsBlack(cur.position[tar]) : IsRed(cur.position[tar])))
                        {
                            break;
                        }
                    }
                }
                break;
            default:
                //其它棋子红黑方算法不同（对称），分别处理 
                //红方 
                if (who == RED)
                {
                    switch (cur.position[index])
                    {
                        //帅
                        //相对位置模板法 
                        case R_KING:
                            for(i = 0; i < 4; i++)
							{
								tar = index + cnKINGMOVETAB[i];
								if (IsBlackOrNone(cur.position[tar]))
								{
									//判断帅落点是否出九宫
									if (!((tar & 0x0f) < 0x06 || (tar & 0x0f) > 0x08 || ((tar & 0xf0) >> 4) < 0x0a || ((tar & 0xf0) >> 4) > 0x0c))
									if(PreAdd(cur, who, index, tar, chk))
									{
										Movement mv(index, tar, cur.position[tar], chk);
                                        move[kind++] = mv;
										(cur.numOfMove[1])++;
                                        if (kind >= SEARCH_WIDTH)
                                        {
                                            return kind;
                                        }
                                    }
                                }
                            }
                            break;  
                        //仕
                        //可行位置有限，直接枚举 
                        //通过((pnt - index) & 0x01)奇偶校验，判断目标位置可行性 
                        case R_ADVISOR:
                            for (i = 0; i < 5; i++)
                            {
                                tar = R_ADVISOR_POSITION[i];
                                if (IsBlackOrNone(cur.position[tar]) && ((tar - index) & 0x01))
                                {
                                    if (PreAdd(cur, who, index, tar, chk))
                                    {
                                        Movement mv(index, tar, cur.position[tar], chk);
                                        move[kind++] = mv;
										(cur.numOfMove[2])++;
                                        if (kind >= SEARCH_WIDTH)
                                        {
                                            return kind;
                                        }
                                    }
                                }
                            }
                            break;
                        //相
                        //可行位置有限，直接枚举 
                        case R_BISHOP:
                            for (i = 0; i < 7; i++)
							{
								tar = R_BISHOP_POSITION[i];
                                if (IsBlackOrNone(cur.position[tar]))
                                {
									//判断目标位置的可行性
									if ((((tar & 0x0f) - (index & 0x0f)) == 0x02) || (((index & 0x0f) - (tar & 0x0f)) == 0x02))
                                    if (cur.position[(tar + index) >> 1] == NONE) //判断相眼是否被蹩
									if (PreAdd(cur, who, index, tar, chk))
                                    {
                                        Movement mv(index, tar, cur.position[tar], chk);
                                        move[kind++] = mv;
										(cur.numOfMove[3])++;
                                        if (kind >= SEARCH_WIDTH)
                                        {
                                            return kind;
                                        }
                                    }
                                }
                            }
                            break; 
                        //兵
                        //相对位置模板法 
                        case R_PAWN:
							if (index & 0x80)
							{
								tar = index - 0x10;
								if (IsBlackOrNone(cur.position[tar]))
								{
									if (PreAdd(cur, who, index, tar, chk))
									{
										Movement mv(index, tar, cur.position[tar], chk);
										move[kind++] = mv;
										(cur.numOfMove[7])++;
                                        if (kind >= SEARCH_WIDTH)
                                        {
                                            return kind;
                                        }
									}
								}
							}
							else
							{
								for(i = 0; i < 3; i++)
								{
									tar = index + R_PawnMoveTab[i];
									if(IsBlackOrNone(cur.position[tar]))
									{
										if (PreAdd(cur, who, index, tar, chk))
										{
											Movement mv(index, tar, cur.position[tar], chk);
											move[kind++] = mv;
										    (cur.numOfMove[7])++;
                                            if (kind >= SEARCH_WIDTH)
                                            {
                                                return kind;
                                            }
										}
									}
								}
							}
                            break; 
                    }
                }
                //黑方 
                else
                {
                    switch (cur.position[index])
                    {
                        //将
                        //相对位置模板法 
                        case B_KING:
                            for(i = 0; i < 4; i++)
							{
								tar = index + cnKINGMOVETAB[i];
								if (IsRedOrNone(cur.position[tar]))
								{
									//判断将落点是否出九宫
									if (!((tar & 0x0f) < 0x06 || (tar & 0x0f) > 0x08 || ((tar & 0xf0) >> 4) < 0x03 || ((tar & 0xf0) >> 4) > 0x05))
									if (PreAdd(cur, who, index, tar, chk))
									{
										Movement mv(index, tar, cur.position[tar], chk);
                                        move[kind++] = mv;
										(cur.numOfMove[1])++;
                                        if (kind >= SEARCH_WIDTH)
                                        {
                                            return kind;
                                        }
                                    }
                                }
                            }
                            break;  
                        //士
                        //可行位置有限，直接枚举 
                        //通过((pnt - index) & 0x01)奇偶校验，判断目标位置可行性 
                        case B_ADVISOR:
                            for (i = 0; i < 5; i++)
                            {
                                tar = B_ADVISOR_POSITION[i];
                                if (IsRedOrNone(cur.position[tar]) && ((tar - index) & 0x01))
                                {
                                    if (PreAdd(cur, who, index, tar, chk))
                                    {
                                        Movement mv(index, tar, cur.position[tar], chk);
                                        move[kind++] = mv;
										(cur.numOfMove[2])++;
                                        if (kind >= SEARCH_WIDTH)
                                        {
                                            return kind;
                                        }
                                    }
                                }
                            }
                            break;
                        //象
                        //可行位置有限，直接枚举 
                        case B_BISHOP:
                            for (i = 0; i < 7; i++)
							{
								tar = B_BISHOP_POSITION[i];
                                if (IsRedOrNone(cur.position[tar]))
                                {
									//判断目标位置的可行性
									if ((((tar & 0x0f) - (index & 0x0f)) == 0x02) || (((index & 0x0f) - (tar & 0x0f)) == 0x02))
                                    if (cur.position[(tar + index) >> 1] == NONE) //判断象眼是否被蹩
									if (PreAdd(cur, who, index, tar, chk))
                                    {
                                        Movement mv(index, tar, cur.position[tar], chk);
                                        move[kind++] = mv;
										(cur.numOfMove[3])++;
                                        if (kind >= SEARCH_WIDTH)
                                        {
                                            return kind;
                                        }
                                    }
                                }
                            }
                            break; 
                        //卒
                        //相对位置模板法 
                        case B_PAWN:
							if (!(index & 0x80))
							{
								tar = index + 0x10;
								if (IsRedOrNone(cur.position[tar]))
								{
									if (PreAdd(cur, who, index, tar, chk))
									{
										Movement mv(index, tar, cur.position[tar], chk);
										move[kind++] = mv;
										(cur.numOfMove[7])++;
                                        if (kind >= SEARCH_WIDTH)
                                        {
                                            return kind;
                                        }
									}
								}
							}
							else
							{
								for(i = 0; i < 3; i++)
								{
									tar = index + B_PawnMoveTab[i];
									if(IsRedOrNone(cur.position[tar]))
									{
										if (PreAdd(cur, who, index, tar, chk))
										{
											Movement mv(index, tar, cur.position[tar], chk);
											move[kind++] = mv;
											(cur.numOfMove[7])++;
                                            if (kind >= SEARCH_WIDTH)
                                            {
                                                return kind;
                                            }
										}
									}
								}
							}
                            break; 
                    }
                }
                break;
        }
    }
    return kind;
}

Bool MoveMaker::PreAdd(Board & cur, PLAYER who, BDPOINT from, BDPOINT to, Bool & check)
{
    check = FALSE;
    
    int i, flag;
    Bool face = TRUE, suicide = FALSE;
    BDPOINT tar, leg;
    Movement mv(from, to, cur.position[to], check);
    
    cur.AddMovement(mv);
    //看有没有将帅直面现象，有则返回 FALSE。
    if ((cur.kingpos[BLACK] & 0x0f) == (cur.kingpos[RED] & 0x0f))
    {
        for (i = cur.kingpos[BLACK] + 0x10; i < cur.kingpos[RED]; i += 0x10)
        {
            if (cur.position[i] != NONE)
            {
                face = FALSE;   
                break;
            }
        }
    }
    else
    {
        face = FALSE; 
    }
    
    //有没有主动送将，有则返回 FALSE。
    //车、炮、兵（卒）的将军方式 
    for (flag = 0, tar = cur.kingpos[who] - 0x10; tar > 0x20; tar -= 0x10) //帅前方，将后方 
    {
        if (cur.position[tar] == NONE) continue;
        else {
            if ((tar == cur.kingpos[who] - 0x10) && (who == RED) && (cur.position[tar] == B_PAWN))
            {
			    suicide = TRUE;
			    goto finish;
            }  //判断帅前方第一格是否受到卒威胁 
            if ((flag == 0) && ((who == BLACK) ? (cur.position[tar] == R_ROOK) : (cur.position[tar] == B_ROOK)))
            {
			    suicide = TRUE;
			    goto finish;
            } //判断帅（将）是否受到对方车威胁 
            else if ((flag == 1) && ((who == BLACK) ? (cur.position[tar] == R_CANNON) : (cur.position[tar] == B_CANNON)))
            {
			    suicide = TRUE;
			    goto finish;
            } //判断帅（将）是否受到对方炮威胁 
            flag++;
        }
        if (flag == 2) break;  //循环遍历到第二颗棋子后直接跳出 
    }
    for (flag = 0, tar = cur.kingpos[who] + 0x10; tar < 0xd0; tar += 0x10) //帅后方，将前方 
    {
        if (cur.position[tar] == NONE) continue;
        else {
            if ((tar == cur.kingpos[who] + 0x10) && (who == BLACK) && (cur.position[tar] == R_PAWN))
            {
			    suicide = TRUE;
			    goto finish;
            } //判断将前方第一格是否受到兵威胁 
            if ((flag == 0) && ((who == BLACK) ? (cur.position[tar] == R_ROOK) : (cur.position[tar] == B_ROOK)))
            {
			    suicide = TRUE;
			    goto finish;
            } //判断帅（将）是否受到对方车威胁 
            else if ((flag == 1) && ((who == BLACK) ? (cur.position[tar] == R_CANNON) : (cur.position[tar] == B_CANNON)))
            {
			    suicide = TRUE;
			    goto finish;
            } //判断帅（将）是否受到对方炮威胁 
            flag++;
        }
        if (flag == 2) break;  //循环遍历到第二颗棋子后直接跳出 
    }
    for (flag = 0, tar = cur.kingpos[who] - 0x01; (tar & 0x0f) > 0x02; tar -= 0x01) //帅（将）左方 
    {
        if (cur.position[tar] == NONE) continue;
        else {
            if ((tar == cur.kingpos[who] - 0x01) && ((who == BLACK) ? (cur.position[tar] == R_PAWN) : (cur.position[tar] == B_PAWN)))
            {
			    suicide = TRUE;
			    goto finish;
            }  //判断帅（将）是否受到卒(兵)的威胁 
            if ((flag == 0) && ((who == BLACK) ? (cur.position[tar] == R_ROOK) : (cur.position[tar] == B_ROOK)))
            {
			    suicide = TRUE;
			    goto finish;
            } //判断帅（将）是否受到对方车威胁 
            else if ((flag == 1) && ((who == BLACK) ? (cur.position[tar] == R_CANNON) : (cur.position[tar] == B_CANNON)))
            {
			    suicide = TRUE;
			    goto finish;
            } //判断帅（将）是否受到对方炮威胁 
            flag++;
        }
        if (flag == 2) break;  //循环遍历到第二颗棋子后直接跳出 
    }
    for (flag = 0, tar = cur.kingpos[who] + 0x01; (tar & 0x0f) < 0x0c; tar += 0x01) //帅（将）右方 
    {
        if (cur.position[tar] == NONE) continue;
        else {
            if ((tar == cur.kingpos[who] + 0x01) && ((who == BLACK) ? (cur.position[tar] == R_PAWN) : (cur.position[tar] == B_PAWN)))
            {
			    suicide = TRUE;
			    goto finish;
            }  //判断帅（将）是否受到卒(兵)的威胁  
            if ((flag == 0) && ((who == BLACK) ? (cur.position[tar] == R_ROOK) : (cur.position[tar] == B_ROOK))) 
            {
			    suicide = TRUE;
			    goto finish;
            } //判断帅（将）是否受到对方车威胁 
            else if ((flag == 1) && ((who == BLACK) ? (cur.position[tar] == R_CANNON) : (cur.position[tar] == B_CANNON)))
            {
			    suicide = TRUE;
			    goto finish;
            } //判断帅（将）是否受到对方炮威胁 
            flag++;
        }
        if (flag == 2) break;  //循环遍历到第二颗棋子后直接跳出 
    }
    //马的将军方式 
	for(i = 0; i < 8; i++)
	{
		tar = cur.kingpos[who] + cnKnightMoveTab[i];
		leg = cur.kingpos[who] + cnHorseLegBackTab[i];
		if (((who == BLACK) ? (cur.position[tar] == R_KNIGHT) : (cur.position[tar] == B_KNIGHT)) && cur.position[leg] == NONE)
		{
			suicide = TRUE;
			goto finish;
        }
	}

finish:    
    cur.DelMovement(mv);
    return (!face && !suicide);
}
