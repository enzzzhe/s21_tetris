#ifndef DEFINES_H
#define DEFINES_H

#define HIGH_SCORE_FILE "high_score.txt"

#define CLOCKS_PER_SEC_TICKS (CLOCKS_PER_SEC / 1000)
#define DEBOUNCE_INTERVAL 0.01

#define BOARD_M 10
#define ROWS_MAP 20
#define BOARD_N 40

#define TEMPLATESAMOUNT 7
#define FIGURE_SIZE 5

#define TETRAMONIESTART_X ((BOARD_M - FIGURE_SIZE) / 2) + 1
#define TETRAMONIESTART_Y 0

#define MAP_PADDING 3

#define NOSIG -1
#define ESCAPE 27
#define ENTER_KEY 10
#define KEY_SPACE 32
#define X_KEY 120
#define Z_KEY 122

#define LEVEL_SCORE 600

#define FINAL_LEVEL 11

#define WIN_INIT()          \
  {                         \
    initscr();              \
    cbreak();               \
    noecho();               \
    nodelay(stdscr, FALSE); \
    keypad(stdscr, TRUE);   \
    nodelay(stdscr, TRUE);  \
    srand(time(NULL));      \
    start_color();          \
  }

#endif