// Copyright 2018 Nedelcu Horia (nedelcu.horia.alexandru@gmail.com)

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "./game_preparation.h"
#define UC unsigned char
#define UI unsigned int

typedef struct {
    // damage, radius, coordinates:
    UC dmg, radius, y, x;
} snowstorm_t;

static inline int check_if_elf_won(FILE *fout,
    dates_for_glacierfight_game_t *dates) {
    // return true if there is only one elf player on the glacier

    if (dates->rm_pls == 1) {
        for (int i = 0; i < dates->nr_pls; ++i) {
            if (dates->player[i].hp) {
                fprintf(fout, "%s has won.\n", dates->player[i].name);
                break;
            }
        }
        return 1; // true
    }
    return 0; // false
}

static inline int grater_elf_player(const elf_player_t *a,
    const elf_player_t *b) {
    // return 1 (true) if a is grater than b (a is after b in scoreboard)

    if (!a->hp && b->hp) {
        return 1; // true
    }

    if (((a->hp && b->hp) || (!a->hp && !b->hp)) && a->elim < b->elim) {
        return 1; // true
    }
    if (((a->hp && b->hp) || (!a->hp && !b->hp)) && a->elim == b->elim
            && strcmp(a->name, b->name) > 0) {
        return 1; // true
    }

    return 0; // false
}

int start_new_battle(int p1, int p2, elf_player_t *player);
void move_up_down_left_right(FILE *fout, int dx, int dy, int index_pl,
    dates_for_glacierfight_game_t *dates);
void move_command(FILE *fout, int index_pl, char *move,
    dates_for_glacierfight_game_t *dates);

void get_storm_parametres(int k_binary, snowstorm_t *storm);
void snowstorm_command(FILE *fout, int k_binary,
    dates_for_glacierfight_game_t *dates);

void sort_indexes_elf_players(int size, int *vector, elf_player_t *player);
void print_scoreboard_command(FILE *fout, dates_for_glacierfight_game_t *dates);

void meltdown_command(FILE *fout, int new_stamina,
    dates_for_glacierfight_game_t *dates);

void read_execute_commands(FILE *fin, FILE *fout,
    dates_for_glacierfight_game_t *dates);

#endif /* GAME_LOGIC_H */
