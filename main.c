#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "./game/game.h"

int main(int argc, char **argv) {
    char str[20];
    game_t *game;

    if (argc < 2)
        return 0;

    game = loadGame(*(argv + 1));

    sscanf(*(argv + 1), "%s.", str);

    strcat(str, ".final");

    printf("%s\n", str);

    writeBoard(*(game->board), str);

    printf("%s\n", str);

    return 0;
}