all: programa

programa: board.o barrier.o game.o
	gcc -Wall -Wextra -Werror -pthread -g -std=c99 -o main main.c board.o barrier.o game.o

board.o: ./board/board.c
	gcc -Wall -Wextra -Werror -pthread -g -std=c99 -c ./board/board.c

barrier.o: ./barrier/barrier.c
	gcc -Wall -Wextra -Werror -pthread -g -std=c99 -c ./barrier/barrier.c

game.o: ./game/game.c
	gcc -Wall -Wextra -Werror -pthread -g -std=c99 -c ./game/game.c

clean:
	rm -f programa *.o
