#include "game.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue.h"

typedef struct game_s *game;

game game_new_empty(void) {
  game g = malloc(sizeof(struct game_s));
  if (g == NULL) {
    return NULL;
  }

  // Initialise les dimensions
  g->nb_cols = 5;
  g->nb_rows = 5;

  // Initialise les tableaux des formes et orientations
  g->shapes = malloc(g->nb_cols * g->nb_rows * sizeof(shape));
  g->orientations = malloc(g->nb_cols * g->nb_rows * sizeof(direction));
  if (g->shapes == NULL || g->orientations == NULL) {
    game_delete(g);
    return NULL;
  }

  for (uint i = 0; i < g->nb_cols * g->nb_rows; i++) {
    g->shapes[i] = EMPTY;
    g->orientations[i] = NORTH;
  }

  // Alloue et initialise les piles pour undo/redo
  g->undo = queue_new();
  g->redo = queue_new();
  if (g->undo == NULL || g->redo == NULL) {
    game_delete(g);
    return NULL;
  }

  return g;
}

game game_new(shape *shapes, direction *orientations) {
  game g = game_new_empty();
  if (g == NULL) {
    return NULL;
  }

  // Initialise les tableaux des formes et orientations avec les valeurs données
  for (uint i = 0; i < g->nb_cols * g->nb_rows; i++) {
    g->shapes[i] = (shapes != NULL) ? shapes[i] : EMPTY;
    g->orientations[i] = (orientations != NULL) ? orientations[i] : NORTH;
  }

  return g;
}

game game_copy(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }

  shape *shapes = malloc((game_nb_rows(g) * game_nb_cols(g)) * sizeof(shape));
  if (shapes == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  direction *orientations =
      malloc((game_nb_rows(g) * game_nb_cols(g)) * sizeof(direction));
  if (orientations == NULL) {
    free(shapes);
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  // Copie les formes et orientations du jeu existant dans le nouveau jeu
  for (uint i = 0; i < game_nb_rows(g) * game_nb_cols(g); i++) {
    shapes[i] = g->shapes[i];
    orientations[i] = g->orientations[i];
  }

  // Créer un nouveau jeu avec les mêmes caractéristiques que l'original
  game new_game = game_new_ext(game_nb_rows(g), game_nb_cols(g), shapes,
                               orientations, game_is_wrapping(g));
  free(shapes);
  free(orientations);
  return new_game;
}

bool game_equal(cgame g1, cgame g2, bool ignore_orientation) {
  if (g1 == NULL || g2 == NULL || game_nb_cols(g1) != game_nb_cols(g2) ||
      game_nb_rows(g1) != game_nb_rows(g2) ||
      game_is_wrapping(g1) != game_is_wrapping(g2)) {
    return false;
  }

  if (g1->shapes == NULL || g2->shapes == NULL) {
    fprintf(stderr, "Shapes are not allocated in one of the games.\n");
    return false;
  }

  if (!ignore_orientation &&
      (g1->orientations == NULL || g2->orientations == NULL)) {
    fprintf(stderr, "Orientations are not allocated in one of the games.\n");
    return false;
  }

  // Compare les formes et orientations pour chaque case du jeu
  for (uint i = 0; i < game_nb_cols(g1) * game_nb_rows(g1); i++) {
    if (g1->shapes[i] != g2->shapes[i]) {
      return false;
    }

    if (!ignore_orientation && g1->orientations[i] != g2->orientations[i]) {
      return false;
    }
  }

  return true;
}

void game_delete(game g) {
  if (g == NULL) {
    fprintf(stderr, "Pointeur non valide\n");
    exit(EXIT_FAILURE);
  }

  if (g->undo != NULL) {
    queue_free_full(g->undo, (void (*)(void *))game_delete);
    g->undo = NULL;
  }

  if (g->redo != NULL) {
    queue_free_full(g->redo, (void (*)(void *))game_delete);
    g->redo = NULL;
  }

  if (g->shapes != NULL) {
    free(g->shapes);
    g->shapes = NULL;
  }

  if (g->orientations != NULL) {
    free(g->orientations);
    g->orientations = NULL;
  }

  free(g);
}

void game_set_piece_shape(game g, uint i, uint j, shape s) {
  assert(g != NULL);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  g->shapes[i * g->nb_cols + j] = s;
}

void game_set_piece_orientation(game g, uint i, uint j, direction o) {
  assert(g != NULL);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  g->orientations[i * g->nb_cols + j] = o;
}

shape game_get_piece_shape(cgame g, uint i, uint j) {
  assert(g != NULL);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  return g->shapes[i * g->nb_cols + j];
}

direction game_get_piece_orientation(cgame g, uint i, uint j) {
  assert(g != NULL);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  return g->orientations[i * g->nb_cols + j];
}

void game_play_move(game g, uint i, uint j, int nb_quarter_turns) {
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }

  if (i >= g->nb_rows || j >= g->nb_cols) {
    fprintf(stderr, "Out-of-bounds index\n");
    exit(EXIT_FAILURE);
  }

  uint index = i * g->nb_cols + j;

  if (g->shapes[index] == EMPTY) {
    fprintf(stderr, "EMPTY piece at (%u, %u), no changes made.\n", i, j);
    return;
  }

  // Sauvegarder l'état actuel dans la pile undo
  if (g->undo == NULL) {
    fprintf(stderr, "Undo stack not initialized\n");
    exit(EXIT_FAILURE);
  }

  game copy = game_copy(g);
  if (copy == NULL) {
    fprintf(stderr, "Failed to copy game for undo stack\n");
    exit(EXIT_FAILURE);
  }
  queue_push_head(g->undo, copy);

  while (!queue_is_empty(g->redo)) {
    game redo_game = queue_pop_head(g->redo);
    game_delete(redo_game);
  }

  // Vider la pile redo car un nouveau coup a été joué
  while (!queue_is_empty(g->redo)) {
    game redo_game = queue_pop_head(g->redo);
    game_delete(redo_game);
  }

  // Calculer et appliquer la nouvelle orientation
  int new_orientation = (g->orientations[index] + nb_quarter_turns) % NB_DIRS;
  if (new_orientation < 0) {
    new_orientation += NB_DIRS;  // Corrige les orientations négatives
  }
  g->orientations[index] = (direction)new_orientation;

  /*printf("Piece at (%u, %u) rotated. New orientation: %d\n", i, j,
         g->orientations[index]);*/
}

bool game_won(cgame g) {
  if (g == NULL) {
    exit(EXIT_FAILURE);
  }
  // Vérifier la connectivité
  if (!game_is_connected(g)) {
    return false;
  }
  // Vérifier les appairages
  if (!game_is_well_paired(g)) {
    return false;
  }
  return true;
}

void game_reset_orientation(game g) {
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }

  // Initialise les orientations à NORTH pour chaque pièce du jeu
  for (uint i = 0; i < g->nb_rows; i++) {
    for (uint j = 0; j < g->nb_cols; j++) {
      uint index = i * g->nb_cols + j;  // Calculer l'index dans le tableau 1D
      g->orientations[index] = NORTH;
    }
  }
}

void game_shuffle_orientation(game g) {
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }

  // Assigne une orientation aléatoire à chauque pièce du jeu
  for (uint i = 0; i < g->nb_rows; i++) {
    for (uint j = 0; j < g->nb_cols; j++) {
      uint index = i * g->nb_cols + j;  // Calculer l'index dans le tableau 1D
      g->orientations[index] =
          (direction)(rand() % NB_DIRS);  // Orientation aléatoire
    }
  }
}