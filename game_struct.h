#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"

#ifndef __GAME_STRUCT_H__
#define __GAME_STRUCT_H__

#define DEFAULT_SIZE 5

struct game_s {
  int size;
  shape* shapes;
  direction* orientations;
};

#endif  // __GAME_STRUCT_H__