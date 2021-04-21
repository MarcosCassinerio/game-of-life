#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

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
    FILE *file = fopen(filename, "r");

    if (!file)
        return NULL;

    if (fscanf(file, "%d %d %d", &cycles, &row, &col) == 3) {
        game = malloc(sizeof(game_t));
        
        falla = game == NULL;

        if (!falla) {
            game->cycles = cycles;

            game->board = malloc(sizeof(board_t));
        
            falla = game->board == NULL;

            if (!falla) {
                falla = board_init(game->board, col, row);
                
                if (!falla) {
                    res = malloc(sizeof(char) * ((2 * col) + 2));
                    fgets(res, (2 * col) + 2, file);

                    for (; i < (int) game->board->row; ++i) {
                        fgets(res, (2 * col) + 2, file);

                        falla = board_load_row(game->board, res, i);

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
    FILE *file = fopen(filename, "w+");

    board_show(board, res);

    fprintf(file, "%s", res);

    fclose(file);

    free(res);
}

submatrix *distribute(unsigned int row, unsigned int col, const int nuproc) {
    submatrix *submatrixArray = malloc(sizeof(submatrix) * nuproc), *currentSmx;
    int option = row >= col ? 1 : 0, i = 1;

    for (; i <= nuproc; ++i) {
        currentSmx = (submatrixArray + i - 1);
        if (option) { // if row >= col
            currentSmx->xFrom = 0;
            currentSmx->xTo = col - 1;
            if (i == 1)
                currentSmx->yFrom = 0;
            else
                currentSmx->yFrom = (currentSmx - 1)->yTo + 1;
            currentSmx->yTo = currentSmx->yFrom + (row / nuproc) - (i > (int) (row % nuproc) ? 1 : 0);
        } else { // if col > row
            currentSmx->yFrom = 0;
            currentSmx->yTo = row - 1;
            if (i == 1)
                currentSmx->xFrom = 0;
            else
                currentSmx->xFrom = (currentSmx - 1)->xTo + 1;
            currentSmx->xTo = currentSmx->xFrom + (col / nuproc) - (i > (int) (col % nuproc) ? 1 : 0);
        }
    }

    return submatrixArray;
}

State getNextState(board_t board, int row, int col) {
    int neigAlive = 0, i, j;
    State state = board.cell[row][col];

    for (i = (row - 1); i <= (row + 1); ++i) {
        for (j = (col - 1); j <= (col + 1); ++j) {
            if (i != row || j != col) {
                neigAlive += (board_get_round(board, j, i) == DEAD) ? 0 : 1;
                if (neigAlive > 3)
                    break;
            }
        }
        if (neigAlive > 3)
            break;
    }

    if (state == DEAD && neigAlive == 3)
        state = ALIVE;
    
    if (state == ALIVE && (neigAlive < 2 || neigAlive > 3))
        state = DEAD;

    return state;
}

void *threadFoo(void *args) {
    int i, j;
    unsigned int c = 0;
    board_t **boardArray = ((arg_struct*)args)->boardArray, *board, *boardAux;
    unsigned int cycles = ((arg_struct*)args)->cycles;
    barrier_t *barrier = ((arg_struct*)args)->barrier;
    submatrix submatrix = ((arg_struct*)args)->submatrix;

    for (; c < cycles; ++c) {
        board = *(boardArray + (c % 2)), boardAux = *(boardArray + ((c + 1) % 2));
        for (i = submatrix.yFrom; i <= (int) submatrix.yTo; ++i) {
            for (j = submatrix.xFrom; j <= (int) submatrix.xTo; ++j) {
                board_set(*boardAux, j, i, getNextState(*board, i, j));
            }
        }
        
        assert(!barrier_wait(barrier));
    }

    return NULL;
}

board_t *conwayGoL(board_t *board, unsigned int cycles, const int nuproc) {
    unsigned int nuprocAux = (unsigned int) nuproc, cantThread = (nuprocAux < board->row || nuprocAux < board->col) ? nuprocAux : board->row > board->col ? board->row : board->col, i = 0;

    arg_struct arguments[cantThread];
    pthread_t threads[cantThread];

    if (cycles <= 0)
        return board;

    barrier_t *barrier = malloc(sizeof(barrier_t));

    if (!barrier)
        return NULL;
    
    barrier_init(barrier, cantThread);

    submatrix *submatrixArray = distribute(board->row, board->col, cantThread);

    board_t *boardAux, **boardArray;
    
    if (board) {
        boardArray = malloc(sizeof(board_t*) * 2);

        if (!boardArray) {
            barrier_destroy(barrier);
            free(submatrixArray);
            return NULL;
        }

        boardAux = malloc(sizeof(board_t));

        if (!boardAux || board_init(boardAux, board->col, board->row)) {
            barrier_destroy(barrier);
            free(submatrixArray);
            free(boardArray);
            return NULL;
        }

        *boardArray = board;
        *(boardArray + 1) = boardAux;

        for (; i < cantThread; ++i) {
            arguments[i].boardArray = boardArray;
            arguments[i].cycles = cycles;
            arguments[i].barrier = barrier;
            arguments[i].submatrix = *(submatrixArray + i);
            assert(!pthread_create(&threads[i], NULL, threadFoo, (void *)&arguments[i]));
        }

        for (i = 0; i < cantThread; ++i)
            assert(!pthread_join(threads[i], NULL));

        board_destroy(*(boardArray + ((cycles + 1) % 2)));

        board = *(boardArray + (cycles % 2));

        free(boardArray);

        return board;
    }

    return NULL;
}

void destroyGame(game_t *game) {
    if (game) {
        board_destroy(game->board);
        free(game);
    }
}