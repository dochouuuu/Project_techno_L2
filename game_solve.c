#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"

void usage() {
  fprintf(stderr, "Usage: ./game_solve <option> <input> [<output>]\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -s : Find and save a solution for the game.\n");
  fprintf(
      stderr,
      "  -c : Count the number of possible solutions and save the result.\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  if (argc < 3 || argc > 4) {
    fprintf(stderr, "Error: Incorrect number of arguments.\n");
    usage();
    return EXIT_FAILURE;
  }

  char *option = argv[1];
  char *input_file = argv[2];
  char *output_file = (argc == 4) ? argv[3] : NULL;

  // Charger le jeu depuis le fichier d'entrée
  game g = game_load(input_file);
  if (g == NULL) {
    fprintf(stderr, "Error: Failed to load game from %s.\n", input_file);
    return EXIT_FAILURE;
  }

  if (strcmp(option, "-s") == 0) {
    // Résolution du jeu
    if (game_solve(g)) {
      if (output_file) {
        game_save(g, output_file);
        printf("Solution saved to %s\n", output_file);
      } else {
        printf("Solution found:\n");
        game_print(g);
      }
      game_delete(g);
      return EXIT_SUCCESS;
    } else {
      printf("No solution found.\n");
      game_delete(g);
      return EXIT_FAILURE;
    }
  } else if (strcmp(option, "-c") == 0) {
    // Comptage du nombre de solutions
    uint num_solutions = game_nb_solutions(g);
    if (output_file) {
      FILE *f = fopen(output_file, "w");
      if (f == NULL) {
        fprintf(stderr, "Error: Failed to open output file.\n");
        game_delete(g);
        return EXIT_FAILURE;
      }
      fprintf(f, "%u\n", num_solutions);
      fclose(f);
      printf("Number of solutions saved to %s\n", output_file);
    } else {
      printf("Number of solutions: %u\n", num_solutions);
    }
    game_delete(g);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Error: Unknown option %s.\n", option);
    usage();
    game_delete(g);
    return EXIT_FAILURE;
  }
}