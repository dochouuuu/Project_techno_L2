#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>  
#include "game.h"
#include "game_aux.h"

/* ********** TEST DUMMY ********** */
int test_dummy() {
    return EXIT_SUCCESS; 
}

/* ********** TEST GAME GET PIECE SHAPE ********** */
bool test_game_get_piece_shape(void) {
    game g = game_default();
    
    //Vérifier que chaque case a un état initial non modifié
    bool initial_test = true;
    for (uint i = 0; i < DEFAULT_SIZE; i++) {
        for (uint j = 0; j < DEFAULT_SIZE; j++) {
            shape initial_shape = game_get_piece_shape(g, i, j);
            if (initial_shape != game_get_piece_shape(g, i, j)) {
                initial_test = false;  // Détecte toute incohérence
            }
        }
    }

    //Définir une forme spécifique pour (0,0) et vérifier la modification
    game_set_piece_shape(g, 0, 0, CORNER);
    bool test1 = (game_get_piece_shape(g, 0, 0) == CORNER);

    //Modifier une autre case et vérifier le changement
    game_set_piece_shape(g, 4, 4, ENDPOINT);
    bool test2 = (game_get_piece_shape(g, 4, 4) == ENDPOINT);

    //Modifier la première case avec une autre forme
    game_set_piece_shape(g, 0, 0, SEGMENT);
    bool test3 = (game_get_piece_shape(g, 0, 0) == SEGMENT);

    game_delete(g);

    return initial_test && test1 && test2 && test3;
}


/* ********** TEST GAME GET PIECE ORIENTATION ********** */
bool test_game_get_piece_orientation(void) {
    game g = game_default();
    
    game_set_piece_orientation(g, 0, 0, EAST);
    game_set_piece_orientation(g, 2, 2, SOUTH);
    
    bool test1 = (game_get_piece_orientation(g, 0, 0) == EAST);
    bool test2 = (game_get_piece_orientation(g, 2, 2) == SOUTH);
    
    game_delete(g);
    return test1 && test2;
}

/* ********** TEST GAME PLAY MOVE ********** */
bool test_game_play_move(void){
    game g1 = game_default();
    assert(g1 != NULL);
    cgame g = g1;
    game g2 = game_copy(g);
    for(uint i = 0; i < DEFAULT_SIZE; i++){
        for(uint j = 0; j < DEFAULT_SIZE; j++){
            game_play_move(g2, i, j, 2);
            game_play_move(g2, i, j, -2);
        }
    }
    cgame g0 = g2;
    bool test = game_equal(g, g0, false);
    game_delete(g1);
    game_delete(g2);
    return test;
}

/* ********** TEST GAME WON ********** */
bool test_game_won() {
    // test 1 : le jeu vide est gagné par défaut (le graphe vide est connexe)
    game g = game_new_empty();
    if (!game_won(g)) {
        game_delete(g);
        return false;
    }

    // test 2 : le jeu par défaut n'est pas gagné, la solution par défaut est gagnée
    if (game_won(game_default()) || !game_won(game_default_solution())) {
        game_delete(g);
        return false;
    }

    // test 3 : un jeu avec 1 seule case non-vide ne peut pas être gagné (car pas bien appairé)
    game_set_piece_shape(g, 2, 2, CORNER);
    if (game_won(g)) {
        game_delete(g);
        return false;
    }

    // test 4 : un jeu avec un petit ilot appairé et tout le reste en empty est gagnant
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

    // test 5 : juste un ilot pas bien appairé, donc pas gagnant
    game_set_piece_shape(g, 1, 3, TEE);
    if (game_won(g)) {
        game_delete(g);
        return false;
    }

    // test 6 : un ilot bien appairé, un ilot mal appairé
    game_set_piece_shape(g, 1, 3, CORNER);
    game_set_piece_shape(g, 3, 0, SEGMENT);
    game_set_piece_shape(g, 2, 0, ENDPOINT);

    if (game_won(g)) {
        game_delete(g);
        return false;
    }

    // test 7 : deux sous-ilots connexes bien appairés, donc pas de victoire
    game_set_piece_shape(g, 4, 0, ENDPOINT);
    game_set_piece_orientation(g, 2, 0, SOUTH);

    if (game_won(g)) {
        game_delete(g);
        return false;
    }

    game_delete(g);

    // Si tous les tests sont passés, retourne true pour indiquer le succès
    return true;
}


/* ********** TEST GAME RESET ORIENTATION ********** */
bool test_game_reset_orientation(void) {
    game g = game_default();

    // Change les orientations pour tester la réinitialisation
    game_play_move(g, 0, 0, 1);
    game_play_move(g, 1, 1, 2);

    game_reset_orientation(g);

    // Vérifie si toutes les pièces sont bien orientées au nord
    bool all_north = true;
    for (uint i = 0; i < DEFAULT_SIZE; i++) {
        for (uint j = 0; j < DEFAULT_SIZE; j++) {
            if (game_get_piece_orientation(g, i, j) != NORTH) {
                all_north = false;
            }
        }
    }

    game_delete(g);
    return all_north;
}

/* ********** TEST GAME SHUFFLE ORIENTATION ********** */
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
    return shuffled_00 && shuffled_11 && shuffled_22;  // S'assurer que toutes les pièces ont changé au moins une fois
}

/* ********** TEST GAME PRINT ********** */
bool test_game_print(void) {
    game g = game_default();
    
    game_print(g);  

    game_delete(g);  
    
    return true;  
}

int main(int argc, char *argv[]) {
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
    } 
    
    else if (strcmp(argv[1], "test_game_get_piece_orientation") == 0) {
        if (test_game_get_piece_orientation()) {
            printf("Test game_get_piece_orientation PASSED\n"); 
            return EXIT_SUCCESS;
        } else {
            printf("\n**Test game_get_piece_orientation FAILED**\n"); 
            return EXIT_FAILURE;
        }
    } 
    
    else if (strcmp(argv[1], "test_game_play_move") == 0) {
        if (test_game_play_move()) {
            printf("Test game_play_move PASSED\n"); 
            return EXIT_SUCCESS;
        } else {
            printf("\n**Test game_play_move FAILED**\n"); 
            return EXIT_FAILURE;
        }
    } 
    
    else if (strcmp(argv[1], "test_game_won") == 0) {
        if (test_game_won()) {
            printf("Test game_won PASSED\n"); 
            return EXIT_SUCCESS;
        } else {
            printf("\n**Test game_won FAILED**\n"); 
            return EXIT_FAILURE;
        }
    } 
    
    else if (strcmp(argv[1], "test_game_reset_orientation") == 0) {
        if (test_game_reset_orientation()) {
            printf("Test game_reset_orientation PASSED\n"); 
            return EXIT_SUCCESS;
        } else {
            printf("\n**Test game_reset_orientation FAILED**\n"); 
            return EXIT_FAILURE;
        }
    } 
    
    else if (strcmp(argv[1], "test_game_shuffle_orientation") == 0) {
        if (test_game_shuffle_orientation()) {
            printf("Test game_shuffle_orientation PASSED\n"); 
            return EXIT_SUCCESS;
        } else {
            printf("\n**Test game_shuffle_orientation FAILED**\n"); 
            return EXIT_FAILURE;
        }
    } 
    
    else if (strcmp(argv[1], "test_game_print") == 0) {
        if (test_game_print()) {
            printf("Test game_print PASSED\n"); 
            return EXIT_SUCCESS;
        } else {
            printf("\n**Test game_print FAILED**\n"); 
            return EXIT_FAILURE;
        }
    } 
    
    else {
        fprintf(stderr, "Unknown test: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
}