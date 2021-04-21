#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>

#include "./game/game.h"

int main(int argc, char **argv) {
    char str[20];
    game_t *game;

    if (argc < 2)
        return 0;

    game = loadGame(*(argv + 1));

    sscanf(*(argv + 1), "%s.", str);

    strcat(str, ".final");

    if (game) {
        game->board = congwayGoL(game->board, game->cycles, get_nprocs());
        
        if (game->board)
            writeBoard(*(game->board), str);
    }
    else
        perror("rompo");

    return 0;
}