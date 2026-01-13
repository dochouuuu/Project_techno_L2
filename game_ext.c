#include "game_ext.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_struct.h"
#include "queue.h"

#define DEFAULT_QUEUE_SIZE 50

typedef struct game_s *game;

game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping) {
  game g = malloc(sizeof(struct game_s));
  if (g == NULL) {
    fprintf(stderr, "Failed to allocate memory for game structure\n");
    exit(EXIT_FAILURE);
  }

  // Initialisation dimensions
  g->nb_rows = nb_rows;
  g->nb_cols = nb_cols;

  uint total_size = nb_rows * nb_cols;

  // Initialisation et remplissage des tableaux shapes et orientations
  g->shapes = malloc(total_size * sizeof(shape));
  g->orientations = malloc(total_size * sizeof(direction));
  if (g->shapes == NULL || g->orientations == NULL) {
    fprintf(stderr, "Failed to allocate memory for shapes or orientations\n");
    free(g);
    exit(EXIT_FAILURE);
  }

  for (uint i = 0; i < total_size; i++) {
    g->shapes[i] = EMPTY;
    g->orientations[i] = NORTH;
  }

  // Initailisation piles undo et redo
  g->undo = queue_new();
  g->redo = queue_new();
  if (g->undo == NULL || g->redo == NULL) {
    fprintf(stderr, "Failed to initialize undo or redo stacks\n");
    free(g->shapes);
    free(g->orientations);
    queue_free_full(g->undo, NULL);
    queue_free_full(g->redo, NULL);
    free(g);
    exit(EXIT_FAILURE);
  }

  g->wrapping = wrapping;
  return g;
}

game game_new_ext(uint nb_rows, uint nb_cols, shape *shapes,
                  direction *orientations, bool wrapping) {
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping);
  if (g == NULL) {
    return NULL;
  }

  if (shapes != NULL) {
    for (uint i = 0; i < nb_rows * nb_cols; i++) {
      g->shapes[i] = shapes[i];
    }
  }

  if (orientations != NULL) {
    for (uint i = 0; i < nb_rows * nb_cols; i++) {
      g->orientations[i] = orientations[i];
    }
  }

  return g;
}

uint game_nb_rows(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }

  return g->nb_rows;
}

uint game_nb_cols(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }
  return g->nb_cols;
}

bool game_is_wrapping(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }
  return g->wrapping;
}

void game_undo(game g) {
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }

  // Vérifier si la pile d'annulation est vide
  if (queue_is_empty(g->undo)) {
    printf("No move to undo.\n");
    return;
  }

  // Sauvegarder l'état actuel dans la pile redo
  queue_push_head(g->redo, game_copy(g));

  // Restaurer l'état précédent depuis la pile undo
  game previous_game = queue_pop_head(g->undo);

  // Copier les données de l'état précédent dans le jeu actuel
  uint total_size = g->nb_rows * g->nb_cols;
  for (uint i = 0; i < total_size; i++) {
    g->shapes[i] = previous_game->shapes[i];
    g->orientations[i] = previous_game->orientations[i];
  }

  // Libérer l'état précédent après sa restauration
  game_delete(previous_game);

  printf("Move undone successfully.\n");
}

void game_redo(game g) {
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }

  // Vérifier si la pile redo est vide
  if (queue_is_empty(g->redo)) {
    printf("No move to redo.\n");
    return;
  }

  // Sauvegarder l'état actuel dans la pile undo
  queue_push_head(g->undo, game_copy(g));

  // Restaurer l'état suivant depuis la pile redo
  game next_game = queue_pop_head(g->redo);

  // Copier les données de l'état suivant dans le jeu actuel
  uint total_size = g->nb_rows * g->nb_cols;
  for (uint i = 0; i < total_size; i++) {
    g->shapes[i] = next_game->shapes[i];
    g->orientations[i] = next_game->orientations[i];
  }

  // Libérer l'état suivant après sa restauration
  game_delete(next_game);

  printf("Move redone successfully.\n");
}