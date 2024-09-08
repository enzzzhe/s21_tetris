#include "tetris_backend.h"

void init_figures(params_t *prms) {
  int templates[TEMPLATESAMOUNT][FIGURE_SIZE][FIGURE_SIZE] = {
      {{0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {1, 1, 1, 1, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0}},
      {{0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 1, 1, 0, 0},
       {0, 1, 1, 0, 0},
       {0, 0, 0, 0, 0}},
      {{0, 0, 0, 0, 0},
       {0, 0, 1, 0, 0},
       {1, 1, 1, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0}},
      {{0, 0, 0, 0, 0},
       {0, 1, 0, 0, 0},
       {0, 1, 0, 0, 0},
       {0, 1, 1, 0, 0},
       {0, 0, 0, 0, 0}},
      {{0, 0, 0, 0, 0},
       {0, 0, 1, 0, 0},
       {0, 0, 1, 0, 0},
       {0, 1, 1, 0, 0},
       {0, 0, 0, 0, 0}},
      {{0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {0, 1, 1, 0, 0},
       {1, 1, 0, 0, 0},
       {0, 0, 0, 0, 0}},
      {{0, 0, 0, 0, 0},
       {0, 0, 0, 0, 0},
       {1, 1, 0, 0, 0},
       {0, 1, 1, 0, 0},
       {0, 0, 0, 0, 0}}};
  for (int i = 0; i < TEMPLATESAMOUNT; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      for (int k = 0; k < FIGURE_SIZE; k++) {
        prms->templates[i][j][k] = templates[i][j][k];
      }
    }
  }
}

void init_field(params_t *prms) {
  prms->game_info.field = (int **)malloc(ROWS_MAP * sizeof(int *));
  prms->field_colors = (int **)malloc(ROWS_MAP * sizeof(int *));
  for (int i = 0; i < ROWS_MAP; i++) {
    prms->game_info.field[i] = (int *)malloc(BOARD_M * sizeof(int));
    prms->field_colors[i] = (int *)malloc(BOARD_M * sizeof(int));
    memset(prms->game_info.field[i], 0, BOARD_M * sizeof(int));
    memset(prms->field_colors[i], 0, BOARD_M * sizeof(int));
  }

  prms->game_info.next = (int **)malloc(FIGURE_SIZE * sizeof(int *));
  for (int i = 0; i < FIGURE_SIZE; i++) {
    prms->game_info.next[i] = (int *)malloc(FIGURE_SIZE * sizeof(int));
    memset(prms->game_info.next[i], 0, FIGURE_SIZE * sizeof(int));
  }

  init_next(prms);
}

bool check_collide(params_t prms) {
  int(*figure)[FIGURE_SIZE][FIGURE_SIZE] =
      &prms.templates[prms.tmplt_num.current];
  int x = prms.pos.x;
  int y = prms.pos.y;
  int **field = prms.game_info.field;
  bool result = false;
  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      if ((*figure)[i][j] == 1) {
        int newX = x + j;
        int newY = y + i;
        if (newX < 0 || newX >= BOARD_M || newY >= ROWS_MAP ||
            field[newY][newX] == 1) {
          result = true;
        }
      }
    }
  }
  return result;
}

void init_next(params_t *prms) {
  int figure_index = prms->tmplt_num.next;
  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      prms->game_info.next[i][j] = prms->templates[figure_index][i][j];
    }
  }
}

void attach(params_t *prms) {
  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      if ((prms->templates[prms->tmplt_num.current])[i][j] &&
          prms->pos.y + i >= 0) {
        prms->game_info.field[prms->pos.y + i][prms->pos.x + j] = 1;
        prms->field_colors[prms->pos.y + i][prms->pos.x + j] =
            prms->tmplt_num.current + 1;
      }
    }
  }
}

void spawn(params_t *prms) {
  figurepos_init(&prms->pos);
  prms->tmplt_num.current = prms->tmplt_num.next;
  prms->tmplt_num.next = rand() % TEMPLATESAMOUNT;
  init_next(prms);
}

int load_high_score() {
  FILE *file = fopen(HIGH_SCORE_FILE, "r");
  int high_score = 0;

  if (file) {
    fscanf(file, "%d", &high_score);
    fclose(file);
  }

  return high_score;
}

void save_high_score(int high_score) {
  FILE *file = fopen(HIGH_SCORE_FILE, "w");

  if (file) {
    fprintf(file, "%d", high_score);
    fclose(file);
  }
}

void drop_down(params_t *prms) {
  while (!check_collide(*prms)) {
    (prms->pos.y)++;
  }
  (prms->pos.y)--;
}

void moveleft(params_t *prms) {
  prms->pos.x--;
  if (check_collide(*prms)) {
    prms->pos.x++;
  }
}

void moveright(params_t *prms) {
  prms->pos.x++;
  if (check_collide(*prms)) {
    prms->pos.x--;
  }
}

void rotate(params_t *prms) {
  if (prms->tmplt_num.current != 1) {
    int tmp[FIGURE_SIZE][FIGURE_SIZE];
    for (int i = 0; i < FIGURE_SIZE; i++) {
      for (int j = 0; j < FIGURE_SIZE; j++) {
        tmp[i][j] =
            prms->templates[prms->tmplt_num.current][FIGURE_SIZE - j - 1][i];
      }
    }

    for (int i = 0; i < FIGURE_SIZE; i++) {
      for (int j = 0; j < FIGURE_SIZE; j++) {
        prms->templates[prms->tmplt_num.current][i][j] = tmp[i][j];
      }
    }

    if (check_collide(*prms)) {
      unrotate(prms);
    }
  }
}

void unrotate(params_t *prms) {
  if (prms->tmplt_num.current != 1) {
    int tmp[FIGURE_SIZE][FIGURE_SIZE];
    int(*figure)[FIGURE_SIZE][FIGURE_SIZE] =
        &prms->templates[prms->tmplt_num.current];

    for (int i = 0; i < FIGURE_SIZE; i++) {
      for (int j = 0; j < FIGURE_SIZE; j++) {
        tmp[i][j] = (*figure)[j][FIGURE_SIZE - i - 1];
      }
    }

    for (int i = 0; i < FIGURE_SIZE; i++) {
      for (int j = 0; j < FIGURE_SIZE; j++) {
        (*figure)[i][j] = tmp[i][j];
      }
    }
  }
}

void move_down(params_t *prms) { prms->pos.y++; }

params_t *getGameInstance() {
  static params_t *prms_inst = NULL;
  if (prms_inst == NULL) {
    prms_inst = (params_t *)malloc(sizeof(params_t));
    memset(prms_inst, 0, sizeof(params_t));
    initGame(prms_inst);
  }

  return prms_inst;
}

void initGame(params_t *prms) {
  init_figures(prms);
  init_field(prms);
  prms->game_info.pause = false;
  prms->game_info.speed = 10;
  prms->game_info.level = 1;
  prms->game_info.score = 0;
  prms->game_info.high_score = load_high_score();
  prms->tmplt_num.current = rand() % TEMPLATESAMOUNT;
  prms->tmplt_num.next = rand() % TEMPLATESAMOUNT;
}

int *getTerminateStatus() {
  static int terminate = 0;
  return &terminate;
}

bool is_full_line(params_t *prms, int row) {
  for (int j = 0; j < BOARD_M; j++) {
    if (prms->game_info.field[row][j] == 0) {
      return false;
    }
  }
  return true;
}

void shift_lines_down(params_t *prms, int from_row) {
  for (int k = from_row; k > 0; k--) {
    for (int j = 0; j < BOARD_M; j++) {
      prms->game_info.field[k][j] = prms->game_info.field[k - 1][j];
      prms->field_colors[k][j] = prms->field_colors[k - 1][j];
    }
  }
}

void clear_top_row(params_t *prms) {
  for (int j = 0; j < BOARD_M; j++) {
    prms->game_info.field[0][j] = 0;
    prms->field_colors[0][j] = 0;
  }
}

void update_score(params_t *prms, int lines_cleared) {
  switch (lines_cleared) {
    case 1:
      prms->game_info.score += 100;
      break;
    case 2:
      prms->game_info.score += 300;
      break;
    case 3:
      prms->game_info.score += 700;
      break;
    case 4:
      prms->game_info.score += 1500;
      break;
  }
}

void update_high_score_and_level(params_t *prms) {
  prms->game_info.high_score = load_high_score();
  prms->game_info.level = (prms->game_info.score / LEVEL_SCORE) + 1;
  prms->game_info.speed = FINAL_LEVEL - prms->game_info.level;

  if (prms->game_info.score > prms->game_info.high_score) {
    prms->game_info.high_score = prms->game_info.score;
    save_high_score(prms->game_info.high_score);
  }
}

void clear_lines(params_t *prms) {
  int lines_cleared = 0;

  for (int i = 0; i < ROWS_MAP; i++) {
    if (is_full_line(prms, i)) {
      lines_cleared++;
      shift_lines_down(prms, i);
      clear_top_row(prms);
    }
  }

  update_score(prms, lines_cleared);
  update_high_score_and_level(prms);
}

void cleanupGameInstance(params_t *prms) {
  for (int i = 0; i < ROWS_MAP; i++) {
    if (prms->game_info.field[i] != NULL) {
      free(prms->game_info.field[i]);
      prms->game_info.field[i] = NULL;  // Prevent use after free
    }
    if (prms->field_colors[i] != NULL) {
      free(prms->field_colors[i]);
      prms->field_colors[i] = NULL;  // Prevent use after free
    }
  }
}

void terminate_game(params_t *prms) {
  for (int i = 0; i < ROWS_MAP; i++) {
    free(prms->game_info.field[i]);
    free(prms->field_colors[i]);
  }
  free(prms->game_info.field);
  free(prms->field_colors);

  for (int i = 0; i < FIGURE_SIZE; i++) {
    free(prms->game_info.next[i]);
  }
  free(prms->game_info.next);

  free(prms);
}

void figurepos_init(tetramonie_pos *pos) {
  pos->x = TETRAMONIESTART_X;
  pos->y = TETRAMONIESTART_Y;
}
