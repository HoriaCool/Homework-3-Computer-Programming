// Copyright 2018 Nedelcu Horia (nedelcu.horia.alexandru@gmail.com)

#include "./game_preparation.h"

char* mystrcat(char* dest, char* src) {
    // concatenate src to dest

    while (*dest) {
        dest++;
    }
    while ((*(dest++) = *(src++))) {
    }
    return --dest;
}

char* strconcat(const char *s1, const char *s2) {
    // concatenate 2 strings

    // +1 for the null-terminator
    char *result = malloc((strlen(s1) + strlen(s2) + 1) * sizeof(char));
    if (result == NULL) {
        printf("Error! memory not allocated.\n");
        exit(0);
    }

    result[0] = '\0';
    mystrcat(mystrcat(result, (char *) s1), (char *) s2);

    return result;
}

void open_file(FILE **file, char *file_name, char *mode) {
    *file = fopen(file_name, mode);
    if (*file == NULL) {
        fprintf(stderr, "ERROR: Can't open file %s.", file_name);
        exit(0);
    }
}

void files_preparation(FILE **fin, FILE **fout, char *str_name) {
    char *file_name;

    file_name = strconcat(str_name, ".in");
    open_file(fin, file_name, "r");
    free(file_name);

    file_name = strconcat(str_name, ".out");
    open_file(fout, file_name, "w");
    free(file_name);
}

void free_elf_vector(int n, elf_player_t *v) {
    for (int i = 0; i < n; ++i) {
        free(v[i].name);
    }
    free(v);
}

void free_matrix(int n, glacier_cell_t **a) {
    for (int i = 0; i < n; ++i) {
        free(a[i]);
    }
    free(a);
}

void allocate_memory_elf_player_t(int p, elf_player_t **v, int **w) {
    // alloc memory for p players

    *v = malloc(p * sizeof(elf_player_t));
    if (*v == NULL) {
        fprintf(stderr, "Error! memory not allocated for players.\n");
        exit(0);
    }
    for (int i = 0; i < p; ++i) {
        (*v)[i].name = malloc(N_MAX * sizeof(char));
        if ((*v)[i].name == NULL) {
            fprintf(stderr,
                "Error! memory not allocated for name of player %d.\n", i);
            free_elf_vector(p, *v);
            exit(0);
        }
    }

    *w = malloc(p * sizeof(int));
    if (*w == NULL) {
        fprintf(stderr, "Error! memory not allocated for players.\n");
        exit(0);
    }
}

void allocate_memory_glacier_cell_t(int r, glacier_cell_t ***a) {
    // alloc memory for a glacier of radius r

    r = r * 2 + 1;

    *a = malloc(r * sizeof(glacier_cell_t*));
    if (*a == NULL) {
        fprintf(stderr, "Error! memory not allocated for glacier.\n");
        exit(0);
    }

    for (int i = 0; i < r; ++i) {
        (*a)[i] = malloc(r * sizeof(glacier_cell_t));
        if ((*a)[i] == NULL) {
            fprintf(stderr, "Error! memory not allocated for row %d.\n", i);
            free_matrix(r, *a);
            exit(0);
        }
    }
}

void reallocate_memory_glacier_cell_t(int r, glacier_cell_t ***a) {
    // reduce the radius r of glacier and reallocate memory for it

    glacier_cell_t **tmp;
    allocate_memory_glacier_cell_t(r - 1, &tmp);

    for (int i = 1; i < r * 2; ++i) {
        for (int j = 1; j < r * 2; ++j) {
            tmp[i - 1][j - 1] = (*a)[i][j];
        }
    }

    free_matrix(r * 2 + 1, *a);
    *a = tmp;
}

void read_game_preparation(FILE *fin, FILE *fout,
    dates_for_glacierfight_game_t *dates) {
    // alloc memory and read the dates for glacier fight game:

    int i, j, x, y, hp, stamina;
    char *name = malloc(N_MAX * sizeof(char));
    if (name == NULL) {
        fprintf(stderr, "In function: read_game_preparation\n");
        fprintf(stderr, "Error! memory not allocated for variable 'name'.\n");
        close_files_free_memory(fin, fout, dates);
        exit(0);
    }

    fscanf(fin, "%d%d", &dates->radius, &dates->nr_pls);
    dates->rm_pls = dates->nr_pls;

    allocate_memory_elf_player_t(dates->nr_pls, &dates->player, &dates->index);
    allocate_memory_glacier_cell_t(dates->radius, &dates->map);

    for (i = 0; i < dates->radius * 2 + 1; ++i) {
        for (j = 0; j < dates->radius * 2 + 1; ++j) {
            fscanf(fin, "%d%d", &dates->map[i][j].altitude,
                &dates->map[i][j].gloves);
            dates->map[i][j].index = 0; // initially there aren't players
        }
    }

    for (i = 0; i < dates->nr_pls; ++i) {
        fscanf(fin, "%s%d%d%d%d", name, &x, &y, &hp, &stamina);

        dates->player[i].name[0] = '\0';
        mystrcat(dates->player[i].name, name);
        dates->player[i].x = x;
        dates->player[i].y = y;
        dates->player[i].hp = hp;
        dates->player[i].dmg = dates->map[x][y].gloves;
        dates->player[i].stamina = stamina;
        // initially there aren't eliminated players by anybody
        dates->player[i].elim = 0;

        if (valid_inside_circle(x, y, dates->radius, dates->radius,
            dates->radius)) {
            dates->map[x][y].gloves = 0; // get gloves from cell x y
            dates->map[x][y].index = i + 1; // put player i in cell x y
        } else {
            fprintf(fout, "%s has missed the glacier.\n", name);
            dates->player[i].hp = 0; // player is eliminated
            dates->rm_pls--;
        }
    }

    free(name);
}

void close_files_free_memory(FILE *fin, FILE *fout,
    dates_for_glacierfight_game_t *dates) {
    // close the .in and .out files and free the memory

    free(dates->index);
    free_elf_vector(dates->nr_pls, dates->player);
    free_matrix(dates->radius * 2 + 1, dates->map);

    fclose(fin);
    fclose(fout);
}
