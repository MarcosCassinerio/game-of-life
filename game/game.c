#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

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

            falla = board_init(&(game->board), col, row);
            
            if (!falla) {
                res = malloc(sizeof(char) * ((2 * col) + 2));
                fgets(res, (2 * col) + 2, file);

                for (; i < game->board.row; ++i) {
                    fgets(res, (2 * col) + 2, file);

                    falla = board_load_row(&(game->board), res, i);

                    if (falla) {
                        free(game);
                        break;
                    }
                }

                free(res);
            }
        }

        fclose(file);
    } else
        return NULL;

    return falla ? NULL : game;
}

void writeBoard(board_t board, const char *filename) {
    char *res = malloc(sizeof(char) * (board.col + 1) * board.row);
    FILE *file = fopen(filename, 'w+');

    board_show(board, res);

    fprintf(file, "%s", res);

    free(res);

    fclose(file);
}
