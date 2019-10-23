build: main.c game_preparation.c game_logic.c
	gcc -Wall -Wextra -std=c99 main.c game_preparation.c game_logic.c -o snowfight -lm
clean:
	rm -f snowfight
