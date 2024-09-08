#ifndef TETRIS_FRONTEND_H
#define TETRIS_FRONTEND_H

#include <ncurses.h>
#include <stdbool.h>

#include "../../brick_game/tetris/defines.h"
#include "../../brick_game/tetris/fsm.h"
#include "../../brick_game/tetris/objects.h"

void print_rectangle(int bottom_y, int right_x);
void print_field_and_figures(params_t prms, int top_y, int left_x);
int show_start_screen();
void color_block(int top_y, int left_x, int i, int j, int color_pair);
void print_next(int **next, int template_number, int top_y, int left_x);
void print_states(params_t *prms, int left_x);
void render(params_t *prms);

#endif