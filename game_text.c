#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

int main(int argc, char *argv[]) {
  game g;

  if (argc > 1) {
    g = game_load(argv[1]);

    if (g == NULL) {
      fprintf(stderr, "Failed to load game from file : %s\n", argv[1]);
      exit(EXIT_FAILURE);
    }

  } else {
    g = game_default();
  }

  while (game_won(g) != true) {
    game_print(g);
    printf("? [h for help]\n");

    char c;
    scanf(" %c", &c);

    if (c == 'h') {
      printf("Action: help\n");
      printf("- Press 'c <i> <j>' to rotate piece clockwise in square (i,j)\n");
      printf(
          "- Press 'a <i> <j>' to rotate piece anti-clockwise in square "
          "(i,j)\n");
      printf("- Press 'r' to shuffle game\n");
      printf("- Press 'z' to undo move\n");
      printf("- Press 'y' to redo move\n");
      printf("- Press 's <filename>' to save a game into a file\n");
      printf("- Press 'q' to quit\n");

    } else if (c == 'r') {
      printf("Action: Restart\n");
      game_shuffle_orientation(g);

    } else if (c == 'q') {
      printf("Action: Quit\n");
      printf("What a shame, you gave up :-(\n");
      game_delete(g);
      return EXIT_SUCCESS;

    } else if (c == 'c' || c == 'a') {
      int i, j;
      scanf(" %d %d", &i, &j);
      if (c == 'c') {
        printf("Action: Play move 'c' into square (%d,%d)\n", i, j);
        game_play_move(g, i, j, 1);
      }
      if (c == 'a') {
        printf("Action: Play move 'a' into square (%d,%d)\n", i, j);
        game_play_move(g, i, j, -1);
      }

    } else if (c == 'z') {
      printf("Action: Undo\n");
      game_undo(g);

    } else if (c == 'y') {
      printf("Action: Redo\n");
      game_redo(g);

    } else if (c == 's') {
      char filename[256];
      scanf(" %s", filename);
      printf("Action: Save game to file '%s'\n", filename);
      game_save(g, filename);

    } else {
      printf("Invalid command\n");
    }
  }

  game_print(g);
  printf("Congratulation! You win!\n");

  game_delete(g);
  return EXIT_SUCCESS;
}