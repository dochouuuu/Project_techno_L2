#include "game_aux.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue.h"

#define MAX_ROWS 100
#define MAX_COLS 100

bool visited[MAX_ROWS][MAX_COLS];

typedef struct game_s *game;

int opposite_direction(int d) {
  static const int opposites[] = {SOUTH, WEST, NORTH, EAST};
  return opposites[d];
}

void game_print(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Game pointer is NULL\n");
    exit(EXIT_FAILURE);
  }

  uint rows = game_nb_rows(g);
  uint cols = game_nb_cols(g);

  printf("   ");
  for (uint j = 0; j < cols; j++) {
    printf("%d ", j);
  }
  printf("\n   ");
  for (uint j = 0; j < cols * 2; j++) {
    printf("-");
  }
  printf("\n");

  for (uint i = 0; i < rows; i++) {
    printf("%d |", i);
    for (uint j = 0; j < cols; j++) {
      shape s = game_get_piece_shape(g, i, j);
      direction d = game_get_piece_orientation(g, i, j);

      if (s == ENDPOINT) {
        if (d == NORTH) {
          printf("^ ");
        } else if (d == SOUTH) {
          printf("v ");
        } else if (d == EAST) {
          printf("> ");
        } else if (d == WEST) {
          printf("< ");
        } else {
          printf("? ");
        }
      } else if (s == SEGMENT) {
        if (d == NORTH || d == SOUTH) {
          printf("| ");
        } else if (d == EAST || d == WEST) {
          printf("- ");
        } else {
          printf("? ");
        }
      } else if (s == CORNER) {
        if (d == NORTH) {
          printf("└ ");
        } else if (d == SOUTH) {
          printf("┐ ");
        } else if (d == EAST) {
          printf("┌ ");
        } else if (d == WEST) {
          printf("┘ ");
        } else {
          printf("? ");
        }
      } else if (s == TEE) {
        if (d == NORTH) {
          printf("┴ ");
        } else if (d == SOUTH) {
          printf("┬ ");
        } else if (d == EAST) {
          printf("├ ");
        } else if (d == WEST) {
          printf("┤ ");
        } else {
          printf("? ");
        }
      } else if (s == CROSS) {
        printf("┼ ");
      } else if (s == EMPTY) {
        printf("  ");
      } else {
        printf("? ");
      }
    }
    printf("|");
    printf("\n");
  }

  printf("   ");
  for (uint j = 0; j < cols * 2; j++) {
    printf("-");
  }
  printf("\n");
}

game game_default(void) {
  shape shapes[5 * 5] = {CORNER,   ENDPOINT, ENDPOINT, CORNER,   ENDPOINT,
                         TEE,      TEE,      TEE,      TEE,      TEE,
                         ENDPOINT, ENDPOINT, TEE,      ENDPOINT, SEGMENT,
                         ENDPOINT, TEE,      TEE,      CORNER,   SEGMENT,
                         ENDPOINT, TEE,      ENDPOINT, ENDPOINT, ENDPOINT};

  direction orientations[5 * 5] = {
      WEST, NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST,
      EAST, EAST,  NORTH, WEST,  WEST,  EAST,  SOUTH, SOUTH, NORTH,
      WEST, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH};

  game g = game_new_ext(5, 5, shapes, orientations, false);
  return g;
}

game game_default_solution(void) {
  shape shapes[5 * 5] = {CORNER,   ENDPOINT, ENDPOINT, CORNER,   ENDPOINT,
                         TEE,      TEE,      TEE,      TEE,      TEE,
                         ENDPOINT, ENDPOINT, TEE,      ENDPOINT, SEGMENT,
                         ENDPOINT, TEE,      TEE,      CORNER,   SEGMENT,
                         ENDPOINT, TEE,      ENDPOINT, ENDPOINT, ENDPOINT};

  direction orientations[5 * 5] = {
      EAST,  WEST,  EAST,  SOUTH, SOUTH, EAST,  SOUTH, SOUTH, NORTH,
      WEST,  NORTH, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH, NORTH,
      SOUTH, NORTH, EAST,  NORTH, WEST,  NORTH, NORTH};

  game g = game_new_ext(5, 5, shapes, orientations, false);
  return g;
}

bool game_get_ajacent_square(cgame g, uint i, uint j, direction d,
                             uint *pi_next, uint *pj_next) {
  if (g == NULL || i >= g->nb_rows || j >= g->nb_cols || pi_next == NULL ||
      pj_next == NULL) {
    return false;
  }

  if (g->wrapping) {
    // Cas avec wrapping
    if (d == NORTH) {
      *pi_next = (i == 0) ? g->nb_rows - 1 : i - 1;
      *pj_next = j;
      return true;
    } else if (d == SOUTH) {
      *pi_next = (i == g->nb_rows - 1) ? 0 : i + 1;
      *pj_next = j;
      return true;
    } else if (d == EAST) {
      *pi_next = i;
      *pj_next = (j == g->nb_cols - 1) ? 0 : j + 1;
      return true;
    } else if (d == WEST) {
      *pi_next = i;
      *pj_next = (j == 0) ? g->nb_cols - 1 : j - 1;
      return true;
    }
  } else {
    // Cas sans wrapping
    if (d == NORTH) {
      if (i == 0) return false;  // Bordure en haut
      *pi_next = i - 1;
      *pj_next = j;
      return true;
    } else if (d == SOUTH) {
      if (i == g->nb_rows - 1) return false;  // Bordure en bas
      *pi_next = i + 1;
      *pj_next = j;
      return true;
    } else if (d == EAST) {
      if (j == g->nb_cols - 1) return false;  // Bordure à droite
      *pi_next = i;
      *pj_next = j + 1;
      return true;
    } else if (d == WEST) {
      if (j == 0) return false;  // Bordure à gauche
      *pi_next = i;
      *pj_next = j - 1;
      return true;
    }
  }

  return false;  // Si la direction est invalide ou non gérée
}

bool game_has_half_edge(cgame g, uint i, uint j, direction d) {
  if (g == NULL || i >= g->nb_rows || j >= g->nb_cols || d < 0 ||
      d >= NB_DIRS || i < 0 || j < 0) {
    return false;
  }
  shape s = game_get_piece_shape(g, i, j);
  direction dir = game_get_piece_orientation(g, i, j);

  // Toute les possibilités de connection pour chaque direction
  if (s == CROSS) {
    return true;
  }
  if (s != EMPTY) {
    if (d == NORTH) {
      if ((dir == NORTH) || (s == SEGMENT && dir == SOUTH) ||
          (s == CORNER && dir == WEST) || (s == TEE && dir != SOUTH)) {
        return true;
      }
    } else if (d == SOUTH) {
      if ((dir == SOUTH) || (s == SEGMENT && dir == NORTH) ||
          (s == CORNER && dir == EAST) || (s == TEE && dir != NORTH)) {
        return true;
      }
    } else if (d == EAST) {
      if ((dir == EAST) || (s == SEGMENT && dir == WEST) ||
          (s == CORNER && dir == NORTH) || (s == TEE && dir != WEST)) {
        return true;
      }
    } else if (d == WEST) {
      if ((dir == WEST) || (s == SEGMENT && dir == EAST) ||
          (s == CORNER && dir == SOUTH) || (s == TEE && dir != EAST)) {
        return true;
      }
    }
  }
  return false;
}

edge_status game_check_edge(cgame g, uint i, uint j, direction d) {
  if (g == NULL || i >= g->nb_cols * g->nb_rows ||
      j >= g->nb_cols * g->nb_rows || d < 0 || d >= NB_DIRS || i < 0 || j < 0) {
    return NOEDGE;
  }

  uint i_next = 0, j_next = 0;
  if (!game_get_ajacent_square(g, i, j, d, &i_next, &j_next)) {
    // Vérifie si une demi-arête est présente sur cette case
    if (game_has_half_edge(g, i, j, d)) {
      return MISMATCH;  // Demi-arête seule sans correspondance
    } else {
      return NOEDGE;  // Pas de demi-arête
    }
  }

  // Vérifie si la case voisine (i_next, j_next) a une demi-arête dans la
  // direction opposée
  bool edge_s = game_has_half_edge(g, i, j, d);
  bool edge_aja_s = false;

  edge_aja_s = game_has_half_edge(g, i_next, j_next, opposite_direction(d));

  // Détermine le statut des 2 arêtes
  if (edge_s && edge_aja_s) {
    return MATCH;
  } else if (edge_s || edge_aja_s) {
    return MISMATCH;
  } else {
    return NOEDGE;
  }
}

bool game_is_well_paired(cgame g) {
  if (g == NULL) {
    return false;
  }

  for (uint i = 0; i < g->nb_rows; i++) {
    for (uint j = 0; j < g->nb_cols; j++) {
      for (direction d = 0; d < NB_DIRS; d++) {
        if (game_has_half_edge(g, i, j, d)) {
          // Vérifie l'état de l'arête
          edge_status status = game_check_edge(g, i, j, d);

          if (status != MATCH) {
            return false;  // Une arête mal appariée ou absente
          }
        }
      }
    }
  }

  return true;
}

// Tableau pour stocker les cases visitées
bool visited[MAX_COLS][MAX_ROWS];

// Fonction pour effectuer un parcours en profondeur
void parcours(cgame g, uint i, uint j) {
  if (g == NULL || i >= g->nb_rows || j >= g->nb_cols) {
    fprintf(stderr, "Position invalide ou jeu NULL : (%u, %u)\n", i, j);
    return;
  }

  if (visited[i][j]) {
    return;  // Ne pas revisiter une case
  }

  visited[i][j] = true;

  for (int d = 0; d < NB_DIRS; d++) {
    uint i_next = 0, j_next = 0;

    if (game_get_ajacent_square(g, i, j, d, &i_next, &j_next)) {
      if (game_check_edge(g, i, j, d) == MATCH) {
        parcours(g, i_next, j_next);  // Si connecté, visite la case voisine
      }
    }
  }
}

// Fonction principale pour vérifier si le jeu est connecté
bool game_is_connected(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Le jeu est NULL\n");
    return false;
  }

  // Réinitialiser le tableau `visited`
  for (uint i = 0; i < g->nb_rows; i++) {
    for (uint j = 0; j < g->nb_cols; j++) {
      visited[i][j] = false;
    }
  }

  uint start_i = 0, start_j = 0;

  // Trouver une case valide pour commencer le parcours
  bool start_found = false;
  for (uint i = 0; i < g->nb_rows && !start_found; i++) {
    for (uint j = 0; j < g->nb_cols && !start_found; j++) {
      for (int d = 0; d < NB_DIRS; d++) {
        if (game_has_half_edge(g, i, j, d)) {
          start_i = i;
          start_j = j;
          start_found = true;
          break;
        }
      }
    }
  }

  if (!start_found) {
    printf(
        "Aucun point de départ trouvé, le jeu est connecté (aucune "
        "demi-arête).\n");
    return true;
  }

  // Lancer le parcours depuis le point de départ
  parcours(g, start_i, start_j);

  // Vérifie si toutes les cases contenant des demi-arêtes ont été visitées
  for (uint i = 0; i < g->nb_rows; i++) {
    for (uint j = 0; j < g->nb_cols; j++) {
      for (int d = 0; d < NB_DIRS; d++) {
        if (game_has_half_edge(g, i, j, d) && !visited[i][j]) {
          return false;
        }
      }
    }
  }

  return true;
}