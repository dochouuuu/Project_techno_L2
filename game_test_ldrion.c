#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"

int test_dummy() { return 0; }

bool test_game_default(void)
{
  game g1 = game_default();
  game g2 = game_default();

  // Vérifie que les cases ne sont pas vides et que toutes les orientations sont
  // valides
  bool all_filled = true;
  bool all_orientations_valid = true;

  for (unsigned int i = 0; i < DEFAULT_SIZE; i++) {
    for (unsigned int j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_piece_shape(g1, i, j) == EMPTY) {
        all_filled = false;
        printf("Case vide trouvée à (%u, %u)\n", i, j);
      }
      direction orientation = game_get_piece_orientation(g1, i, j);
      if (orientation < 0 || orientation > 3) {
        all_orientations_valid = false;
        printf("Orientation invalide à (%u, %u), trouvée: %d\n", i, j, orientation);
      }
    }
  }

  // Vérifie si le jeu par défault est toujours le même
  bool are_same = game_equal(g1, g2, false);

  game_delete(g1);
  game_delete(g2);

  // Renvoie true si toutes les cases sont remplies, orientations valides et les
  // jeux sont identiques
  return all_filled && all_orientations_valid && are_same;
}

bool test_game_default_solution(void)
{
  game g_default = game_default();

  game g_solution = game_default_solution();

  game g_solution2 = game_default_solution();

  bool are_equal = !game_equal(g_default, g_solution, false);
  bool is_game_won = game_won(g_solution);  // Vérifie si la solution est gagnante

  // Vérifie si la solution du jeu par défault est toujours la même
  bool are_same = game_equal(g_solution, g_solution2, false);

  game_delete(g_default);
  game_delete(g_solution);
  game_delete(g_solution2);

  // Retourne vrai si les jeux sont différents et si la solution est gagnante
  return are_equal && is_game_won && are_same;
}

bool test_game_get_adjacent_square(void)
{
  game g = game_default();

  unsigned int i_next = 0;
  unsigned int j_next = 0;

  bool is_outside1 = !game_get_ajacent_square(g, 0, 0, NORTH, &i_next, &j_next);
  bool is_outside2 = !game_get_ajacent_square(g, 0, 0, WEST, &i_next, &j_next);
  bool test1 = (i_next == 0 && j_next == 0);

  bool is_inside = game_get_ajacent_square(g, 1, 1, EAST, &i_next, &j_next);
  bool test = (i_next == 1 && j_next == 2);

  game_delete(g);
  return test && test1 && is_outside1 && is_outside2 && is_inside;
}

bool test_game_has_half_edge(void)
{
  shape shapes[5 * 5] = {ENDPOINT, SEGMENT, ENDPOINT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                         EMPTY,    EMPTY,   EMPTY,    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                         EMPTY,    EMPTY,   EMPTY,    EMPTY, EMPTY, EMPTY, EMPTY};

  direction orientations[5 * 5] = {EAST,  EAST,  WEST,  NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
                                   NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
                                   NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH};

  game g = game_new(shapes, orientations);

  bool result = true;
  game_print(g);
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      for (direction d = NORTH; d < NB_DIRS; d++) {
        bool has_half_edge = game_has_half_edge(g, i, j, d);

        if (i == 0 && j == 0 && game_get_piece_shape(g, i, j) == ENDPOINT && d == EAST) {
          if (!has_half_edge) {
            printf(
                "Erreur : ENDPOINT à (%u, %u) devrait avoir une demi-arête "
                "dans la direction %d.\n",
                i, j, d);
            result = false;
          }
        } else if (i == 1 && j == 0 && game_get_piece_shape(g, i, j) == SEGMENT && (d == EAST || d == WEST)) {
          if (!has_half_edge) {
            printf(
                "Erreur : SEGMENT à (%u, %u) devrait avoir une demi-arête dans "
                "la direction %d.\n",
                i, j, d);
            result = false;
          }
        } else if (i == 2 && j == 0 && game_get_piece_shape(g, i, j) == ENDPOINT && d == WEST) {
          if (!has_half_edge) {
            printf(
                "Erreur : ENDPOINT à (%u, %u) devrait avoir une demi-arête dans "
                "la direction %d.\n",
                i, j, d);
            result = false;
          }
        }
      }
    }
  }
  game g_empty = game_new_empty();
  game_print(g_empty);
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      for (direction d = NORTH; d < NB_DIRS; d++) {
        bool has_half_edge = game_has_half_edge(g_empty, i, j, d);
        if (has_half_edge) {
          printf(
              "Erreur : Jeu vide à (%u, %u) ne devrait pas avoir de demi-arête "
              "dans la direction %d.\n",
              i, j, d);
          result = false;
        }
      }
    }
  }
  game_delete(g_empty);
  game_delete(g);
  return result;
}

bool test_game_check_edge()
{
  game g = game_default();
  bool all_tests_passed = true;

  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      for (direction d = NORTH; d < NB_DIRS; d++) {
        edge_status status = game_check_edge(g, i, j, d);
        bool has_half_edge = game_has_half_edge(g, i, j, d);

        if ((status == MATCH && has_half_edge == false) || (status == NOEDGE && has_half_edge == true)) {
          printf("Erreur : à (%u, %u), direction %d, bool : %d\n", i, j, d, has_half_edge);
          all_tests_passed = false;
        }
      }
    }
  }

  game_delete(g);
  return all_tests_passed;
}

bool test_game_is_well_paired(void)
{
  game g_default = game_default_solution();
  bool is_paired_default = game_is_well_paired(g_default);
  if (!is_paired_default) {
    printf("Erreur : La solution par défaut n'est pas bien appariée.\n");
  }

  game g_modified_solution = game_default_solution();
  game_set_piece_orientation(g_modified_solution, 0, 0, SOUTH);
  bool is_not_paired = game_is_well_paired(g_modified_solution);
  if (is_not_paired) {
    printf("Erreur : La grille modifiée devrait être mal appariée.\n");
  }

  game g_empty = game_new_empty();
  bool is_paired_empty = game_is_well_paired(g_empty);
  if (!is_paired_empty) {
    printf("Erreur : Le jeu vide devrait être bien apparié.\n");
  }
  shape shapes[5 * 5] = {EMPTY,    EMPTY,   EMPTY,    EMPTY,   ENDPOINT, EMPTY,   EMPTY, EMPTY, ENDPOINT,
                         SEGMENT,  EMPTY,   EMPTY,    EMPTY,   SEGMENT,  SEGMENT, EMPTY, EMPTY, EMPTY,
                         ENDPOINT, SEGMENT, ENDPOINT, SEGMENT, SEGMENT,  SEGMENT, TEE};

  direction orientations[5 * 5] = {NORTH, NORTH, NORTH, NORTH, SOUTH, NORTH, NORTH, NORTH, SOUTH,
                                   NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
                                   NORTH, NORTH, EAST,  EAST,  EAST,  EAST,  WEST};

  game g_new = game_new(shapes, orientations);
  bool test = !game_is_well_paired(g_new);
  bool all_edges_paired = true;
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      for (direction d = 0; d < NB_DIRS; d++) {
        edge_status status = game_check_edge(g_default, i, j, d);
        bool has_half_edge = game_has_half_edge(g_default, i, j, d);
        if (has_half_edge && status != MATCH) {
          printf("Erreur : Demi-arête mal appariée en (%u, %u) direction %d.\n", i, j, d);
          all_edges_paired = false;
        }
      }
    }
  }

  if (!all_edges_paired) {
    printf("Erreur : Certaines demi-arêtes ne sont pas correctement appariées.\n");
  }

  game_delete(g_default);
  game_delete(g_modified_solution);
  game_delete(g_empty);
  game_delete(g_new);

  return is_paired_default && !is_not_paired && is_paired_empty && all_edges_paired && test;
}

bool test_game_is_connected(void)
{
  shape shapes[5 * 5] = {EMPTY,    EMPTY, EMPTY,    EMPTY,   EMPTY,    EMPTY, EMPTY, EMPTY, ENDPOINT,
                         EMPTY,    EMPTY, EMPTY,    EMPTY,   SEGMENT,  EMPTY, EMPTY, EMPTY, EMPTY,
                         ENDPOINT, EMPTY, ENDPOINT, SEGMENT, ENDPOINT, EMPTY, EMPTY};

  direction orientations[5 * 5] = {NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, SOUTH,
                                   NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
                                   NORTH, NORTH, EAST,  EAST,  WEST,  NORTH, NORTH};
  shape shapes_connected[5 * 5] = {EMPTY,    EMPTY, EMPTY, EMPTY, EMPTY,   EMPTY, EMPTY, EMPTY, ENDPOINT,
                                   EMPTY,    EMPTY, EMPTY, EMPTY, SEGMENT, EMPTY, EMPTY, EMPTY, EMPTY,
                                   ENDPOINT, EMPTY, EMPTY, EMPTY, EMPTY,   EMPTY, EMPTY};

  direction orientations_connected[5 * 5] = {NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, SOUTH,
                                             NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
                                             NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH};

  game g_empty = game_new_empty();
  game g_default = game_default();
  game g_disconnected = game_new(shapes, orientations);
  game g_connected = game_new(shapes_connected, orientations_connected);
  game g_solution = game_default_solution();

  // Vérification pour le jeu vide
  bool test_empty_game = game_is_connected(g_empty);
  if (!test_empty_game) {
    printf("Erreur : le jeu vide devrait être considéré comme connecté.\n");
  }

  // Vérification pour le jeu par défaut (supposé non connecté)
  bool test_default_game = !game_is_connected(g_default);
  if (!test_default_game) {
    printf("Erreur : le jeu par défaut ne devrait pas être connecté.\n");
  }

  // Vérification pour un jeu configuré comme non connecté
  bool test_disconnected_game = !game_is_connected(g_disconnected);
  if (!test_disconnected_game) {
    printf("Erreur : le jeu ne devrait pas être connecté.\n");
  }
  // Vérification pour un jeu configuré comme connecté
  bool test_connected_game = game_is_connected(g_connected);
  if (!test_connected_game) {
    printf("Erreur : le jeu devrait être connecté.\n");
  }

  // Vérification pour le jeu solution (supposé connecté)
  bool test_solution_game = game_is_connected(g_solution);
  if (!test_solution_game) {
    printf("Erreur : le jeu solution devrait être connecté.\n");
  }

  game_delete(g_empty);
  game_delete(g_default);
  game_delete(g_disconnected);
  game_delete(g_connected);
  game_delete(g_solution);

  return test_empty_game && test_default_game && test_disconnected_game && test_connected_game && test_solution_game;
}

int main(int argc, char* argv[])
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <testname>\n", argv[0]);
    return EXIT_FAILURE;
  }
  if (strcmp(argv[1], "dummy") == 0) {
    test_dummy();
    return EXIT_SUCCESS;

  } else if (strcmp(argv[1], "test_game_default") == 0) {
    if (test_game_default()) {
      printf("test_game_default PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_default FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_default_solution") == 0) {
    if (test_game_default_solution()) {
      printf("test_game_default_solution PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_default_solution FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_get_adjacent_square") == 0) {
    if (test_game_get_adjacent_square()) {
      printf("test_game_get_adjacent_square PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_get_adjacent_square FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_has_half_edge") == 0) {
    if (test_game_has_half_edge()) {
      printf("test_game_has_half_edge PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_has_half_edge FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_check_edge") == 0) {
    if (test_game_check_edge()) {
      printf("test_game_check_edge PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_check_edge FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_is_connected") == 0) {
    if (test_game_is_connected()) {
      printf("test_game_is_connected PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_is_connected FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_is_well_paired") == 0) {
    if (test_game_is_well_paired()) {
      printf("test_game_is_well_paired PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_is_well_paired FAILED\n");
      return EXIT_FAILURE;
    }
  } else {
    fprintf(stderr, "No test named: %s\n", argv[1]);
    return EXIT_FAILURE;
  }
}