#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "board.h"

int board_init(board_t *board, size_t col, size_t row) {
    unsigned int i = 0;
    int falla = 0;

    board->col = col;
    board->row = row;

    board->cell = malloc(sizeof(char*) * row);

    if (!(board->cell)) {
        free(board);
        return 1;
    }
    
    for (; i < row; ++i) {
        board->cell[i] = malloc(sizeof(char) * col);
        if (!(board->cell)) {
            falla = 1;
            break;
        }
    }

    if (falla) {
        for (; i > 0; --i)
            free(board->cell[i]);
        free(board->cell);
        return 1;
    }

    return 0;
}

int board_init_def(board_t *board, size_t col, size_t row, char def) {
    unsigned int i = 0, j;
    int falla = 0;
    
    board = malloc(sizeof(board_t));

    board->col = col;
    board->row = row;

    board->cell = malloc(sizeof(char*) * row);

    if (!(board->cell)) {
        free(board);
        return 1;
    }
    
    for (; i < row; ++i) {
        board->cell[i] = malloc(sizeof(char) * col);
        if (!(board->cell)) {
            falla = 1;
            break;
        }
        for (j = 0; j < col; ++j)
            board->cell[i][j] = def;
    }

    if (falla) {
        for (; i > 0; --i)
            free(board->cell[i]);
        free(board->cell);
        free(board);
        return 1;
    }

    return 0;
}

char board_get(board_t board, unsigned int col, unsigned int row) {
    if (col >= board.col || row >= board.row)
        return ' ';

    return board.cell[row][col];
}

char board_get_round(board_t board, int col, int row) {
    row = row % board.row;

    col = col % board.col;

    return board.cell[(row >= 0) ? (unsigned int)row : (board.row + row)][(col >= 0) ? (unsigned int)col : (board.col + col)];
}

int board_set(board_t board, unsigned int col, unsigned int row, char val) {
    if (col >= board.col || row >= board.row)
        return 1;

    board.cell[row][col] = val;

    return 0;
}

int digits_of_int(unsigned int n) {
    int count = 0;
    while (n != 0) {
        n /= 10;     
        ++count;
    }
    return count;
}

int board_load(board_t *board, char *str) {
    unsigned int j, mult, pos = 0;
    int falla = 0;
    char val;
    while (sscanf(str, " %d%c", &mult, &val) == 2) {
        for (j = 0; j < mult; ++j) {
            falla = board_set(*board, pos % board->col, pos / board->col, val);
            if (falla)
                break;
            ++pos;
        }
        str += (*str == '\n' ? 1 : 0) + digits_of_int(mult) + 1;
        if (*str == '\n' || *str == '\0') {
            falla = (pos % board->col != 0);
            if (falla)
                break;
        }
    }
    return falla;
}

int board_load_row(board_t *board, char *str, unsigned int row) {
    unsigned int j, mult, pos = 0;
    int falla = 0;
    char val;
    while (sscanf(str, " %d%c", &mult, &val) == 2) {
        for (j = 0; j < mult; ++j) {
            falla = board_set(*board, pos, row, val);
            if (falla)
                break;
            ++pos;
        }
        str += digits_of_int(mult) + 1;
        if (*str == '\n' || *str == '\0') {
            falla = (pos != board->col);
            if (falla)
                break;
        }
    }
    return falla;
}

void board_show(board_t board, char *res) {
    unsigned int i = 0, j;
    for (; i < board.row; ++i) {
        for (j = 0; j < board.col; ++j)
            *(res + (i * (board.col + 1)) + j) = board.cell[i][j];
        *(res + (i * (board.col + 1)) + board.col) = '\n';
    }
    *(res + ((board.col + 1) * board.row) - 1) = '\0';
}

void board_destroy(board_t *board) {
    for (unsigned int i = 0; i < board->row; ++i)
        free(board->cell[i]);
    free(board->cell);
}
