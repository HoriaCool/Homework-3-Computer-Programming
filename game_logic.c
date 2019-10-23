// Copyright 2018 Nedelcu Horia (nedelcu.horia.alexandru@gmail.com)

#include "./game_preparation.h"
#include "./game_logic.h"

int start_new_battle(int p1, int p2, elf_player_t *player) {
    // decide and return who win the battle bettwen p1 and p2
    // p1, p2 - indexes for elf players

    // the player with the most stamina attacks first
    if (player[p1].stamina < player[p2].stamina) {
        swap_int(&p1, &p2);
    }

    while (1) {
        player[p2].hp -= player[p1].dmg;
        if (player[p2].hp <= 0) {
            player[p2].hp = 0;
            player[p1].elim++; // p1 eliminated p2
            player[p1].stamina += player[p2].stamina; // p1 get stamina of p2
            player[p2].stamina = 0; // p2 stamina become 0
            return p1; // p1 won
        }

        player[p1].hp -= player[p2].dmg;
        if (player[p1].hp <= 0) {
            player[p1].hp = 0;
            player[p2].elim++; // p2 eliminated p1
            player[p2].stamina += player[p1].stamina; // p2 get stamina of p1
            player[p1].stamina = 0; // p2 stamina become 0
            return p2; // p2 won
        }
    }
}

void move_up_down_left_right(FILE *fout, int dx, int dy, int index_pl,
    dates_for_glacierfight_game_t *dates) {
    // check if the current move can be executed:

    int x, y, p1, p2, winner, radius, needed_stamina;
    // x, y player coordinates
    // p1, p2 indexes for players that will battle

    x = dates->player[index_pl].x;
    y = dates->player[index_pl].y;
    radius = dates->radius;
    dates->map[x][y].index = 0; // the elf is about to move
    // new coordinates:
    x += dx;
    y += dy;

    if (x == -1 || y == -1 ||
            x == dates->radius * 2 + 1 || y == dates->radius * 2 + 1) {
        // the player has left the map
        dates->player[index_pl].hp = 0;
        dates->rm_pls--;

        fprintf(fout, "%s fell off the glacier.\n",
            dates->player[index_pl].name);
        return;
    }

    // needed stamina = |new altitude - old altitude|:
    needed_stamina = abs(dates->map[x][y].altitude -
        dates->map[x - dx][y - dy].altitude);

    // check if the elf doesn't have enough stamina for a new position:
    if (dates->player[index_pl].stamina < needed_stamina) {
        // return to old coordinates:
        x -= dx;
        y -= dy;
        dates->map[x][y].index = index_pl + 1;
        return;
    } else { // he have enough stamina
        dates->player[index_pl].stamina -= needed_stamina;
    }

    // the player is no longer on the glacier:
    if (!valid_inside_circle(x, y, dates->radius, dates->radius, radius)) {
        dates->player[index_pl].hp = 0;
        dates->rm_pls--;

        fprintf(fout, "%s fell off the glacier.\n",
            dates->player[index_pl].name);
        return;
    }

    // check if he find better dmg-gloves and swap them
    if (dates->player[index_pl].dmg < dates->map[x][y].gloves) {
        swap_int(&dates->player[index_pl].dmg, &dates->map[x][y].gloves);
    }

    if (dates->map[x][y].index) { // check if the cell is not empty
        p1 = index_pl; // player that have moved
        p2 = dates->map[x][y].index - 1; // player that was already there
        winner = start_new_battle(p1, p2, dates->player);

        dates->player[winner].x = x;
        dates->player[winner].y = y;
        dates->map[x][y].index = winner + 1;

        if (p1 != winner) {
            fprintf(fout, "%s sent %s back home.\n",
                dates->player[winner].name, dates->player[p1].name);
        } else {
            fprintf(fout, "%s sent %s back home.\n",
                dates->player[winner].name, dates->player[p2].name);
        }

        dates->rm_pls--;
    } else { // no battle occurred so the player has been put in the new cell
        dates->player[index_pl].x = x;
        dates->player[index_pl].y = y;
        dates->map[x][y].index = index_pl + 1;
    }
}

void move_command(FILE *fout, int index_pl, char *move,
    dates_for_glacierfight_game_t *dates) {
    // execute the list of moves for the elf player index_pl:

    if (dates->player[index_pl].hp == 0) { // the elf is already eliminated
        return;
    }

    int i, number_of_moves, dx, dy; // dx, dy the moving difference
    number_of_moves = strlen(move);

    for (i = 0; i < number_of_moves; ++i) {
        switch (move[i]) {
            case 'U': // up
                dx = -1;
                dy = 0;
                break;
            case 'D': // down
                dx = +1;
                dy = 0;
                break;
            case 'L': // left
                dx = 0;
                dy = -1;
                break;
            case 'R': // right
                dx = 0;
                dy = +1;
                break;
            default:
                break;
        }

        move_up_down_left_right(fout, dx, dy, index_pl, dates);

        if (!dates->player[index_pl].hp) { // the elf is now wet => stop moving
            break;
        }
    }
}

void get_storm_parametres(int k_binary, snowstorm_t *storm) {
    // (int) k_binary = 00000000  00000000  00000000  00000000
    //          <=>     storm.dm  storm.ra  storm.y   storm.x

    #define bits_size_byte 8
    int i, index;

    storm->dmg = 0; // initialize all bits with 0
    index = 3; // byte's index in the number K (k_binary)
    for (i = index * bits_size_byte; i < (index + 1) * bits_size_byte; ++i) {
        storm->dmg |= ((UI) k_binary & ((UI) 1 << i)) >> index * bits_size_byte;
    }

    storm->radius = 0; // initialize all bits with 0
    index = 2; // byte's index in the number K (k_binary)
    for (i = index * bits_size_byte; i < (index + 1) * bits_size_byte; ++i) {
        storm->radius |= ((UI) k_binary & ((UI) 1 << i)) >> index *
            bits_size_byte;
    }

    storm->y = 0; // initialize all bits with 0
    index = 1; // byte's index in the number K (k_binary)
    for (i = index * bits_size_byte; i < (index + 1) * bits_size_byte; ++i) {
        storm->y |= ((UI) k_binary & ((UI) 1 << i)) >> index * bits_size_byte;
    }

    storm->x = 0; // initialize all bits with 0
    index = 0; // byte's index in the number K (k_binary)
    for (i = index * bits_size_byte; i < (index + 1) * bits_size_byte; ++i) {
        storm->x |= ((UI) k_binary & ((UI) 1 << i)) >> index * bits_size_byte;
    }
}

void snowstorm_command(FILE *fout, int k_binary,
    dates_for_glacierfight_game_t *dates) {
    // check if the players are hit by the snowstorm:

    snowstorm_t storm;
    get_storm_parametres(k_binary, &storm);

    for (int i = 0; i < dates->nr_pls; ++i) {
        if (dates->player[i].hp) { // player still in game
            if (valid_inside_circle(dates->player[i].x, dates->player[i].y,
                    storm.x, storm.y, storm.radius)) {
                // player hit by snowstorm

                dates->player[i].hp -= storm.dmg;

                if (dates->player[i].hp <= 0) {
                    dates->player[i].hp = 0;

                    // now the cell is empty:
                    dates->
                        map[dates->player[i].x][dates->player[i].y].index = 0;

                    dates->rm_pls--;

                    fprintf(fout, "%s was hit by snowstorm.\n",
                        dates->player[i].name);
                }
            }
        }
    }
}

void sort_indexes_elf_players(int size, int *vector, elf_player_t *player) {
    int i, stop;

    while (1) {
        stop = 1;
        for (i = 0; i < size - 1; ++i) {
            if (grater_elf_player(player + vector[i], player + vector[i + 1])) {
                swap_int(vector + i, vector + i + 1);
                stop = 0;
            }
        }
        if (stop) {
           break;
        }
    }
}

void print_scoreboard_command(FILE *fout,
    dates_for_glacierfight_game_t *dates) {
    // print the scoreboard:

    int index;

    for (int i = 0; i < dates->nr_pls; ++i) {
        dates->index[i] = i;
    }

    sort_indexes_elf_players(dates->nr_pls, dates->index, dates->player);

    fprintf(fout, "SCOREBOARD:\n");
    for (int i = 0; i < dates->nr_pls; ++i) {
        index = dates->index[i];

        fprintf(fout, "%s\t", dates->player[index].name);
        if (dates->player[index].hp) {
            fprintf(fout, "DRY\t");
        } else {
            fprintf(fout, "WET\t");
        }
        fprintf(fout, "%d\n", dates->player[index].elim);
    }
}

void meltdown_command(FILE *fout, int new_stamina,
    dates_for_glacierfight_game_t *dates) {
    // decreases the radius of the glacier with a unit:

    for (int i = 0; i < dates->nr_pls; ++i) {
        if (dates->player[i].hp) {
            if (!valid_inside_circle(dates->player[i].x, dates->player[i].y,
                    dates->radius, dates->radius, dates->radius - 1)) {
                dates->player[i].hp = 0;

                // the cell is empty now:
                dates->map[dates->player[i].x][dates->player[i].y].index = 0;

                fprintf(fout, "%s got wet because of global warming.\n",
                    dates->player[i].name);
                dates->rm_pls--;
            } else { // the player is still on the glacier
                dates->player[i].stamina += new_stamina;

                // new coordinates:
                dates->player[i].x--;
                dates->player[i].y--;
            }
        }
    }

    reallocate_memory_glacier_cell_t(dates->radius, &dates->map);
    dates->radius--;
}

void read_execute_commands(FILE *fin, FILE *fout,
    dates_for_glacierfight_game_t *dates) {
    // read and execute the game commands:

    int tmp; // variable with multiple purpose
    char *command, *move;

    command = malloc(N_MAX * sizeof(char));
    if (command == NULL) {
        fprintf(stderr, "In function: read_execute_commands\n");
        fprintf(stderr,
            "Error! memory not allocated for variable 'command'.\n");
        close_files_free_memory(fin, fout, dates);
        exit(0);
    }

    move = malloc(N_MAX * sizeof(char));
    if (move == NULL) {
        fprintf(stderr, "In function: read_execute_commands\n");
        fprintf(stderr,
            "Error! memory not allocated for variable 'move'.\n");
        close_files_free_memory(fin, fout, dates);
        free(command);
        exit(0);
    }


    while (fscanf(fin, "%s", command) == 1) {
        if (check_if_elf_won(fout, dates)) {
            free(command);
            free(move);
            return;
        }

        if (!strcmp(command, "MOVE")) {
            // here tmp is the index of the player who moves
            fscanf(fin, "%d%s", &tmp, move);
            move_command(fout, tmp, move, dates);
        }

        if (check_if_elf_won(fout, dates)) {
            free(command);
            free(move);
            return;
        }

        if (!strcmp(command, "SNOWSTORM")) {
            // here tmp is a number K that represent
            // dmg, radius, y, x (coordonates) of the snowstorm
            fscanf(fin, "%d", &tmp);
            snowstorm_command(fout, tmp, dates);
        }

        if (check_if_elf_won(fout, dates)) {
            free(command);
            free(move);
            return;
        }

        if (!strcmp(command, "PRINT_SCOREBOARD")) {
            print_scoreboard_command(fout, dates);
        }

        if (!strcmp(command, "MELTDOWN")) {
            // here tmp is stamina that is given to the remaining players
            fscanf(fin, "%d", &tmp);
            meltdown_command(fout, tmp, dates);
        }
    }

    check_if_elf_won(fout, dates);

    free(command);
    free(move);
}
