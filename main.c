// Copyright 2018 Nedelcu Horia (nedelcu.horia.alexandru@gmail.com)

#include "./game_preparation.h"
#include "./game_logic.h"

int main(int argc, char *argv[]) {
    FILE *fin, *fout;
    dates_for_glacierfight_game_t dates;

    files_preparation(&fin, &fout, argv[argc - 1]);
    read_game_preparation(fin, fout, &dates);
    read_execute_commands(fin, fout, &dates);
    close_files_free_memory(fin, fout, &dates);

    return 0;
}
