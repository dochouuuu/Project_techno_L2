#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"

int test_dummy() { return 0; }

/*Ce que fait la fonction : Crée un jeu vide de taille par défaut
(DEFAULT_SIZE), avec toutes les pièces initialisées à la forme EMPTY et
orientation NORTH. Tests à implémenter : Vérifie que le jeu est bien créé (le
pointeur n'est pas NULL). Vérifie que la taille du jeu est bien DEFAULT_SIZE.
Vérifie que toutes les pièces sont bien de forme EMPTY et orientées vers
NORTH.*/

bool test_game_new_empty()
{
  game g1 = game_new_empty();
  game g2 = game_new_empty();
  bool test2 = (g1 != NULL);
  bool test3 = (g2 != NULL);

  bool test1 = game_won(g1);
  bool test4 = game_is_connected(g1);
  bool test5 = game_is_well_paired(g1);

  bool equal = game_equal(g1, g2, true);

  game_set_piece_orientation(g2, 0, 0, EAST);
  bool are_equal_after_change = !game_equal(g1, g2, false);

  bool test = true;
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_piece_shape(g1, i, j) != EMPTY || game_get_piece_orientation(g1, i, j) != NORTH) {
        test = false;
      }
    }
  }

  game_delete(g1);
  game_delete(g2);
  return test && test1 && test2 && test3 && test4 && test5 && equal && are_equal_after_change;
}

/*
 * @brief Creates a new game with default size and initializes it.
 * @param shapes an array describing the piece shape in each square
 * (or NULL to set all shapes to empty)
 * @param orientations an array describing the orientation of piece in each
 * square (or NULL to set all orientations to north)
 * @details Both the arrays for shapes and orientations are 1D arrays of size
 * DEFAULT_SIZE squared, using row-major storage convention.
 * @pre @p shapes must be an initialized array of DEFAULT_SIZE squared or NULL.
 * @pre @p orientations must be an initialized array of DEFAULT_SIZE squared or
 * NULL.
 * @return the created game*/
/* Crée un jeu avec les formes et orientations spécifiées. Si les tableaux
shapes ou orientations sont NULL, il initialise les pièces respectivement à
EMPTY et NORTH. Tests à implémenter : Teste avec des tableaux non NULL pour les
formes et orientations, et vérifie que le jeu est bien initialisé avec les
valeurs données. Teste avec des tableaux NULL, et vérifie que toutes les formes
sont EMPTY et que toutes les orientations sont NORTH. Vérifie que la taille du
jeu est correcte (DEFAULT_SIZE).*/

bool test_game_new()
{
  bool test = true;
  shape shapes[DEFAULT_SIZE * DEFAULT_SIZE] = {ENDPOINT, SEGMENT, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                               EMPTY,    EMPTY,   EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                                               EMPTY,    EMPTY,   EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};

  direction orientations[DEFAULT_SIZE * DEFAULT_SIZE] = {NORTH, EAST,  NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
                                                         NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH,
                                                         NORTH, NORTH, NORTH, NORTH, NORTH, NORTH, NORTH};

  game g = game_new(shapes, orientations);
  if (g == NULL) {
    test = false;
  }

  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_piece_shape(g, i, j) != shapes[i * DEFAULT_SIZE + j] ||
          game_get_piece_orientation(g, i, j) != orientations[i * DEFAULT_SIZE + j]) {
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

/* @brief Duplicates a game.
 * @param g the game to copy
 * @return the copy of the game
 * @pre @p g must be a valid pointer toward a game structure.
 **/
/*Crée une copie d’un jeu existant.
Tests à implémenter :
Crée un jeu, fais-en une copie et vérifie que toutes les pièces de la copie
correspondent à celles du jeu original. Modifie une pièce dans le jeu original
et vérifie que la copie n'est pas affectée (vérifie que la copie est bien une
nouvelle instance et non une référence partagée).*/

bool test_game_copy()
{
  game g1 = game_new_empty();
  game_set_piece_shape(g1, 0, 0, ENDPOINT);
  game g2 = game_copy(g1);
  bool result = game_equal(g1, g2, false);
  game_delete(g1);
  game_delete(g2);
  return result;
}

/**
 * @brief Tests if two games are equal.
 * @param g1 the first game
 * @param g2 the second game
 * @param ignore_orientation if true, the orientation of pieces is ignored
 * @return true if the two games are equal, false otherwise
 * @pre @p g1 must be a valid pointer toward a game structure.
 * @pre @p g2 must be a valid pointer toward a game structure.
 **/
/*Ce que fait la fonction : Compare deux jeux pour voir s’ils sont égaux, avec
une option pour ignorer l'orientation des pièces. Tests à implémenter : Vérifie
que deux jeux identiques sont considérés comme égaux. Vérifie que deux jeux avec
des pièces différentes sont considérés comme non égaux. Teste avec
ignore_orientation à true et à false pour s'assurer que l'orientation est
correctement ignorée ou prise en compte.*/

bool test_game_equal()
{
  // Fonction pour générer un tableau aléatoire de shapes

  shape* s = malloc(DEFAULT_SIZE * DEFAULT_SIZE * sizeof(shape));
  if (s != NULL) {
    for (int i = 0; i < DEFAULT_SIZE * DEFAULT_SIZE; i++) {
      s[i] = rand() % NB_SHAPES;
    }
  }

  // Fonction pour générer un tableau aléatoire de directions

  direction* dir = malloc(DEFAULT_SIZE * DEFAULT_SIZE * sizeof(direction));
  if (s != NULL) {
    for (int i = 0; i < DEFAULT_SIZE * DEFAULT_SIZE; i++) {
      dir[i] = rand() % NB_DIRS;
    }
  }

  // Génération de tableaux de shapes et directions aléatoires

  assert(s);
  assert(dir);

  // Création des jeux avec les données générées
  game g1 = game_new(s, dir);
  game g3 = game_copy(g1);
  game g2 = game_new(s, dir);

  // Vérification de l'égalité entre les jeux
  if (!game_equal(g1, g3, true) || !game_equal(g1, g3, false)) {
    return false;
  }

  game_shuffle_orientation(g3);

  if (!game_equal(g1, g3, true) || game_equal(g1, g3, false)) {
    return false;
  }

  game_delete(g1);
  game_delete(g2);
  game_delete(g3);

  return true;
}

/**
 * @brief Deletes the game and frees the allocated memory.
 * @param g the game to delete
 * @pre @p g must be a valid pointer toward a game structure.
 **/
/*Ce que fait la fonction : Libère la mémoire allouée à un jeu.
Tests à implémenter :
Appelle la fonction et vérifie qu'elle ne provoque pas d'erreurs (comme un
segmentation fault). Utilise un outil comme valgrind pour vérifier qu’il n’y a
pas de fuite de mémoire après la suppression du jeu.*/

bool test_game_delete()
{
  game g = game_default();
  game_delete(g);
  return true;
}

/**
 * @brief Sets the piece shape in a given square.
 * @details This function is useful for initializing the squares of an empty
 * game.
 * @param g the game
 * @param i row index
 * @param j column index
 * @param s shape
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game height
 * @pre @p j < game width
 *
 Ce que fait la fonction : Change la forme d’une pièce dans un jeu.
Tests à implémenter :
Modifie la forme d'une pièce et vérifie que le changement a bien été effectué.
Vérifie que les autres pièces n’ont pas été affectées par le changement.
Teste avec des indices en dehors des bornes et vérifie que la fonction ne
modifie rien ou génère une erreur.*/

bool test_game_set_piece_shape()
{
  game g = game_default();
  game g_copy = game_copy(g);  // Crée une copie du jeu original pour comparaison

  bool result = true;
  game_set_piece_shape(g, 0, 0, EMPTY);
  if (game_get_piece_shape(g, 0, 0) != EMPTY) {
    result = false;
  }

  bool no_change = true;

  for (unsigned int i = 0; i < DEFAULT_SIZE; i++) {
    for (unsigned int j = 0; j < DEFAULT_SIZE; j++) {
      if ((i != 0 || j != 0) && game_get_piece_shape(g, i, j) != game_get_piece_shape(g_copy, i, j)) {
        no_change = false;
      }
    }
  }

  game_delete(g);
  game_delete(g_copy);

  return result && no_change;
}

/**
 * @brief Sets the piece orientation in a given square.
 * @details This function is useful for initializing the squares of an empty
 * game.
 * @param g the game
 * @param i row index
 * @param j column index
 * @param o orientation
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game height
 * @pre @p j < game width
 *Ce que fait la fonction : Change l’orientation d’une pièce dans un jeu.
Tests à implémenter :
Modifie l'orientation d'une pièce et vérifie que le changement a bien été
effectué. Vérifie que les autres pièces n’ont pas été affectées. Teste avec des
indices en dehors des bornes et vérifie le comportement attendu.*/

bool test_game_set_piece_orientation()
{
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

int main(int argc, char* argv[])
{
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
  } else if (strcmp(argv[1], "test_game_new") == 0) {
    if (test_game_new()) {
      printf("test_game_new PASSED\n");
      return EXIT_SUCCESS;
    } else {
      printf("test_game_new FAILED\n");
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