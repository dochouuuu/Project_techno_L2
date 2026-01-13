#include "game_aux.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_struct.h"

typedef struct game_s* game;

int opposite_direction(int d)
{
  static const int opposites[] = {SOUTH, WEST, NORTH, EAST};
  return opposites[d];
}

void game_print(cgame g)
{
  if (g == NULL) {
    fprintf(stderr, "Game pointer is NULL\n");
    exit(EXIT_FAILURE);
  }

  printf("   ");
  for (int i = 0; i < g->size; i++) {
    printf("%d ", i);
  }
  printf("\n");
  printf("   ");

  for (int i = 0; i < (g->size) * 2; i++) {
    printf("-");
  }
  printf("\n");

  for (int i = 0; i < g->size; i++) {
    printf("%d |", i);
    for (int j = 0; j < g->size; j++) {
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
          printf("_ ");
        } else if (d == EAST || d == WEST) {
          printf("| ");
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
      } else {
        printf("? ");
      }
    }
    printf("|");
    printf("\n");
  }
  printf("   ");
  for (int i = 0; i < (g->size) * 2; i++) {
    printf("-");
  }

  printf("\n");
}

game game_default(void)
{
  shape shapes[DEFAULT_SIZE * DEFAULT_SIZE] = {
      CORNER,   ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,    TEE,     TEE,      TEE, TEE,      ENDPOINT, ENDPOINT, TEE,
      ENDPOINT, SEGMENT,  ENDPOINT, TEE,    TEE,      CORNER, SEGMENT, ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT};

  direction orientations[DEFAULT_SIZE * DEFAULT_SIZE] = {WEST, NORTH, WEST,  NORTH, SOUTH, SOUTH, WEST,  NORTH, EAST,
                                                         EAST, EAST,  NORTH, WEST,  WEST,  EAST,  SOUTH, SOUTH, NORTH,
                                                         WEST, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH};

  game g = game_new(shapes, orientations);
  return g;
}

game game_default_solution(void)
{
  shape shapes[DEFAULT_SIZE * DEFAULT_SIZE] = {
      CORNER,   ENDPOINT, ENDPOINT, CORNER, ENDPOINT, TEE,    TEE,     TEE,      TEE, TEE,      ENDPOINT, ENDPOINT, TEE,
      ENDPOINT, SEGMENT,  ENDPOINT, TEE,    TEE,      CORNER, SEGMENT, ENDPOINT, TEE, ENDPOINT, ENDPOINT, ENDPOINT};

  direction orientations[DEFAULT_SIZE * DEFAULT_SIZE] = {EAST,  WEST,  EAST,  SOUTH, SOUTH, EAST,  SOUTH, SOUTH, NORTH,
                                                         WEST,  NORTH, NORTH, EAST,  WEST,  SOUTH, EAST,  SOUTH, NORTH,
                                                         SOUTH, NORTH, EAST,  NORTH, WEST,  NORTH, NORTH};

  game g = game_new(shapes, orientations);
  return g;
}

bool game_get_ajacent_square(cgame g, uint i, uint j, direction d, uint* pi_next, uint* pj_next)
{
  if (g == NULL || i >= g->size || j >= g->size || pi_next == NULL || pj_next == NULL) {
    return false;
  }

  if (d == NORTH) {
    if (i == 0) return false;  // Bordure de la 1ère ligne
    *pi_next = i - 1;
    *pj_next = j;
    return true;
  } else if (d == SOUTH) {
    if (i == g->size - 1) return false;  // Bordure de la dernière ligne
    *pi_next = i + 1;
    *pj_next = j;
    return true;
  } else if (d == EAST) {
    if (j == g->size - 1) return false;  // Bordure de la 1ere colonne
    *pi_next = i;
    *pj_next = j + 1;
    return true;
  } else if (d == WEST) {
    if (j == 0) return false;  // Bordure de la dernière colonne
    *pi_next = i;
    *pj_next = j - 1;
    return true;
  }

  return false;
}

bool game_has_half_edge(cgame g, uint i, uint j, direction d)
{
  if (g == NULL || i >= g->size || j >= g->size || d < 0 || d >= NB_DIRS) {
    return false;
  }
  shape s = game_get_piece_shape(g, i, j);
  direction dir = game_get_piece_orientation(g, i, j);

  // Toute les possibilités de connection pour chaque direction
  if (s != EMPTY) {
    if (d == NORTH) {
      if ((dir == NORTH) || (s == SEGMENT && dir == SOUTH) || (s == CORNER && dir == WEST) ||
          (s == TEE && dir != SOUTH)) {
        return true;
      }
    } else if (d == SOUTH) {
      if ((dir == SOUTH) || (s == SEGMENT && dir == NORTH) || (s == CORNER && dir == EAST) ||
          (s == TEE && dir != NORTH)) {
        return true;
      }

    } else if (d == EAST) {
      if ((dir == EAST) || (s == SEGMENT && dir == WEST) || (s == CORNER && dir == NORTH) ||
          (s == TEE && dir != WEST)) {
        return true;
      }

    } else if (d == WEST) {
      if ((dir == WEST) || (s == SEGMENT && dir == EAST) || (s == CORNER && dir == SOUTH) ||
          (s == TEE && dir != EAST)) {
        return true;
      }

    } else
      return false;
  }
  return false;
}

edge_status game_check_edge(cgame g, uint i, uint j, direction d)
{
  if (g == NULL || i >= g->size || j >= g->size || d < 0 || d >= NB_DIRS) {
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

  // Vérifie si la case voisine (i_next, j_next) a une demi-arête dans la direction opposée
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

bool game_is_well_paired(cgame g)
{
  if (g == NULL) {
    return false;
  }

  uint size = g->size;

  // Vérifie si chaque demi-arête trouvée est bien connectée (MATCH)
  for (uint i = 0; i < size; i++) {
    for (uint j = 0; j < size; j++) {
      for (direction d = 0; d < NB_DIRS; d++) {
        if (game_has_half_edge(g, i, j, d)) {
          edge_status status = game_check_edge(g, i, j, d);
          if (status != MATCH) {
            return false;
          }
        }
      }
    }
  }

  return true;
}

bool visited[DEFAULT_SIZE][DEFAULT_SIZE];
// Fonction auxiliaire pour effectuer un parcours en profondeur
void parcours(cgame g, uint i, uint j)
{
  if (g == NULL || i >= g->size || j >= g->size) {
    fprintf(stderr, "Game pointer is NULL\n");
    exit(EXIT_FAILURE);
  }

  if (visited[i][j]) return;  // Ne pas revisiter une case

  visited[i][j] = true;
  for (direction d = 0; d < NB_DIRS; d++) {
    uint i_next = 0, j_next = 0;

    if (game_get_ajacent_square(g, i, j, d, &i_next, &j_next)) {
      if (game_check_edge(g, i, j, d) == MATCH) {
        parcours(g, i_next, j_next);  // Si la case voisine existe et est connectée, on la visite
      }
    }
  }
}

bool game_is_connected(cgame g)
{
  if (g == NULL) {
    return false;
  }

  // Réinitialiser le tableau `visited`
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      visited[i][j] = false;
    }
  }

  uint start_i = 0, start_j = 0;

  // Trouver une case valide pour commencer le parcours
  bool start = false;
  for (uint i = 0; i < DEFAULT_SIZE && !start; i++) {
    for (uint j = 0; j < DEFAULT_SIZE && !start; j++) {
      for (direction d = 0; d < NB_DIRS; d++) {
        if (game_has_half_edge(g, i, j, d)) {
          start_i = i;
          start_j = j;
          start = true;
          break;
        }
      }
    }
  }

  if (!start) {
    return true;  // Si aucune demi-arête n'est trouvée, alors le jeu est vlide et connecté
  }

  parcours(g, start_i, start_j);

  // Vérifie si toutes les cases contenant des demi-arêtes ont été visitées
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      for (direction d = 0; d < NB_DIRS; d++) {
        if (game_has_half_edge(g, i, j, d) && !visited[i][j]) {
          return false;
        }
      }
    }
  }

  return true;
}