#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"
#include "queue.h"

int test_dummy() { return EXIT_SUCCESS; }

bool test_game_get_piece_shape(void) {
  game g = game_default();

  // Vérifier que chaque case a un état initial non modifié
  bool initial_test = true;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      shape initial_shape = game_get_piece_shape(g, i, j);
      if (initial_shape != game_get_piece_shape(g, i, j)) {
        initial_test = false;  // Détecte toute incohérence
      }
    }
  }

  // Définir une forme spécifique pour (0,0) et vérifier la modification
  game_set_piece_shape(g, 0, 0, CROSS);
  bool test1 = (game_get_piece_shape(g, 0, 0) == CROSS);

  // Modifier une autre case et vérifier le changement
  game_set_piece_shape(g, 4, 4, ENDPOINT);
  bool test2 = (game_get_piece_shape(g, 4, 4) == ENDPOINT);

  // Modifier la première case avec une autre forme
  game_set_piece_shape(g, 0, 0, SEGMENT);
  bool test3 = (game_get_piece_shape(g, 0, 0) == SEGMENT);

  game_delete(g);

  return initial_test && test1 && test2 && test3;
}

bool test_game_get_piece_orientation(void) {
  game g = game_default();

  // Définir l'orientation de certaines pièces
  game_set_piece_orientation(g, 0, 0, EAST);
  game_set_piece_orientation(g, 2, 2, SOUTH);

  bool test1 = (game_get_piece_orientation(g, 0, 0) == EAST);
  bool test2 = (game_get_piece_orientation(g, 2, 2) == SOUTH);

  game_delete(g);
  return test1 && test2;
}

bool test_game_play_move(void) {
  game g1 = game_default();
  assert(g1 != NULL);

  // Créer une copie du jeu
  cgame g = g1;
  game g2 = game_copy(g);

  // Effectuer des rotations sur toutes les pièces
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      game_play_move(g2, i, j, 2);
      game_play_move(g2, i, j, -2);
    }
  }

  // Comparer les 2 jeux après les rotations
  cgame g0 = g2;
  bool test = game_equal(g, g0, false);

  game_delete(g1);
  game_delete(g2);

  return test;
}

bool test_game_won() {
  // Tester si le jeu vide est gagné par défaut (le graphe vide est connexe)
  game g = game_new_empty();
  if (!game_won(g)) {
    game_delete(g);
    return false;
  }

  // Tester si le jeu par défaut n'est pas gagné, la solution par défaut est
  // gagnée
  if (game_won(game_default()) || !game_won(game_default_solution())) {
    game_delete(g);
    return false;
  }

  // Tester si un jeu avec 1 seule case non-vide ne peut pas être gagné (car pas
  // bien appairé)
  game_set_piece_shape(g, 2, 2, CORNER);
  if (game_won(g)) {
    game_delete(g);
    return false;
  }

  // Tester si un jeu avec un petit ilot appairé et tout le reste en empty est
  // gagnant
  game_set_piece_shape(g, 2, 3, CORNER);
  game_set_piece_orientation(g, 2, 3, WEST);
  game_set_piece_shape(g, 1, 2, CORNER);
  game_set_piece_orientation(g, 1, 2, EAST);
  game_set_piece_shape(g, 1, 3, CORNER);
  game_set_piece_orientation(g, 1, 3, SOUTH);

  if (!game_won(g)) {
    game_delete(g);
    return false;
  }

  // Tester si juste un ilot pas bien appairé, donc pas gagnant
  game_set_piece_shape(g, 1, 3, TEE);
  if (game_won(g)) {
    game_delete(g);
    return false;
  }

  // Tester si un ilot bien appairé, un ilot mal appairé
  game_set_piece_shape(g, 1, 3, CORNER);
  game_set_piece_shape(g, 3, 0, SEGMENT);
  game_set_piece_shape(g, 2, 0, ENDPOINT);

  if (game_won(g)) {
    game_delete(g);
    return false;
  }

  // Tester si deux sous-ilots connexes bien appairés, donc pas de victoire
  game_set_piece_shape(g, 4, 0, ENDPOINT);
  game_set_piece_orientation(g, 2, 0, SOUTH);

  if (game_won(g)) {
    game_delete(g);
    return false;
  }

  game_delete(g);

  return true;
}

bool test_game_reset_orientation(void) {
  game g = game_default();

  // Change les orientations pour tester la réinitialisation
  game_play_move(g, 0, 0, 1);
  game_play_move(g, 1, 1, 2);

  game_reset_orientation(g);

  // Vérifie si toutes les pièces sont bien orientées au nord
  bool all_north = true;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_piece_orientation(g, i, j) != NORTH) {
        all_north = false;
      }
    }
  }

  game_delete(g);
  return all_north;
}

bool test_game_shuffle_orientation(void) {
  game g = game_default();

  // Définir des orientations spécifiques pour avoir un point de départ connu
  game_set_piece_orientation(g, 0, 0, NORTH);
  game_set_piece_orientation(g, 1, 1, EAST);
  game_set_piece_orientation(g, 2, 2, SOUTH);

  bool shuffled_00 = false, shuffled_11 = false, shuffled_22 = false;
  int attempts = 5;

  // Essayer plusieurs fois pour s'assurer que les orientations changent
  for (int i = 0; i < attempts; i++) {
    game_shuffle_orientation(g);
    shuffled_00 = shuffled_00 || (game_get_piece_orientation(g, 0, 0) != NORTH);
    shuffled_11 = shuffled_11 || (game_get_piece_orientation(g, 1, 1) != EAST);
    shuffled_22 = shuffled_22 || (game_get_piece_orientation(g, 2, 2) != SOUTH);
  }

  game_delete(g);
  return shuffled_00 && shuffled_11 && shuffled_22;
  // S'assurer que toutes les pièces ont changé au moins une fois
}

bool test_game_print(void) {
  game g = game_default();
  game g2 = game_default_solution();

  game_print(g);
  game_print(g2);

  game_delete(g);
  game_delete(g2);

  return true;
}

bool test_game_nb_rows_and_nb_cols(void) {
  game g = game_new_empty_ext(5, 10, false);
  if (game_nb_rows(g) != 5) {
    printf("expected 5, got %d\n", game_nb_rows(g));
    game_delete(g);
    return false;
  }

  if (game_nb_cols(g) != 10) {
    printf("expected 10, got %d\n", game_nb_cols(g));
    game_delete(g);
    return false;
  }

  game_delete(g);
  return true;
}

bool test_game_is_wrapping() {
  game g1 = game_new_empty_ext(5, 10, true);  // Grille avec wrapping
  if (!game_is_wrapping(g1)) {
    printf("game_is_wrapping failed for wrapping = true\n");
    game_delete(g1);
    return false;
  }

  game g2 = game_new_empty_ext(5, 10, false);  // Grille sans wrapping
  if (game_is_wrapping(g2)) {
    printf("game_is_wrapping failed for wrapping = false\n");
    game_delete(g2);
    return false;
  }

  game_delete(g1);
  game_delete(g2);
  return true;
}

bool test_game_save_load(void) {
  bool test = true;

  // Sauvegarder et recharger le jeu par défaut
  game g1 = game_default();
  game_save(g1, "default.txt");

  // Charger le jeu sauvegardé
  game g2 = game_load("default.txt");
  if (g2 == NULL || !game_equal(g1, g2, false)) {
    test = false;
  }

  game_delete(g1);
  game_delete(g2);

  // Sauvegarder et recharger un jeu vide
  game g3 = game_new_empty();
  game_save(g3, "empty.txt");

  game g4 = game_load("empty.txt");
  if (!game_equal(g3, g4, false)) {
    test = false;
  }

  game_delete(g3);
  game_delete(g4);

  // Sauvegarder et recharger un jeu avec wrapping activé
  shape shapes[6 * 4] = {TEE,    TEE,     TEE,     TEE,     TEE,     TEE,
                         CORNER, SEGMENT, SEGMENT, SEGMENT, SEGMENT, CORNER,
                         CORNER, SEGMENT, SEGMENT, SEGMENT, SEGMENT, CORNER,
                         TEE,    TEE,     TEE,     TEE,     TEE,     TEE};

  direction orientations[6 * 4] = {NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
                                   EAST,  EAST,  EAST,  EAST,  EAST,  SOUTH,
                                   NORTH, EAST,  EAST,  EAST,  EAST,  WEST,
                                   SOUTH, SOUTH, SOUTH, SOUTH, SOUTH, SOUTH};

  game g5 = game_new_ext(4, 6, shapes, orientations, true);
  game_print(g5);
  game_save(g5, "testwrapping.txt");

  game g6 = game_load("testwrapping.txt");
  if (!game_equal(g5, g6, false)) {
    test = false;
  }

  game_delete(g5);
  game_delete(g6);

  return test;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <testname>\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (strcmp(argv[1], "dummy") == 0) {
    test_dummy();
    return EXIT_SUCCESS;
  }

  else if (strcmp(argv[1], "test_game_get_piece_shape") == 0) {
    if (test_game_get_piece_shape()) {
      printf("Test game_get_piece_shape PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("\n**Test game_get_piece_shape FAILED**\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_get_piece_orientation") == 0) {
    if (test_game_get_piece_orientation()) {
      printf("Test game_get_piece_orientation PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("\n**Test game_get_piece_orientation FAILED**\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_play_move") == 0) {
    if (test_game_play_move()) {
      printf("Test game_play_move PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("\n**Test game_play_move FAILED**\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_won") == 0) {
    if (test_game_won()) {
      printf("Test game_won PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("\n**Test game_won FAILED**\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_reset_orientation") == 0) {
    if (test_game_reset_orientation()) {
      printf("Test game_reset_orientation PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("\n**Test game_reset_orientation FAILED**\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_shuffle_orientation") == 0) {
    if (test_game_shuffle_orientation()) {
      printf("Test game_shuffle_orientation PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("\n**Test game_shuffle_orientation FAILED**\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_print") == 0) {
    if (test_game_print()) {
      printf("Test game_print PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("\n**Test game_print FAILED**\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_nb_rows_and_nb_cols") == 0) {
    if (test_game_nb_rows_and_nb_cols()) {
      printf("Test game_nb_rows_and_nb_cols PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("\n**Test game_nb_rows_and_nb_cols FAILED**\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_is_wrapping") == 0) {
    if (test_game_is_wrapping()) {
      printf("Test game_is_wrapping PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("\n**Test game_is_wrapping FAILED**\n");
      return EXIT_FAILURE;
    }
  } else if (strcmp(argv[1], "test_game_save_load") == 0) {
    if (test_game_save_load()) {
      printf("Test test_game_save_load PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("\n**Test test_game_save_load FAILED**\n");
      return EXIT_FAILURE;
    }
  } else {
    fprintf(stderr, "Unknown test: %s\n", argv[1]);
    return EXIT_FAILURE;
  }
}