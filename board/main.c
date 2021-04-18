#include <stdlib.h>
#include <stdio.h>

#include "board.h"

int main() {
    board_t board;
    char *salida;

    if (!board_init(&board, 3, 3)) {
        if (!board_load(&board, "1X1O1X\n3X\n1X48O\0")) {
            salida = malloc(sizeof(char) * board.row * (board.col + 1));
            board_show(board, salida);
            printf("%s\n", salida);
            free(salida);
        } else
            printf("Mal Parseado\n");
        board_destroy(&board);
    } else
        printf("Error al crear Board\n");
    return 0;
}