#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"

int main(void)
{
  game g = game_default();

  while (game_won(g) != true) {
    game_print(g);
    printf("? [h for help]\n");

    char c;
    scanf(" %c", &c);

    if (c == 'h') {
      printf("action: help\n");
      printf("- press 'c <i> <j>' to rotate piece clockwise in square (i,j)\n");
      printf(
          "- press 'a <i> <j>' to rotate piece anti-clockwise in square "
          "(i,j)\n");
      printf("- press 'r' to shuffle game\n");
      printf("- press 'q' to quit\n");

    } else if (c == 'r') {
      printf("action: restart\n");
      game_shuffle_orientation(g);

    } else if (c == 'q') {
      printf("action: quit\n");
      printf("What a shame, you gave up :-(\n");
      game_delete(g);
      return EXIT_SUCCESS;

    } else if (c == 'c' || c == 'a') {
      int i, j;
      scanf(" %d %d", &i, &j);
      if (c == 'c') {
        printf("action: play move 'c' into square (%d,%d)\n", i, j);
        game_play_move(g, i, j, 1);
      }
      if (c == 'a') {
        printf("action: play move 'a' into square (%d,%d)\n", i, j);
        game_play_move(g, i, j, -1);
      }

    } else {
      printf("Invalid command\n");
    }
  }
  game_print(g);

  return EXIT_SUCCESS;
}