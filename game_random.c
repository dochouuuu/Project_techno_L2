#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

int main(int argc, char *argv[]) {
  if (argc < 6) {
    printf(
        "Usage: ./game_random <nb_rows> <nb_cols> <wrapping> <nb_empty> "
        "<nb_extra> <shuffle> [<filename>]\n"
        "Example: ./game_random 4 4 0 0 0 0 random.sol\n");
    return EXIT_SUCCESS;
  }
  bool all_correct = true;
  if (atoi(argv[1]) < 2 || atoi(argv[2]) < 2) {
    printf("Error: <nb_rows> and <nb_cols> must be at least 2\n");
    all_correct = false;
  }
  int nb_rows = atoi(argv[1]);
  int nb_cols = atoi(argv[2]);

  int wrapping_arg = atoi(argv[3]);
  if (wrapping_arg != 0 && wrapping_arg != 1) {
    printf("Bad argument for <wrapping>. It should be 0 or 1.\n");
    all_correct = false;
  }
  bool wrapping = (wrapping_arg == 1);

  if (atoi(argv[4]) < 0 || atoi(argv[4]) > nb_rows * nb_cols) {
    printf("Error: <nb_empty> must be between 0 and %d\n", nb_rows * nb_cols);
    all_correct = false;
  }
  int nb_empty = atoi(argv[4]);
  int nb_extra = atoi(argv[5]);

  int shuffle_arg = atoi(argv[6]);
  if (shuffle_arg != 0 && shuffle_arg != 1) {
    printf("Bad argument for <shuffle>. It should be 0 or 1.\n");
    all_correct = false;
  }
  bool shuffle = (shuffle_arg == 1);

  char *filename = (argc == 8) ? argv[7] : NULL;

  if (all_correct) {
    printf(
        "nb_rows = %d, nb_cols = %d, wrapping= %d, nb_empty= %d, nb_extra = "
        "%d, "
        "shuffle = %d\n",
        nb_rows, nb_cols, wrapping, nb_empty, nb_extra, shuffle);

    srand(time(NULL));
    game g = game_random(nb_rows, nb_cols, wrapping, nb_empty, nb_extra);
    if (g == NULL) {
      fprintf(stderr, "Failed to create the game\n");
      exit(EXIT_FAILURE);
    }

    if (shuffle) {
      game_shuffle_orientation(g);
    }

    game_print(g);

    if (filename != NULL) {
      printf("Game saved in %s\n", filename);
      game_save(g, filename);
    }
    if (g != NULL) {
      game_delete(g);
    }
  }
  return EXIT_SUCCESS;
}