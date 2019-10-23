// Copyright 2018 Nedelcu Horia (nedelcu.horia.alexandru@gmail.com)

#ifndef GAME_PREPARATION_H
#define GAME_PREPARATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N_MAX 100

typedef struct {
    char *name;
    int x, y; // coordinates
    int hp, dmg; // health points, damage
    int stamina;
    int elim; // number of players eliminated by this
} elf_player_t;

typedef struct {
    // index: 0 <=> empty cell; not 0 <=> player present in the cell
    int altitude, gloves, index;
} glacier_cell_t;

typedef struct {
    // nr_pls <=> number of players
    // rm_pls <=> remaining players
    // index <=> vector with indexes of elf players for sort command
    int radius, nr_pls, rm_pls, *index;
    elf_player_t *player;
    glacier_cell_t **map;
} dates_for_glacierfight_game_t;

static inline void swap_int(int *a, int *b) {
    int tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

static inline int valid_inside_circle(int x, int y, int x0, int y0, int rad) {
    // x0, y0 <=> coordinates of circle, rad <=> radius

    // change the center of the cartesian system with the center of the circle:
    x -= x0;
    y -= y0;

    if (x * x + y * y <= rad * rad) {
        return 1;
    }
    return 0;
}

char* mystrcat(char* dest, char* src);
char* strconcat(const char *s1, const char *s2);
void open_file(FILE **file, char *file_name, char *mode);
void files_preparation(FILE **fin, FILE **fout, char *str_name);

void free_elf_vector(int n, elf_player_t *v);
void free_matrix(int n, glacier_cell_t **a);
void allocate_memory_elf_player_t(int p, elf_player_t **v, int **w);
void allocate_memory_glacier_cell_t(int r, glacier_cell_t ***a);
void reallocate_memory_glacier_cell_t(int r, glacier_cell_t ***a);

void read_game_preparation(FILE *fin, FILE *fout,
    dates_for_glacierfight_game_t *dates);

void close_files_free_memory(FILE *fin, FILE *fout,
    dates_for_glacierfight_game_t *dates);

#endif /* GAME_PREPARATION_H */
