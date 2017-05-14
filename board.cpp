#include "shared.h"
#include "board.h"
#include "feature_functions.h"
#include "tetromino.h"

uint16_t cell_masks[BOARD_WIDTH] = {
        0x4000, // 0b0100000000000000
        0x2000, // 0b0010000000000000
        0x1000, // 0b0001000000000000
        0x0800, // 0b0000100000000000
        0x0400, // 0b0000010000000000
        0x0200, // 0b0000001000000000
        0x0100, // 0b0000000100000000
        0x0080, // 0b0000000010000000
        0x0040, // 0b0000000001000000
        0x0020, // 0b0000000000100000
};

board::board()
{
    for (int i = 0; i < BOARD_HEIGHT; i++)
        lines[i] = EMPTY_LINE;
}


int board::remove_lines(t_last_placement *tlp)
{
    int n_lines_removed = 0;

    if (tlp != NULL)
    {
        tlp->n_lines_removed = 0;
        tlp->lines_removed = (int *) malloc(sizeof(int) * BOARD_HEIGHT);
    }

    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        if (lines[y] == FULL_LINE)
        {
            if (tlp != NULL)
            {
                tlp->lines_removed[tlp->n_lines_removed++] = y;
            }

            n_lines_removed++;
            remove_line(y);
        }
    }

    return n_lines_removed;
}

void board::remove_line(int line)
{
    for (int y = line; y > 0; y--)
    {
        lines[y] = lines[y - 1];
    }

    lines[0] = EMPTY_LINE;
}

int board::get_tile(int x, int y)
{
    return lines[y] & cell_masks[x];
}

void set_tile(int x, int y, struct board *board, int value)
{
    if (value)
    {
        board->lines[y] |= cell_masks[x];
    } else
    {
        board->lines[y] &= ~cell_masks[x];
    }
}

void print_board(FILE *stream, struct board *board)
{
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        fprintf(stream, "|");

        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (board->lines[y] & cell_masks[x])
            {
                fprintf(stream, "#");
            } else
            {
                fprintf(stream, " ");
            }
        }

        fprintf(stream, "|\n");
    }

    for (int i = 0; i < BOARD_WIDTH + 2; i++)
    {
        fprintf(stream, "-");
    }
    fprintf(stream, "\n");
}

int board::directly_drop(const tetromino *tr, int position, int *placement)
{
    uint16_t lines[4];
    shift_lines(lines, position, tr);
    int i;
    for (i = -tr->p_top; i < BOARD_HEIGHT + tr->p_bottom - 4 + 1; i++)
        for (int y = tr->p_top; y < 4 - tr->p_bottom; y++)
            if ((lines[i + y] & lines[y]) != EMPTY_LINE)
            {
                i--;
                goto found_i;
            }
    i--;

found_i:
    if (i < -tr->p_top) return 1;

    if (placement != NULL)
        *placement = i;

    for (int y = tr->p_top; y < 4 - tr->p_bottom; y++) lines[i + y] |= lines[y];
    return 0;
}
/*
void read_board (struct board * board) {
    size_t len;
    char * line = NULL;

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        getline(&line, &len, stdin);

        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (line[x + 1] == '#') {
                set_tile(x, y, board, 1);
            } else {
                set_tile(x, y, board, 0);
            }
        }
    }
}*/