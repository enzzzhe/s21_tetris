#ifndef OBJECTS_H
#define OBJECTS_H

#include "defines.h"

typedef struct {
  int x;
  int y;
} tetramonie_pos;

typedef struct {
  int current;
  int next;
} template_number;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef struct {
  GameInfo_t game_info;
  tetramonie_pos pos;
  template_number tmplt_num;
  int templates[TEMPLATESAMOUNT][FIGURE_SIZE][FIGURE_SIZE];
  int **field_colors;
} params_t;

#endif