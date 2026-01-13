#include "game.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_aux.h"
#include "game_struct.h"

typedef struct game_s* game;

game game_new_empty(void)
{
  game g = malloc(sizeof(struct game_s));
  if (g == NULL) {
    return NULL;
  }
  g->size = DEFAULT_SIZE;
  uint total_size = g->size * g->size;
  g->shapes = malloc(total_size * sizeof(shape));
  g->orientations = malloc(total_size * sizeof(direction));
  assert(g->shapes != NULL && g->orientations != NULL);

  for (int i = 0; i < total_size; i++) {
    g->shapes[i] = EMPTY;
    g->orientations[i] = NORTH;
  }
  return g;
}

game game_new(shape* shapes, direction* orientations)
{
  game g = game_new_empty();
  if (g == NULL) {
    return NULL;
  }
  uint total_size = DEFAULT_SIZE * DEFAULT_SIZE;

  if (shapes != NULL) {
    for (uint i = 0; i < total_size; i++) {
      g->shapes[i] = shapes[i];
    }
  }
  if (orientations != NULL) {
    for (uint i = 0; i < total_size; i++) {
      g->orientations[i] = orientations[i];
    }
  }

  return g;
}

game game_copy(cgame g)
{
  if (g == NULL) {
    return NULL;
  }

  game new_game = malloc(sizeof(struct game_s));
  if (new_game == NULL) {
    return NULL;
  }

  new_game->size = g->size;

  uint total_size = g->size * g->size;
  new_game->shapes = malloc(total_size * sizeof(shape));
  new_game->orientations = malloc(total_size * sizeof(direction));
  if (new_game->shapes == NULL || new_game->orientations == NULL) {
    free(new_game->shapes);
    free(new_game->orientations);
    free(new_game);
    return NULL;
  }

  for (uint i = 0; i < total_size; i++) {
    new_game->shapes[i] = g->shapes[i];
    new_game->orientations[i] = g->orientations[i];
  }

  return new_game;
}

bool game_equal(cgame g1, cgame g2, bool ignore_orientation)
{
  if (g1 == NULL || g2 == NULL) {
    return false;
  }

  if (g1->size != g2->size) {
    return false;
  }

  uint total_size = g1->size * g2->size;

  for (uint i = 0; i < total_size; i++) {
    if (g1->shapes[i] != g2->shapes[i]) {
      return false;
    }

    if (!ignore_orientation && g1->orientations[i] != g2->orientations[i]) {
      return false;
    }
  }

  return true;
}

void game_delete(game g)
{
  if (g == NULL) {
    fprintf(stderr, "poiteur non valide");
    exit(EXIT_FAILURE);
  }

  free(g->shapes);
  free(g->orientations);
  free(g);
}

void game_set_piece_shape(game g, uint i, uint j, shape s)
{
  assert(g != NULL);
  assert(i < DEFAULT_SIZE);
  assert(j < DEFAULT_SIZE);
  g->shapes[i * g->size + j] = s;
}

void game_set_piece_orientation(game g, uint i, uint j, direction o)
{
  assert(g != NULL);
  assert(i < DEFAULT_SIZE);
  assert(j < DEFAULT_SIZE);
  g->orientations[i * g->size + j] = o;
}

shape game_get_piece_shape(cgame g, uint i, uint j)
{
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }

  if (i > g->size || j > g->size) {
    fprintf(stderr, "Out-of-bounds indice\n");
  }

  return g->shapes[i * g->size + j];
}

direction game_get_piece_orientation(cgame g, uint i, uint j)
{
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }

  if (i > g->size || j > g->size) {
    fprintf(stderr, "Out-of-bounds indice\n");
    exit(EXIT_FAILURE);
  }

  return g->orientations[i * g->size + j];
}

void game_play_move(game g, uint i, uint j, int nb_quarter_turns)
{
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }

  if (i > g->size || j > g->size) {
    fprintf(stderr, "Out-of-bounds indice\n");
    exit(EXIT_FAILURE);
  }

  uint index = i * g->size + j;  // Calcul de l'index dans le tableau 1D

  // Vérifie si la pièce n'est pas vide avant d'appliquer une rotation
  if (g->shapes[index] == EMPTY) {
    fprintf(stderr, "EMPTY piece at (%u, %u), no changes made.\n", i, j);
    exit(EXIT_FAILURE);
  }

  // Calcul de la nouvelle orientation après rotation
  int new_orientation = (g->orientations[index] + nb_quarter_turns) % NB_DIRS;

  if (new_orientation < 0) {
    new_orientation += NB_DIRS;  // Corrige les rotations négatives
  }

  g->orientations[index] = (direction)new_orientation;  // Mise à jour de l'orientation

  printf("Piece at (%u, %u) rotated. New orientation: %d\n", i, j, g->orientations[index]);
}

bool game_won(cgame g)
{
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }

  if (!game_is_connected(g)) {
    return false;  // Toutes les pièces ne sont pas dans une seule composante
                   // connexe
  }

  if (!game_is_well_paired(g)) {
    return false;  // Une pièce n'est pas bien appairée
  }

  return true;
}

void game_reset_orientation(game g)
{
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }

  for (uint i = 0; i < g->size * g->size; i++) {
    g->orientations[i] = NORTH;  // Réinitialise l'orientation à NORTH
  }
}

void game_shuffle_orientation(game g)
{
  if (g == NULL) {
    fprintf(stderr, "Null game pointer\n");
    exit(EXIT_FAILURE);
  }

  for (uint i = 0; i < g->size * g->size; i++) {
    g->orientations[i] = (direction)(rand() % NB_DIRS);
  }
}