# Homework-3-Computer-Programming
Helped me to structure a project and to improve coding style

Nedelcu Horia
Group 311CA

Homework3 - ElfUnknown's - Glacierfight:
   Elf-Players were organized as a vector of structures, while
the map of the game was kept with a matrix of structures with related
information. Players have their coordinates, but the map also stores
in each cell the index of the player who occupies it to speed up the program.

   Main functions:
   - file_preparation: open the files from which it will read and write;
   - read_game_preparation: reads game data and allocates the necessary
memory;
   - read_execute_commands: commands are read and executed as long as they
are in the file and the game is not over;
   - close_files_free_memory: close files and free memory;

   Implementation details are found in the function comments, where they are
explained in more detail.
