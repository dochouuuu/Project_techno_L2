#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"
#include "queue.h"

int test_dummy() { return 0; }

bool test_game_default(void) {
  game g1 = game_default();
  game g2 = game_default();

  // Vérifie que les cases ne sont pas vides et que toutes les orientations sont
  // valides
  bool all_filled = true;
  bool all_orientations_valid = true;

  for (uint i = 0; i < game_nb_rows(g1); i++) {
    for (uint j = 0; j < game_nb_cols(g1); j++) {
      if (game_get_piece_shape(g1, i, j) == EMPTY) {
        all_filled = false;
        printf("Case vide trouvée à (%u, %u)\n", i, j);
      }
      direction orientation = game_get_piece_orientation(g1, i, j);
      if (orientation < 0 || orientation > 3) {
        all_orientations_valid = false;
        printf("Orientation invalide à (%u, %u), trouvée: %d\n", i, j,
               orientation);
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

bool test_game_default_solution(void) {
  game g_default = game_default();

  game g_solution = game_default_solution();

  game g_solution2 = game_default_solution();

  bool are_equal = !game_equal(g_default, g_solution, false);
  bool is_game_won =
      game_won(g_solution);  // Vérifie si la solution est gagnante

  // Vérifie si la solution du jeu par défault est toujours la même
  bool are_same = game_equal(g_solution, g_solution2, false);

  game_delete(g_default);
  game_delete(g_solution);
  game_delete(g_solution2);

  // Retourne vrai si les jeux sont différents et si la solution est gagnante
  return are_equal && is_game_won && are_same;
}

bool test_game_get_adjacent_square(void) {
  game g = game_default();

  unsigned int i_next = 0;
  unsigned int j_next = 0;

  bool is_outside1 = !game_get_ajacent_square(g, 0, 0, NORTH, &i_next, &j_next);
  bool is_outside2 = !game_get_ajacent_square(g, 0, 0, WEST, &i_next, &j_next);
  bool test1 = (i_next == 0 && j_next == 0);

  bool is_inside = game_get_ajacent_square(g, 1, 1, EAST, &i_next, &j_next);
  bool test = (i_next == 1 && j_next == 2);

  // Test avec wrapping
  shape shapes2[2 * 2] = {SEGMENT, EMPTY, TEE, ENDPOINT};

  direction orientations2[2 * 2] = {NORTH, NORTH, EAST, WEST};

  game g2 = game_new_ext(2, 2, shapes2, orientations2, true);

  bool get_aja_wrapping1 =
      game_get_ajacent_square(g2, 0, 0, NORTH, &i_next, &j_next);
  bool get_aja_wrapping2 =
      game_get_ajacent_square(g2, 1, 0, SOUTH, &i_next, &j_next);

  game_delete(g);
  game_delete(g2);
  return test && test1 && is_outside1 && is_outside2 && is_inside &&
         get_aja_wrapping1 && get_aja_wrapping2;
}

bool test_game_has_half_edge(void) {
  shape shapes[5 * 5] = {
      ENDPOINT, SEGMENT, ENDPOINT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
      EMPTY,    EMPTY,   EMPTY,    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
      EMPTY,    EMPTY,   EMPTY,    EMPTY, EMPTY, EMPTY, EMPTY};

  direction orientations[5 * 5] = {
      EAST,  EAST,  WEST,  NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
      NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
      NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH};

  game g = game_new(shapes, orientations);

  bool result = true;
  // Test sur une grille conue pour détecter les erreurs de fonctionnement
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      for (direction d = NORTH; d < NB_DIRS; d++) {
        bool has_half_edge = game_has_half_edge(g, i, j, d);

        if (i == 0 && j == 0 && game_get_piece_shape(g, i, j) == ENDPOINT &&
            d == EAST) {
          if (!has_half_edge) {
            printf(
                "Erreur : ENDPOINT à (%u, %u) devrait avoir une demi-arête "
                "dans la direction %d.\n",
                i, j, d);
            result = false;
          }
        } else if (i == 1 && j == 0 &&
                   game_get_piece_shape(g, i, j) == SEGMENT &&
                   (d == EAST || d == WEST)) {
          if (!has_half_edge) {
            printf(
                "Erreur : SEGMENT à (%u, %u) devrait avoir une demi-arête dans "
                "la direction %d.\n",
                i, j, d);
            result = false;
          }
        } else if (i == 2 && j == 0 &&
                   game_get_piece_shape(g, i, j) == ENDPOINT && d == WEST) {
          if (!has_half_edge) {
            printf(
                "Erreur : ENDPOINT à (%u, %u) devrait avoir une demi-arête "
                "dans "
                "la direction %d.\n",
                i, j, d);
            result = false;
          }
        }
      }
    }
  }

  // Test sur jeu vide
  game g_empty = game_new_empty();

  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
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

  // Test shape CROSS
  shape shapes2[3 * 3] = {EMPTY,    ENDPOINT, EMPTY,    ENDPOINT, CROSS,
                          ENDPOINT, EMPTY,    ENDPOINT, EMPTY};

  direction orientations2[3 * 3] = {NORTH, SOUTH, NORTH, EAST, NORTH,
                                    WEST,  NORTH, NORTH, NORTH};

  game g2 = game_new_ext(3, 3, shapes2, orientations2, false);

  bool has_half_edge1 = game_has_half_edge(g2, 0, 1, SOUTH);
  has_half_edge1 = game_has_half_edge(g2, 1, 0, EAST);
  has_half_edge1 = game_has_half_edge(g2, 1, 2, WEST);
  has_half_edge1 = game_has_half_edge(g2, 2, 1, NORTH);

  has_half_edge1 = game_has_half_edge(g2, 1, 1, NORTH);
  has_half_edge1 = game_has_half_edge(g2, 1, 1, SOUTH);
  has_half_edge1 = game_has_half_edge(g2, 1, 1, EAST);
  has_half_edge1 = game_has_half_edge(g2, 1, 1, WEST);

  game_delete(g2);
  game_delete(g_empty);
  game_delete(g);

  return result && has_half_edge1;
}

bool test_game_check_edge() {
  // Test sur une grille connue : game default
  game g = game_default();
  bool all_tests_passed = true;

  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      for (direction d = NORTH; d < NB_DIRS; d++) {
        edge_status status = game_check_edge(g, i, j, d);
        bool has_half_edge = game_has_half_edge(g, i, j, d);

        if ((status == MATCH && has_half_edge == false) ||
            (status == NOEDGE && has_half_edge == true)) {
          printf("Erreur : à (%u, %u), direction %d, bool : %d\n", i, j, d,
                 has_half_edge);
          all_tests_passed = false;
        }
      }
    }
  }

  // Test avec wrapping + shape CROSS
  shape shapes1[3 * 2] = {CORNER, ENDPOINT, CROSS, SEGMENT, ENDPOINT, EMPTY};

  direction orientations1[3 * 2] = {EAST, WEST, NORTH, EAST, NORTH, NORTH};

  game g2 = game_new_ext(3, 2, shapes1, orientations1, true);
  edge_status status = game_check_edge(g, 1, 1, EAST);
  bool has_half_edge = game_has_half_edge(g, 1, 1, EAST);
  if ((status == MATCH && has_half_edge == false) ||
      (status == NOEDGE && has_half_edge == true)) {
    printf("Erreur : à (%u, %u), direction EAST, bool : %d\n", 1, 1,
           has_half_edge);
    all_tests_passed = false;
  }
  game_print(g2);
  game_delete(g);
  game_delete(g2);
  return all_tests_passed;
}

bool test_game_is_well_paired(void) {
  // Tests sur des grilles connues : game default, game default mosifiée, jeu
  // vide
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

  // Test sur une grille avec une erreur en bas à droite
  shape shapes[5 * 5] = {EMPTY,    EMPTY,   EMPTY,   EMPTY,    ENDPOINT,
                         EMPTY,    EMPTY,   EMPTY,   ENDPOINT, SEGMENT,
                         EMPTY,    EMPTY,   EMPTY,   SEGMENT,  SEGMENT,
                         EMPTY,    EMPTY,   EMPTY,   ENDPOINT, SEGMENT,
                         ENDPOINT, SEGMENT, SEGMENT, SEGMENT,  TEE};

  direction orientations[5 * 5] = {
      NORTH, NORTH, NORTH, NORTH, SOUTH, NORTH, NORTH, NORTH, SOUTH,
      NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
      NORTH, NORTH, EAST,  EAST,  EAST,  EAST,  WEST};
  game g_new = game_new(shapes, orientations);
  game_print(g_new);
  bool test = !game_is_well_paired(g_new);

  // Test avec wrapping (supposé bon)
  shape shapes1[3 * 3] = {ENDPOINT, EMPTY,    EMPTY, TEE,  ENDPOINT,
                          SEGMENT,  ENDPOINT, EMPTY, EMPTY};
  direction orientations1[3 * 3] = {SOUTH, NORTH, NORTH, WEST, EAST,
                                    EAST,  NORTH, NORTH, NORTH};
  game g = game_new_ext(3, 3, shapes1, orientations1, true);
  test = game_is_well_paired(g);

  // Test avec une erreur
  shape shapes2[5 * 5] = {CORNER,   SEGMENT,  TEE,      SEGMENT,  TEE,
                          ENDPOINT, TEE,      TEE,      SEGMENT,  ENDPOINT,
                          ENDPOINT, TEE,      TEE,      TEE,      ENDPOINT,
                          SEGMENT,  ENDPOINT, ENDPOINT, SEGMENT,  CORNER,
                          ENDPOINT, ENDPOINT, SEGMENT,  ENDPOINT, SEGMENT};
  direction orientations2[5 * 5] = {
      WEST,  NORTH, EAST,  WEST,  NORTH, EAST,  WEST, EAST, WEST,
      WEST,  EAST,  NORTH, NORTH, SOUTH, WEST,  WEST, WEST, SOUTH,
      NORTH, EAST,  SOUTH, SOUTH, NORTH, NORTH, NORTH};
  game g_wrapping = game_new_ext(5, 5, shapes2, orientations2, false);
  test = !game_is_well_paired(g_wrapping);

  // Test avec une boucle et du wrapping (supposé bon)
  shape shape3[6 * 4] = {TEE,    TEE,     TEE,     TEE,     TEE,     TEE,
                         CORNER, SEGMENT, SEGMENT, SEGMENT, SEGMENT, CORNER,
                         CORNER, SEGMENT, SEGMENT, SEGMENT, SEGMENT, CORNER,
                         TEE,    TEE,     TEE,     TEE,     TEE,     TEE};

  direction orientations3[6 * 4] = {
      NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, EAST,  EAST,
      EAST,  EAST,  EAST,  SOUTH, NORTH, EAST,  EAST,  EAST,
      EAST,  WEST,  SOUTH, SOUTH, SOUTH, SOUTH, SOUTH, SOUTH,
  };
  game g3 = game_new_ext(4, 6, shape3, orientations3, true);
  test = game_is_well_paired(g3);
  game_print(g3);
  game_play_move(g3, 0, 0, EAST);
  test = !game_is_well_paired(g3);

  game_delete(g_default);
  game_delete(g_modified_solution);
  game_delete(g_empty);
  game_delete(g_wrapping);
  game_delete(g_new);
  game_delete(g);
  game_delete(g3);

  return is_paired_default && !is_not_paired && is_paired_empty && test;
}

bool test_game_is_connected(void) {
  shape shapes[5 * 5] = {EMPTY,    EMPTY,   EMPTY,    EMPTY,    EMPTY,
                         EMPTY,    EMPTY,   EMPTY,    ENDPOINT, EMPTY,
                         EMPTY,    EMPTY,   EMPTY,    SEGMENT,  EMPTY,
                         EMPTY,    EMPTY,   EMPTY,    ENDPOINT, EMPTY,
                         ENDPOINT, SEGMENT, ENDPOINT, EMPTY,    EMPTY};

  direction orientations[5 * 5] = {
      NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, SOUTH,
      NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
      NORTH, NORTH, EAST,  EAST,  WEST,  NORTH, NORTH};
  shape shapes_connected[5 * 5] = {
      EMPTY,    EMPTY, EMPTY, EMPTY, EMPTY,   EMPTY, EMPTY, EMPTY, ENDPOINT,
      EMPTY,    EMPTY, EMPTY, EMPTY, SEGMENT, EMPTY, EMPTY, EMPTY, EMPTY,
      ENDPOINT, EMPTY, EMPTY, EMPTY, EMPTY,   EMPTY, EMPTY};

  direction orientations_connected[5 * 5] = {
      NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, SOUTH,
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

  // Test avec une erreur
  shape shapes1[2 * 3] = {CORNER, CORNER, SEGMENT, SEGMENT, CORNER, CORNER};
  direction orientations1[2 * 3] = {EAST, SOUTH, NORTH, NORTH, NORTH, WEST};
  game g_new = game_new(shapes1, orientations1);

  bool test_g_new = !game_is_connected(g_new);

  // Test wrapping
  shape shapes2[2 * 2] = {SEGMENT, EMPTY, TEE, ENDPOINT};
  direction orientations2[2 * 2] = {NORTH, NORTH, EAST, WEST};
  game g_wrapping = game_new_ext(2, 2, shapes2, orientations2, true);

  bool test_g_wrapping = game_is_connected(g_wrapping);

  game_delete(g_empty);
  game_delete(g_default);
  game_delete(g_disconnected);
  game_delete(g_connected);
  game_delete(g_solution);
  game_delete(g_new);
  game_delete(g_wrapping);

  return test_empty_game && test_default_game && test_disconnected_game &&
         test_connected_game && test_solution_game && test_g_new &&
         test_g_wrapping;
}

bool test_game_undo(void) {
  // On créé 2 jeux par défault, on en modifie un et on regarde s'ils sont égaux
  // après annulation de l'action
  game g = game_default();
  game g_default = game_default();
  game_play_move(g, 1, 1, 1);
  game_undo(g);

  bool equal = game_equal(g, g_default, false);
  game_delete(g);
  game_delete(g_default);
  return equal;
}

bool test_game_redo(void) {
  // On créé un jeu par défault, on le modifie puis on le copie et on regarde si
  // après un undo et redo il est égal à sa version innitiale
  game g = game_default();
  game_play_move(g, 1, 1, 1);
  game g_after_play = game_copy(g);

  game_undo(g);
  game_redo(g);

  bool equal = game_equal(g, g_after_play, false);

  game_delete(g);
  game_delete(g_after_play);

  return equal;
}

bool test_game_random(void) {
  // Test de la fonction game_random
  uint nb_rows = 3;
  uint nb_cols = 3;
  bool wrapping = false;
  uint nb_empty = 0;
  uint nb_extra = 1;

  game g = game_random(nb_rows, nb_cols, wrapping, nb_empty, nb_extra);
  assert(g != NULL);  // Vérifie que le jeu a été créé

  // Vérifie que le nombre de pièces est correct
  uint expected_pieces = nb_rows * nb_cols - nb_empty;
  uint actual_pieces = 0;
  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      if (game_get_piece_shape(g, i, j) != EMPTY) {
        actual_pieces++;
      }
    }
  }
  assert(actual_pieces == expected_pieces);

  // Libération de la mémoire
  game_delete(g);
  return true;
}

bool test_game_solve_nb_solutions(void) {
  // Test avec le jeu game11, sans wrapping
  game g = game_load("../default.txt");
  game_shuffle_orientation(g);
  printf("Initial game state:\n");
  game_print(g);

  printf("\nFinding  1 solution...\n");
  bool solution = game_solve(g);
  if (solution) {
    printf("Solution found : \n");
    game_print(g);
  } else {
    printf("No solution found.\n");
  }

  printf("\nFinding all solutions...\n");
  uint nb = game_nb_solutions(g);
  printf("Number of solutions: %u\n", nb);

  // Test avec une grille rectangulaire et wrapping
  game g1 = game_random(4, 2, 1, 0, 0);
  game_shuffle_orientation(g1);
  printf("Initial game state:\n");
  game_print(g1);

  printf("\nFinding  1 solution...\n");
  solution = game_solve(g1);
  if (solution) {
    printf("Solution found : \n");
    game_print(g1);
  } else {
    printf("No solution found.\n");
  }

  printf("\nFinding all solutions...\n");
  uint nb1 = game_nb_solutions(g1);
  printf("Number of solutions: %u\n", nb1);

  // Test jeu sans solution
  shape shapes[2 * 2] = {
      ENDPOINT,
      CORNER,
      CORNER,
      CORNER,
  };
  direction orientations[2 * 2] = {NORTH, SOUTH, NORTH, EAST};
  game g2 = game_new_ext(2, 2, shapes, orientations, false);
  printf("Initial game state:\n");
  game_print(g2);

  printf("\nFinding  1 solution...\n");
  solution = game_solve(g2);
  if (solution) {
    printf("Solution found but it should not be the case.\n");
    game_print(g2);
  } else {
    printf("No solution found as expected.\n");
  }
  printf("\nFinding all solutions...\n");
  uint nb2 = game_nb_solutions(g2);
  printf("Number of solutions: %u\n", nb2);
  game_print(g2);

  game_delete(g);
  game_delete(g1);
  game_delete(g2);

  return !solution;
}

bool test_game_solve(void) {
  game g = game_load("../niko.txt");
  game_print(g);
  bool solved = game_solve(g);
  if (solved) {
    printf("g ok\n");
  }
  game_delete(g);

  game g1 = game_load("../game_failed1.txt");
  game_print(g1);
  bool solved1 = game_solve(g1);
  if (solved1) {
    printf("g1 ok\n");
  }
  game_delete(g1);

  return solved && solved1;
}

int main(int argc, char *argv[]) {
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
  } else if (strcmp(argv[1], "test_game_undo") == 0) {
    if (test_game_undo()) {
      printf("test_game_undo PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_undo FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_redo") == 0) {
    if (test_game_redo()) {
      printf("test_game_redo PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_redo FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_random") == 0) {
    if (test_game_random()) {
      printf("test_game_random PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_random FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_solve_nb_solutions") == 0) {
    if (test_game_solve_nb_solutions()) {
      printf("test_game_solve_nb_solutions PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_solve_nb_solutions FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_solve") == 0) {
    if (test_game_solve()) {
      printf("test_game_solve PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_solve FAILED\n");
      return EXIT_FAILURE;
    }
  } else {
    fprintf(stderr, "No test named: %s\n", argv[1]);
    return EXIT_FAILURE;
  }
}