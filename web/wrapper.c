/**
 * @file wrapper.js
 * @brief Game Binding to JavaScript (based on Emscripten & Wasm)
 * @author aurelien.esnard@u-bordeaux.fr
 * @copyright University of Bordeaux. All rights reserved, 2023.
 **/

#include <emscripten.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "../game.h"
#include "../game_aux.h"
#include "../game_ext.h"
#include "../game_tools.h"

/* ******************** Game WASM API ******************** */

EMSCRIPTEN_KEEPALIVE
game new_default(void) { return game_default(); }

EMSCRIPTEN_KEEPALIVE
void delete (game g) { game_delete(g); }

EMSCRIPTEN_KEEPALIVE
void play_move(game g, uint i, uint j, int nb_quarter_turns) {
  game_play_move(g, i, j, nb_quarter_turns);
}

EMSCRIPTEN_KEEPALIVE
void restart(game g) { game_shuffle_orientation(g); }

EMSCRIPTEN_KEEPALIVE
bool won(game g) { return game_won(g); }

EMSCRIPTEN_KEEPALIVE
shape get_piece_shape(cgame g, uint i, uint j) {
  return game_get_piece_shape(g, i, j);
}

EMSCRIPTEN_KEEPALIVE
direction get_piece_orientation(cgame g, uint i, uint j) {
  return game_get_piece_orientation(g, i, j);
}

EMSCRIPTEN_KEEPALIVE
uint nb_rows(cgame g) { return game_nb_rows(g); }

EMSCRIPTEN_KEEPALIVE
uint nb_cols(cgame g) { return game_nb_cols(g); }

EMSCRIPTEN_KEEPALIVE
edge_status check_edge(cgame g, uint i, uint j, direction d) {
  return game_check_edge(g, i, j, d);
}

EMSCRIPTEN_KEEPALIVE
void undo(game g) { game_undo(g); }

EMSCRIPTEN_KEEPALIVE
void redo(game g) { game_redo(g); }

EMSCRIPTEN_KEEPALIVE
bool solve(game g) { return game_solve(g); }

EMSCRIPTEN_KEEPALIVE
game new_random_game(uint nb_rows, uint nb_cols, bool wrapping) {
  printf("Création d'un jeu avec %u rows × %u cols, wrapping = %s\n", nb_rows,
         nb_cols, wrapping ? "true" : "false");

  game g = game_random(nb_rows, nb_cols, wrapping, 0, 0);
  game_shuffle_orientation(g);
  return g;
}

// EOF