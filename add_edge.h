// @copyright University of Bordeaux. All rights reserved, 2024.

/* ************************************************************************** */

/** @brief Hard-coding of pieces (shape & orientation) in an integer array.
 * @details The 4 least significant bits encode the presence of an half-edge in
 * the N-E-S-W directions (in that order). Thus, binary coding 1100 represents
 * the piece "└" (a corner in north orientation).

 */

#ifndef __ADD_EDGE_H__
#define __ADD_EDGE_H__
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"

/* ************************************************************************** */

/** encode a shape and an orientation into an integer code */
uint _encode_shape(shape s, direction o);

/* ************************************************************************** */

/** decode an integer code into a shape and an orientation */
bool _decode_shape(uint code, shape *s, direction *o);

/* ************************************************************************** */

/** add an half-edge in the direction d */
void _add_half_edge(game g, uint i, uint j, direction d);

/* ************************************************************************** */

#define OPPOSITE_DIR(d) ((d + 2) % NB_DIRS)

/* ************************************************************************** */

/**
 * @brief Add an edge between two adjacent squares.
 * @details This is done by modifying the pieces of the two adjacent squares.
 * More precisely, we add an half-edge to each adjacent square, so as to build
 * an edge between these two squares.
 * @param g the game
 * @param i row index
 * @param j column index
 * @param d the direction of the adjacent square
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game height
 * @pre @p j < game width
 * @return true if an edge can be added, false otherwise
 */
bool _add_edge(game g, uint i, uint j, direction d);

/* ************************************************************************** */
#endif