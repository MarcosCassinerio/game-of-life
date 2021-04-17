#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "board.h"

int board_init(board_t *board, size_t col, size_t row) {
    if (col < 0 || row < 0)
        return 1;

    int i = 0, falla = 0;
    
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

int board_init_def(board_t *board, size_t col, size_t row, char def) {
    if (col < 0 || row < 0)
        return 1;

    int i = 0, h, falla = 0;
    
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
        for (h = 0; h < col; ++h)
            board->cell[i][h] = def;
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
        return NULL;

    return board.cell[row][col];
}

char board_get_round(board_t board, int col, int row) {
    row = row % board.row;

    col = col % board.col;

    return board.cell[row >= 0 ? row : (board.row + row)][col >= 0 ? col : (board.col + col)];
}

int board_set(board_t board, unsigned int col, unsigned int row, char val) {
    if (col >= board.col || row >= board.row)
        return 1;

    board.cell[row][col] = val;

    return 0;
}

int board_load(board_t *board, char *str) {
    int i = 0, j, mult, falla = 0, pos = 0;
    char val;
    while (sscanf(str, " %d%c", mult, val) == 2) {
        for (j = 0; j < mult; ++j) {
            falla = board_set(*board, pos % board->col, pos / board->col, val);
            ++pos;
            if (falla)
                break;
        }
    }
    falla = (pos != (board->row * board->col) - 1);
    return falla;
}

void board_show(board_t board, char *res) {
    int i = 0, j;
    res = malloc(sizeof(char) * board.row * board.col);
    for (; i < board.row; ++i) {
        for (j = 0; j < board.col; ++j)
            *(res + (i * board.row) + j) = board.cell[i][j];
        *(res + (i * board.row) + j) = '\n';
    }
    *(res + (i * board.row) + j) = '\0';
}

void board_destroy(board_t *board) {
    for (int i = 0; i < board->row; ++i)
        free(board->cell[i]);
    free(board->cell);
    free(board);
}
