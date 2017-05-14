#ifndef SINGLEFILE

#include "game_board.h"
#include "board.h"

void game_board::put_eliminate(const tetromino *tr, int x, int y)
{
    board new_board(*this);

    t_last_placement tlp{tr, x, y, 0, NULL};
    new_board.place(tr, x, y);
    new_board.remove_lines(&tlp);
    transCount = tlp.n_lines_removed;
    for (int i = 0; i < tlp.n_lines_removed; i++)
        trans[i] = lines[tlp.lines_removed[i]];

    free(tlp.lines_removed);
    for (maxHeight = 1; maxHeight <= MAPHEIGHT; maxHeight++)
        if (lines[20 - maxHeight] == EMPTY_LINE)
            break;
    maxHeight--;
    (board &) *this = new_board;

}


#endif

