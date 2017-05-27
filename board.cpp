#ifndef SINGLEFILE
#include "shared.h"
#include "board.h"
#endif
#define CELL_MASKS(x) uint16_t(1u << (14 - (x)))

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

int board::get_tile(int x, int y) const
{
    return !!(lines[y] & CELL_MASKS(x));
}

void set_tile(int x, int y, struct board *board, int value)
{
    if (value)
    {
        board->lines[y] |= CELL_MASKS(x);
    } else
    {
        board->lines[y] &= ~CELL_MASKS(x);
    }
}

void print_board(FILE *stream, struct board *board)
{
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        fprintf(stream, "|");

        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (board->lines[y] & CELL_MASKS(x))
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

inline void shift_lines(uint16_t lines[], int position, ctet tr)
{
    if (position == 0)
        for (int i = 0; i < 4; ++i) lines[i] = tr.lines[i] | EMPTY_LINE;
    else if (position > 0)
        for (int i = 0; i < 4; ++i) lines[i] = tr.lines[i] >> position | EMPTY_LINE;
    else
        for (int i = 0; i < 4; ++i) lines[i] = tr.lines[i] << -position | EMPTY_LINE;

}

bool board::valid_pos_except_top(ctet tr, int x, int y) const
{
    if (y > BOARD_HEIGHT - 4 + tr.p_bottom) return false;
    uint16_t ls[4];
    shift_lines(ls, x, tr);
    for (int dy = max(tr.p_top, -y); dy < 4 - tr.p_bottom; ++dy)
        if ((lines[y + dy] & ls[dy]) != EMPTY_LINE) return false;
    return true;
}

int board::directly_drop(const tetromino *tr, int position, int *placement) {
    uint16_t ls[4];
    shift_lines(ls, position, *tr);
    int i;
    for (i = -tr->p_top; i < BOARD_HEIGHT + tr->p_bottom - 4 + 1; i++)
        for (int y = tr->p_top; y < 4 - tr->p_bottom; y++)
            if ((lines[i + y] & ls[y]) != EMPTY_LINE)
            {
                i--;
                goto found_i;
            }
    i--;

    found_i:
    if (i < -tr->p_top) return 1;

    if (placement != NULL)
        *placement = i;

    for (int y = tr->p_top; y < 4 - tr->p_bottom; y++) ls[i + y] |= ls[y];
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

void board::print() const {
#ifndef _BOTZONE_ONLINE
    static const char *i2s[] = {
            "~~",
            "~~",
            "  ",
            "[]",
            "##"
    };

    cerr << "cur:                       enemy:\n";
    for (int y = 0; y < MAPHEIGHT; y++) {
        cerr << "~~";
        for (int x = 0; x < MAPWIDTH; x++)
            cerr << i2s[get_tile(x, y) + 2];

        cerr << "~~";
        cerr << endl;
    }
    for (int i = 0; i < 43; i++)
        cerr << "~~";
    cerr << endl;
#endif
}
