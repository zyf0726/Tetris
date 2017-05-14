#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>
#include <stdint.h>
#include "shared.h"
#include "tetromino.h"

#define LINE_MASK  (uint16_t) 0x7FE0 // 0b0111111111100000
#define EMPTY_LINE (uint16_t) 0x8010 // 0b1000000000010000
#define FULL_LINE  (uint16_t) 0xFFF0 // 0b1111111111110000

struct board {
    uint16_t lines[BOARD_HEIGHT];
    INLINE board();
};
extern uint16_t cell_masks[BOARD_WIDTH];

int remove_lines (board * board, t_last_placement * tlp);
void remove_line (board * board, int line);
int get_tile (int x, int y, board * board);
void set_tile (int x, int y, board * board, int value);
void print_board (FILE * stream, board * board);

void read_board (board * board);

#endif
