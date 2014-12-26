/*! \file openbook.cpp
@brief 开局库操作类（单件模块）
*/

#include "openbook.h"

void OpenBook::InitBook()
{
    if (loadedopen)
    {
        return;    
    }
    
    HASHNUM zobrist;
    PLAYER player;
    long trick;
    vector<BookItem>::iterator ptr;
    
    books = new vector<BookItem>();
    
    bookfile = fopen(bookpath, "rb");
    while (!feof(bookfile))
    {
        fread(&zobrist, sizeof(HASHNUM), 1, bookfile);
        fread(&player, sizeof(PLAYER), 1, bookfile);
        fread(&trick, sizeof(long), 1, bookfile);
        
        for (ptr = books->begin(); ptr != books->end(); ptr++)
        {
            if (((BookItem)*ptr).zobrist == zobrist && ((BookItem)*ptr).player == player && ((BookItem)*ptr).tricks->size() < 8)
            {
                ((BookItem)*ptr).tricks->push_back(trick);
                break;
            }
        }
        if (ptr == books->end())
        {
            BookItem newbi;
            newbi.zobrist = zobrist;
            newbi.player = player;
            newbi.tricks = new vector<long>();
            newbi.tricks->push_back(trick);
            books->push_back(newbi);
        }
    }
    
    fclose(bookfile); 
    loadedopen = TRUE;
}

void OpenBook::ClearBook()
{
    vector<BookItem>::iterator ptr;
    for (ptr = books->begin(); ptr != books->end(); ptr++)
    {
        delete ((BookItem)*ptr).tricks;
    }
    delete books;
}

int OpenBook::Search(const Board & cur, char * str)
{
    int kind = 0, a;
    long t;
    vector<BookItem>::iterator ptr;
    a = books->size();
    for (ptr = books->begin(); ptr != books->end(); ptr++)
    {
        if (((BookItem)*ptr).zobrist == cur.zobrist && ((BookItem)*ptr).player == cur.player)
        {
            kind = ((BookItem)*ptr).tricks->size();
            srand((unsigned)time(NULL)); 
            t = ((BookItem)*ptr).tricks->at((rand() % kind));
            str[0] = ((char *)(&t))[0];
            str[1] = ((char *)(&t))[1];
            str[2] = ((char *)(&t))[2];
            str[3] = ((char *)(&t))[3];
            str[4] = '\0'; 
            break;
        }
    }
    return kind;
}
