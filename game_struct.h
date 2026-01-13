#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "queue.h"

#ifndef __GAME_STRUCT_H__
#define __GAME_STRUCT_H__

struct game_s {
  uint nb_rows, nb_cols;
  bool wrapping;
  shape *shapes;
  direction *orientations;

  queue *undo;
  queue *redo;
};

#endif  // __GAME_STRUCT_H__