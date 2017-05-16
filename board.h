#pragma once
#ifndef SINGLEFILE

#include <stdio.h>
#include <stdint.h>
#include "shared.h"
#include "tetromino.h"
#endif
#define LINE_MASK  (uint16_t) 0x7FE0 // 0b0111111111100000
#define EMPTY_LINE (uint16_t) 0x8010 // 0b1000000000010000
#define FULL_LINE  (uint16_t) 0xFFF0 // 0b1111111111110000

struct board {
    uint16_t lines[BOARD_HEIGHT];
    INLINE board();
    int remove_lines (t_last_placement * tlp);

    int get_tile (int x, int y);
    INLINE void place(ctet tr, int x, int y)
    {
        uint16_t tl[4]; shift_lines(tl, x, tr);
        for (int i = tr.p_top; i < 4 - tr.p_bottom; ++i)
        {
#ifdef debug
            assert(0 == (lines[i + y] & tl[i]));
#endif
            lines[i + y] |= tl[i];
        }

    }

    int directly_drop(const tetromino *tr, int position, int *placement);
    INLINE bool valid_pos(ctet tr, int x, int y) const ;

private:
    void remove_line(int line);
};
extern uint16_t cell_masks[BOARD_WIDTH];


void print_board (FILE * stream, board * board);

void read_board (board * board);

