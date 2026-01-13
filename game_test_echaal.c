#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue.h"

int test_dummy() { return 0; }

bool test_game_new_empty() {
  game g1 = game_new_empty();
  game g2 = game_new_empty();
  bool test2 = (g1 != NULL);
  bool test3 = (g2 != NULL);

  // Un jeu vide doit être gagnant, bien connecté et bien appairé
  bool test1 = game_won(g1);
  bool test4 = game_is_connected(g1);
  bool test5 = game_is_well_paired(g1);

  bool equal = game_equal(g1, g2, true);

  game_set_piece_orientation(g2, 0, 0, EAST);
  bool are_equal_after_change = !game_equal(g1, g2, false);

  // Vérification que toutes les shapes sont à EMPTY et les orientations à NORTH
  bool test = true;
  for (uint i = 0; i < game_nb_rows(g1); i++) {
    for (uint j = 0; j < game_nb_cols(g1); j++) {
      if (game_get_piece_shape(g1, i, j) != EMPTY ||
          game_get_piece_orientation(g1, i, j) != NORTH) {
        test = false;
      }
    }
  }

  game_delete(g1);
  game_delete(g2);
  return test && test1 && test2 && test3 && test4 && test5 && equal &&
         are_equal_after_change;
}

bool test_game_new() {
  bool test = true;
  shape shapes[5 * 5] = {ENDPOINT, SEGMENT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                         EMPTY,    EMPTY,   EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                         EMPTY,    EMPTY,   EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                         EMPTY,    EMPTY,   EMPTY, EMPTY};

  direction orientations[5 * 5] = {
      NORTH, EAST,  NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
      NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
      NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH};

  game g = game_new(shapes, orientations);
  if (g == NULL) {
    test = false;
  }

  // Vérification que les shapes et orientations correspondent bien aux 2
  // tableaux
  for (uint i = 0; i < 5; i++) {
    for (uint j = 0; j < 5; j++) {
      if (game_get_piece_shape(g, i, j) != shapes[i * 5 + j] ||
          game_get_piece_orientation(g, i, j) != orientations[i * 5 + j]) {
        game_delete(g);
        test = false;
      }
    }
  }
  game g_empty = game_new_empty();
  bool is_equal = game_equal(game_new(NULL, NULL), g_empty, false);
  game_delete(g_empty);
  game_delete(g);
  return test && is_equal;
}

bool test_game_copy() {
  // Test copie du jeu par défaut
  game g_default = game_default();
  game g_copy = game_copy(g_default);
  if (game_nb_cols(g_default) != game_nb_cols(g_copy) ||
      game_nb_rows(g_default) != game_nb_rows(g_copy)) {
    game_delete(g_default);
    game_delete(g_copy);
    return false;
  }

  for (uint i = 0; i < game_nb_rows(g_default); i++) {
    for (uint j = 0; j < game_nb_cols(g_default); j++) {
      if (game_get_piece_orientation(g_copy, i, j) !=
              game_get_piece_orientation(g_default, i, j) ||
          game_get_piece_shape(g_copy, i, j) !=
              game_get_piece_shape(g_default, i, j)) {
        game_delete(g_default);
        game_delete(g_copy);
        return false;
      }
    }
  }

  // Test d'un jeu avec wrapping
  shape shapes[2 * 2] = {SEGMENT, EMPTY, TEE, ENDPOINT};
  direction orientations[2 * 2] = {NORTH, NORTH, EAST, WEST};
  game g_wrapping = game_new_ext(2, 2, shapes, orientations, true);

  game g_wrapping_copy = game_copy(g_wrapping);
  if (!game_is_wrapping(g_wrapping_copy)) {
    return false;
  }

  for (uint i = 0; i < game_nb_rows(g_wrapping); i++) {
    for (uint j = 0; j < game_nb_cols(g_wrapping); j++) {
      if (game_get_piece_orientation(g_wrapping_copy, i, j) !=
              game_get_piece_orientation(g_wrapping, i, j) ||
          game_get_piece_shape(g_wrapping_copy, i, j) !=
              game_get_piece_shape(g_wrapping, i, j)) {
        return false;
      }
    }
  }

  game_delete(g_default);
  game_delete(g_copy);
  game_delete(g_wrapping);
  game_delete(g_wrapping_copy);
  return true;
}

bool test_game_equal() {
  // Fonction pour générer un tableau aléatoire de shapes
  shape *s = malloc(5 * 5 * sizeof(shape));
  if (s == NULL) {
    printf("Échec : Allocation mémoire pour les shapes a échoué.\n");
    return false;
  }
  for (int i = 0; i < 5 * 5; i++) {
    s[i] = rand() % NB_SHAPES;
  }

  // Fonction pour générer un tableau aléatoire de directions
  direction *dir = malloc(5 * 5 * sizeof(direction));
  if (dir == NULL) {
    printf("Échec : Allocation mémoire pour les directions a échoué.\n");
    free(s);  // Libère la mémoire allouée pour s
    return false;
  }
  for (int i = 0; i < 5 * 5; i++) {
    dir[i] = rand() % NB_DIRS;
  }

  // Création des jeux avec les données générées
  game g1 = game_new(s, dir);
  game g3 = game_copy(g1);
  game g2 = game_new(s, dir);
  if (g1 == NULL || g2 == NULL || g3 == NULL) {
    printf("Échec : Création des jeux a échoué.\n");
    free(s);
    free(dir);
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);

    return false;
  }

  bool result = true;

  // Vérification de l'égalité entre les jeux
  if (!game_equal(g1, g3, false) || !game_equal(g1, g2, false) ||
      !game_equal(g2, g3, false)) {
    result = false;
  }

  game_shuffle_orientation(g3);
  if (!game_equal(g1, g3, true) || game_equal(g1, g3, false)) {
    result = false;
  }

  shape shapes4[2 * 2] = {CORNER, CORNER, CORNER, CORNER};
  direction orientations4[2 * 2] = {NORTH, NORTH, NORTH, NORTH};
  game g4 = game_new_ext(2, 2, shapes4, orientations4, false);

  shape shapes5[2 * 2] = {CORNER, CORNER, CORNER, CORNER};
  direction orientations5[2 * 2] = {SOUTH, SOUTH, EAST, WEST};
  game g5 = game_new_ext(2, 2, shapes5, orientations5, false);

  if (!game_equal(g4, g5, true)) {
    result = false;
  }

  free(s);
  free(dir);
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  game_delete(g5);

  return result;
}

bool test_game_delete() {
  game g = game_default();
  game_delete(g);
  return true;
}

bool test_game_set_piece_shape() {
  game g = game_default();
  // On crée une copie du jeu original pour comparaison
  game g_copy = game_copy(g);

  bool result = true;
  game_set_piece_shape(g, 0, 0, EMPTY);
  if (game_get_piece_shape(g, 0, 0) != EMPTY) {
    result = false;
  }

  bool no_change = true;

  // Comparaison des 2 jeux (supposés égaux)
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if ((i != 0 || j != 0) &&
          game_get_piece_shape(g, i, j) != game_get_piece_shape(g_copy, i, j)) {
        no_change = false;
      }
    }
  }

  game_delete(g);
  game_delete(g_copy);

  return result && no_change;
}

bool test_game_set_piece_orientation() {
  game g = game_default();

  // Vérifie les changements pour toutes les directions
  bool all_orientations_correct = true;
  for (direction o = NORTH; o < NB_DIRS; o++) {
    game_set_piece_orientation(g, 0, 0, o);
    if (game_get_piece_orientation(g, 0, 0) != o) {
      all_orientations_correct = false;
      break;
    }
  }

  // Vérifie le bon fonctionnement de la fonction
  game_set_piece_orientation(g, 1, 0, EAST);

  bool orientation_changed = true;
  if (game_get_piece_orientation(g, 1, 0) != EAST) {
    orientation_changed = false;
  }

  game_delete(g);
  return all_orientations_correct && orientation_changed;
}

bool test_game_new_empty_ext() {
  game g1 = game_new_empty_ext(5, 4, true);
  game g2 = game_new_empty_ext(6, 8, true);
  bool test2 = (g1 != NULL);
  bool test3 = (g2 != NULL);

  // Un jeu vide doit être gagnant, bien connecté et bien appairé
  bool test1 = game_won(g1);
  bool test4 = game_is_connected(g1);
  bool test5 = game_is_well_paired(g1);

  game_set_piece_orientation(g2, 0, 0, EAST);
  bool are_equal_after_change = !game_equal(g1, g2, false);

  // Vérification que toutes les shapes sont à EMPTY et les orientations à NORTH
  bool test = true;
  for (uint i = 0; i < game_nb_rows(g1); i++) {
    for (uint j = 0; j < game_nb_cols(g1); j++) {
      if (game_get_piece_shape(g1, i, j) != EMPTY ||
          game_get_piece_orientation(g1, i, j) != NORTH) {
        test = false;
      }
    }
  }

  game_delete(g1);
  game_delete(g2);
  return test && test1 && test2 && test3 && test4 && test5 &&
         are_equal_after_change;
}

bool test_game_new_ext() {
  bool test = true;

  int nb_rows = 3;
  int nb_cols = 3;
  bool wrapping = true;

  shape shapes[3 * 3] = {ENDPOINT, SEGMENT, EMPTY, EMPTY, EMPTY,
                         EMPTY,    EMPTY,   EMPTY, EMPTY};
  direction orientations[3 * 3] = {NORTH, EAST,  NORTH, NORTH, NORTH,
                                   NORTH, NORTH, NORTH, NORTH};

  game g = game_new_ext(nb_rows, nb_cols, shapes, orientations, wrapping);
  if (g == NULL) {
    printf("Erreur : jeu non créé avec game_new_ext.\n");
    return false;
  }

  // Vérification que les shapes et orientations correspondent bien aux 2
  // tableaux
  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      if (game_get_piece_shape(g, i, j) != shapes[i * nb_cols + j] ||
          game_get_piece_orientation(g, i, j) !=
              orientations[i * nb_cols + j]) {
        printf("Erreur : mismatch pour la pièce (%u, %u).\n", i, j);
        test = false;
      }
    }
  }

  if (game_nb_rows(g) != nb_rows || game_nb_cols(g) != nb_cols) {
    printf("Erreur : dimensions incorrectes (nb_rows=%u, nb_cols=%u).\n",
           game_nb_rows(g), game_nb_cols(g));
    test = false;
  }

  if (game_is_wrapping(g) != wrapping) {
    printf("Erreur : wrapping incorrect (attendu=%d, trouvé=%d).\n", wrapping,
           game_is_wrapping(g));
    test = false;
  }

  game_delete(g);
  return test;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <testname>\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (strcmp(argv[1], "dummy") == 0) {
    if (test_dummy() == EXIT_SUCCESS) {
      printf("dummy test PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("dummy test FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_new_empty") == 0) {
    if (test_game_new_empty()) {
      printf("test_game_new_empty PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_new_empty FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_new_empty_ext") == 0) {
    if (test_game_new_empty_ext()) {
      printf("test_game_new_empty_ext PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_new_empty_ext FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_new") == 0) {
    if (test_game_new()) {
      printf("test_game_new PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_new FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_new_ext") == 0) {
    if (test_game_new_ext()) {
      printf("test_game_new_ext PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_new_ext FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_copy") == 0) {
    if (test_game_copy()) {
      printf("test_game_copy PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_copy FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_equal") == 0) {
    if (test_game_equal()) {
      printf("test_game_equal PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_equal FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_delete") == 0) {
    if (test_game_delete()) {
      printf("test_game_delete PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_delete FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_set_piece_shape") == 0) {
    if (test_game_set_piece_shape()) {
      printf("test_game_set_piece_shape PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_set_piece_shape FAILED\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_set_piece_orientation") == 0) {
    if (test_game_set_piece_orientation()) {
      printf("test_game_set_piece_orientation PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_set_piece_orientation FAILED\n");
      return EXIT_FAILURE;
    }
  } else {
    fprintf(stderr, "Unknown test: %s\n", argv[1]);
    return EXIT_FAILURE;
  }
}