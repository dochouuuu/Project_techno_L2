#include "game_tools.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "add_edge.h"
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"

#define NB_DIRS 4

typedef struct game_s* game;

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

char shape_to_char(shape s) {
  switch (s) {
    case EMPTY:
      return 'E';
    case ENDPOINT:
      return 'N';
    case SEGMENT:
      return 'S';
    case CORNER:
      return 'C';
    case TEE:
      return 'T';
    case CROSS:
      return 'X';
    default:
      return 'E';
  }
}

char direction_to_char(direction d) {
  switch (d) {
    case NORTH:
      return 'N';
    case EAST:
      return 'E';
    case SOUTH:
      return 'S';
    case WEST:
      return 'W';
    default:
      return 'N';
  }
}

shape char_to_shape(char c) {
  switch (c) {
    case 'E':
      return EMPTY;
    case 'N':
      return ENDPOINT;
    case 'S':
      return SEGMENT;
    case 'C':
      return CORNER;
    case 'T':
      return TEE;
    case 'X':
      return CROSS;
    default:
      return EMPTY;
  }
}

direction char_to_direction(char c) {
  switch (c) {
    case 'N':
      return NORTH;
    case 'E':
      return EAST;
    case 'S':
      return SOUTH;
    case 'W':
      return WEST;
    default:
      return NORTH;
  }
}

game game_load(char* filename) {
  if (filename == NULL) {
    fprintf(stderr, "Filename is null\n");
    return NULL;
  }

  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    fprintf(stderr, "Failed to open the file: %s\n", filename);
    return NULL;
  }

  uint nb_rows, nb_cols;
  int wrapping_int;
  if (fscanf(f, "%u %u %d", &nb_rows, &nb_cols, &wrapping_int) != 3) {
    fprintf(stderr, "Error reading game parameters\n");
    fclose(f);
    return NULL;
  }
  bool wrapping = (wrapping_int == 1);
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  if (g == NULL) {
    fprintf(stderr, "Failed to create a new empty game\n");
    fclose(f);
    return NULL;
  }

  for (uint i = 0; i < nb_rows * nb_cols; i++) {
    char shape_char, direction_char;
    if (fscanf(f, " %c%c", &shape_char, &direction_char) != 2) {
      fprintf(stderr, "Error while reading the file\n");
      game_delete(g);
      fclose(f);
      return NULL;
    }

    g->shapes[i] = char_to_shape(shape_char);
    g->orientations[i] = char_to_direction(direction_char);
  }
  fclose(f);
  return g;
}

void game_save(cgame g, char* filename) {
  if (g == NULL) {
    fprintf(stderr, "Game pointer is null\n");
    return;
  }

  if (filename == NULL) {
    fprintf(stderr, "Filename is null\n");
    return;
  }

  FILE* file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Failed to open the file: %s\n", filename);
    return;
  }

  fprintf(file, "%d %d %d\n", game_nb_rows(g), game_nb_cols(g),
          game_is_wrapping(g));
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      shape s = g->shapes[i * game_nb_cols(g) + j];
      direction d = g->orientations[i * game_nb_cols(g) + j];
      fprintf(file, "%c%c ", shape_to_char(s), direction_to_char(d));
    }
    if (i == game_nb_rows(g) - 1) {
      fprintf(file, " ");
    } else {
      fprintf(file, "\n");
    }
  }
  fclose(file);
}

game game_random(uint nb_rows, uint nb_cols, bool wrapping, uint nb_empty,
                 uint nb_extra) {
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  if (g == NULL) {
    return NULL;
  }

  // Placement aléatoire d'un jeu solution à 2 pièces
  uint i = rand() % nb_rows;
  uint j = rand() % nb_cols;
  bool vertical = rand() % 2;

  if (vertical) {
    if (i == nb_rows - 1) {
      i--;  // Ajustement pour éviter de dépasser les limites
    }
    game_set_piece_shape(g, i, j, ENDPOINT);
    game_set_piece_shape(g, i + 1, j, ENDPOINT);
    game_set_piece_orientation(g, i, j, SOUTH);
    game_set_piece_orientation(g, i + 1, j, NORTH);
  } else {
    if (j == nb_cols - 1) {
      j--;  // Ajustement pour éviter de dépasser les limites
    }
    game_set_piece_shape(g, i, j, ENDPOINT);
    game_set_piece_shape(g, i, j + 1, ENDPOINT);
    game_set_piece_orientation(g, i, j, EAST);
    game_set_piece_orientation(g, i, j + 1, WEST);
  }

  // Allouer dynamiquement le tableau 'visited' avec calloc
  bool** visited = (bool**)calloc(nb_rows, sizeof(bool*));
  if (!visited) {
    perror("Erreur d'allocation mémoire pour visited");
    game_delete(g);
    exit(EXIT_FAILURE);
  }
  for (uint row = 0; row < nb_rows; ++row) {
    visited[row] = (bool*)calloc(nb_cols, sizeof(bool));
    if (!visited[row]) {
      perror("Erreur d'allocation mémoire pour visited[row]");
      for (uint r = 0; r < row; ++r) {
        free(visited[r]);
      }
      free(visited);
      game_delete(g);
      exit(EXIT_FAILURE);
    }
  }

  // Marquer les deux premières pièces comme visitées
  visited[i][j] = true;
  if (vertical) {
    visited[i + 1][j] = true;
  } else {
    visited[i][j + 1] = true;
  }

  // Croître l'arbre couvrant en remplissant toute la grille
  uint current_pieces = 2;
  uint total_pieces = nb_rows * nb_cols - nb_empty;

  while (current_pieces < total_pieces) {
    uint i_candidate = rand() % nb_rows;
    uint j_candidate = rand() % nb_cols;
    if (visited[i_candidate][j_candidate]) {
      direction d = rand() % NB_DIRS;
      int ni = i_candidate + (d == NORTH ? -1 : d == SOUTH ? 1 : 0);
      int nj = j_candidate + (d == WEST ? -1 : d == EAST ? 1 : 0);

      if (wrapping) {
        ni = (ni + nb_rows) % nb_rows;
        nj = (nj + nb_cols) % nb_cols;
      }

      if (ni >= 0 && ni < nb_rows && nj >= 0 && nj < nb_cols &&
          !visited[ni][nj]) {
        if (_add_edge(g, i_candidate, j_candidate, d)) {
          visited[ni][nj] = true;
          current_pieces++;
        }
      }
    }
  }

  // Ajouter des arêtes supplémentaires pour créer des cycles
  uint extra_edges_added = 0;
  while (extra_edges_added < nb_extra) {
    uint i_candidate = rand() % nb_rows;
    uint j_candidate = rand() % nb_cols;
    direction d = rand() % NB_DIRS;

    if (game_check_edge(g, i_candidate, j_candidate, d) == NOEDGE) {
      if (_add_edge(g, i_candidate, j_candidate, d)) {
        extra_edges_added++;
      }
    }
  }

  // Marquer les cases vides
  for (uint k = 0; k < nb_empty; k++) {
    uint empty_i, empty_j;
    do {
      empty_i = rand() % nb_rows;
      empty_j = rand() % nb_cols;
    } while (game_get_piece_shape(g, empty_i, empty_j) != EMPTY);

    game_set_piece_shape(g, empty_i, empty_j, EMPTY);
  }

  // Libérer la mémoire allouée pour 'visited'
  for (uint row = 0; row < nb_rows; ++row) {
    free(visited[row]);
  }
  free(visited);

  return g;
}

static bool solve_recc(game g, uint row, uint col) {
  if (g == NULL) {
    fprintf(stderr, "Game pointer is null\n");
    return false;
  }

  // Si on a parcouru toute la grille, on teste si la solution est valide
  if (row >= game_nb_rows(g)) return game_won(g);

  // Calcul des indices pour la case suivante
  uint next_row = (col + 1 >= game_nb_cols(g)) ? row + 1 : row;
  uint next_col = (col + 1) % game_nb_cols(g);

  shape sh = game_get_piece_shape(g, row, col);
  // Si la pièce est EMPTY ou CROSS, on passe directement à la suivante
  if (sh == EMPTY || sh == CROSS) {
    return solve_recc(g, next_row, next_col);
  }

  // Pour SEGMENT, seules 2 orientations sont possibles, sinon NB_DIRS
  uint max_dir = (sh == SEGMENT) ? 2 : NB_DIRS;

  for (uint d = 0; d < max_dir; ++d) {
    game_set_piece_orientation(g, row, col, d);

    // Cas sans wrapping
    if (!game_is_wrapping(g)) {
      // Vérifie la connexion à l'ouest (sauf si en 1ère colonne)
      if (col > 0 && game_check_edge(g, row, col, WEST) == MISMATCH) {
        continue;  // Si mismatch, essayer la prochaine orientation
      }
      // Vérifie la connexion au nord (sauf si en 1ère ligne)
      if (row > 0 && game_check_edge(g, row, col, NORTH) == MISMATCH) {
        continue;  // Si mismatch, essayer la prochaine orientation
      }
    }
    // Cas avec wrapping
    else {
      // Si on est dans la première colonne, on ignore le test de connexion à
      // l'ouest
      if (col != 0 && game_check_edge(g, row, col, WEST) == MISMATCH) {
        continue;  // Si mismatch, essayer la prochaine orientation
      }
      // Si on est dans la première ligne, on ignore le test de connexion au
      // nord
      if (row != 0 && game_check_edge(g, row, col, NORTH) == MISMATCH) {
        continue;  // Si mismatch, essayer la prochaine orientation
      }
    }

    // Si aucune vérification n'a échoué, on continue avec la case suivante
    if (solve_recc(g, next_row, next_col)) return true;
  }

  return false;  // Si aucune orientation ne permet de trouver une solution
}

bool game_solve(game g) {
  game g_copy = game_copy(g);  // Sauvegarde de l'état initial

  bool solved = solve_recc(g, 0, 0);
  if (!solved) {
    // Restaurer l'état initial si aucune solution n'a été trouvée
    for (uint row = 0; row < game_nb_rows(g); row++) {
      for (uint col = 0; col < game_nb_cols(g); col++) {
        game_set_piece_orientation(
            g, row, col, game_get_piece_orientation(g_copy, row, col));
      }
    }
  }

  game_delete(g_copy);
  return solved;
}

static void count_sol_recc(game g, uint num_row, uint num_col,
                           uint* sol_count) {
  if (g == NULL) {
    fprintf(stderr, "Game pointer is null\n");
    exit(EXIT_FAILURE);
  }

  if (num_row >= game_nb_rows(g)) {
    // Si on a trouvé une solution, on incrémente le compteur et on affiche la
    // solution
    if (game_won(g)) {
      (*sol_count)++;
      game_print(g);
    }
    return;
  }

  uint next_row = (num_col + 1 >= game_nb_cols(g)) ? num_row + 1 : num_row;
  uint next_col = (num_col + 1) % game_nb_cols(g);

  shape current_shape = game_get_piece_shape(g, num_row, num_col);
  // Si la pièce est EMPTY ou CROSS, on passe directement à la suivante
  if (current_shape == EMPTY || current_shape == CROSS) {
    return count_sol_recc(g, next_row, next_col, sol_count);
  }

  // Si la pièce est SEGMENT, il n'y a que 2 directions possibles
  uint max_directions = (current_shape == SEGMENT) ? 2 : NB_DIRS;

  // On teste chaque orientation et passe à la case suivante
  for (uint d = 0; d < max_directions; ++d) {
    game_set_piece_orientation(g, num_row, num_col, d);

    // On s'arrête s'il y a un mismatch à l'ouest ou au nord
    if (!game_is_wrapping(g)) {
      if (game_check_edge(g, num_row, num_col, WEST) != MISMATCH &&
          game_check_edge(g, num_row, num_col, NORTH) != MISMATCH) {
        // Pas de mismatch, on continue
        count_sol_recc(g, next_row, next_col, sol_count);
      }
    } else {
      // Cas avec wrapping
      if ((num_row == 0 || num_row == game_nb_rows(g) - 1) &&
          (num_col == 0 || num_col == game_nb_cols(g) - 1)) {
        count_sol_recc(g, next_row, next_col, sol_count);
      } else {
        if (game_check_edge(g, num_row, num_col, WEST) != MISMATCH &&
            game_check_edge(g, num_row, num_col, NORTH) != MISMATCH) {
          // Pas de mismatch, on continue
          count_sol_recc(g, next_row, next_col, sol_count);
        }
      }
    }
  }
}

uint game_nb_solutions(cgame g) {
  if (!g) return 0;
  uint sol_count = 0;
  game g_copy = game_copy(g);

  if (!g_copy) {
    fprintf(stderr, "Failed to copy game\n");
    return 0;
  }

  count_sol_recc(g_copy, 0, 0, &sol_count);
  if (sol_count == 0) {
    for (uint row = 0; row < game_nb_rows(g); row++) {
      for (uint col = 0; col < game_nb_cols(g); col++) {
        game_set_piece_orientation(g_copy, row, col,
                                   game_get_piece_orientation(g, row, col));
      }
    }
  }

  game_delete(g_copy);
  return sol_count;
}