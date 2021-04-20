#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

typedef struct submatrix_t {
    unsigned int xFrom;
    unsigned int yFrom;
    unsigned int xTo;
    unsigned int yTo;
} submatrix;

typedef struct arg_struct_t {
    board_t **boardArray;
    unsigned int cycles;
    barrier_t *barrier;
    submatrix submatrix;
} arg_struct;

game_t *loadGame(const char *filename) {
    unsigned int cycles, col, row;
    int falla, i = 0;
    char *res;
    game_t *game;
    FILE *file = fopen(filename, 'r');

    if (!file)
        return NULL;

    if (fscanf(file, '%d %d %d', cycles, col, row) == 3) {
        game = malloc(sizeof(game_t));
        
        falla = game == NULL;

        if (!falla) {
            game->cycles = cycles;

            game->board = malloc(sizeof(board_t));
        
            falla = game->board == NULL;

            if (!falla) {
                falla = board_init(&(game->board), col, row);
                
                if (!falla) {
                    res = malloc(sizeof(char) * ((2 * col) + 2));
                    fgets(res, (2 * col) + 2, file);

                    for (; i < game->board->row; ++i) {
                        fgets(res, (2 * col) + 2, file);

                        falla = board_load_row(&(game->board), res, i);

                        if (falla)
                            break;
                    }
                    free(res);
                }
            }
        }

        fclose(file);
    } else
        return NULL;

    if (falla) {
        if (game) {
            if (game->board)
                free(game->board);
            free(game);
        }
        return NULL;
    }

    return game;
}

void writeBoard(board_t board, const char *filename) {
    char *res = malloc(sizeof(char) * (board.col + 1) * board.row);
    FILE *file = fopen(filename, 'w+');

    board_show(board, res);

    fprintf(file, "%s", res);

    fclose(file);

    free(res);
}

submatrix *distribute(unsigned int row, unsigned int col, const int nuproc) {
    submatrix *submatrixArray = malloc(sizeof(submatrix) * nuproc);
    int option = row >= col ? 1 : 0, i = 1;

    for (; i <= nuproc; ++i) {
        if (option) { // if row >= col
            (submatrixArray + i)->xFrom = 0;
            (submatrixArray + i)->xTo = col - 1;
            if (i == 1)
                (submatrixArray + i)->yFrom = 0;
            else
                (submatrixArray + i)->yFrom = (submatrixArray + i - 1)->yTo + 1;
            (submatrixArray + i)->yTo = (submatrixArray + i)->yFrom + (row / nuproc) - (i > (row % nuproc) ? 1 : 0);
        } else { // if col > row
            (submatrixArray + i)->yFrom = 0;
            (submatrixArray + i)->yTo = row - 1;
            if (i == 1)
                (submatrixArray + i)->xFrom = 0;
            else
                (submatrixArray + i)->xFrom = (submatrixArray + i - 1)->xTo + 1;
            (submatrixArray + i)->xTo = (submatrixArray + i)->xFrom + (col / nuproc) - (i > (col % nuproc) ? 1 : 0);
        }
    }
}

void thAux(void *args) {

}

board_t *congwayGoL(board_t *board, unsigned int cycles, const int nuproc) {
    unsigned int cantThread = (nuproc < board->row || nuproc < board->col) ? nuproc : board->row > board->col ? board->row : board->col;
    int i = 0;

    pthread_t threads[cantThread];

    barrier_t *barrier = malloc(sizeof(barrier_t));
    
    barrier_init(barrier, cantThread);

    submatrix *submatrixArray = distribute(board->row, board->col, cantThread);

    board_t *boardAux;
    
    if (board && cycles > 0) {
        board_t **boardArray = malloc(sizeof(board_t*));

        if (!boardArray) {
            free(submatrixArray);
            return NULL;
        }

        boardAux = malloc(sizeof(board_t));

        if (!boardAux || board_init(boardAux, board->col, board->row)) {
            free(submatrixArray);
            free(boardArray);
            return NULL;
        }

        arg_struct argStruct;
        argStruct.boardArray = boardArray;
        argStruct.cycles = cycles;
        argStruct.barrier = barrier;


        for (; i < cantThread; i++) {
            argStruct.submatrix = *(submatrixArray + i);
            if (pthread_create(&threads[i], NULL, thAux, (void *)&argStruct)) {
                free(submatrixArray);
                free(boardArray);
                free(boardAux);
                return NULL;
            }
        }

        for (i = 0; i < cantThread; i++)
            pthread_join(threads[i], NULL);
    }
}

// 3 NPROC

// 3 19
// (0, 0) - (2, 6), (0 - 7) - (2, 12), (0, 13) - (2, 18)
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

// (0, -2)   (0, -1)   (0, 0)   (0, 1)   (0, 2)

// 6 6
// XXXXXX
// 000000
// XXXXXX
// OOO000
// XXXXXX
// 000000

// 3 2
// XXX
// OOO

// X
// O
// X
