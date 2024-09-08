#ifndef TETRIS_BACKEND_H
#define TETRIS_BACKEND_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "defines.h"
#include "objects.h"

void init_figures(params_t *prms);
void init_field(params_t *prms);
bool check_collide(params_t prms);
void init_next(params_t *prms);
void attach(params_t *prms);
void spawn(params_t *prms);
int load_high_score();
void save_high_score(int high_score);
void drop_down(params_t *prms);
void moveleft(params_t *prms);
void moveright(params_t *prms);
void rotate(params_t *prms);
void unrotate(params_t *prms);
void move_down(params_t *prms);
int *getTerminateStatus();
bool is_full_line(params_t *prms, int row);
void shift_lines_down(params_t *prms, int from_row);
void clear_top_row(params_t *prms);
void update_score(params_t *prms, int lines_cleared);
void update_high_score_and_level(params_t *prms);
void clear_lines(params_t *prms);
void cleanupGameInstance(params_t *prms);
void terminate_game(params_t *prms);
void figurepos_init(tetramonie_pos *pos);
void initGame(params_t *prms);
params_t *getGameInstance();

#endif