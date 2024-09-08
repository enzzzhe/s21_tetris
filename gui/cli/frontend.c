#include "tetris_frontend.h"

void print_rectangle(int bottom_y, int right_x) {
  mvwaddch(stdscr, 0, 0, ACS_ULCORNER);
  mvwaddch(stdscr, 0, right_x * 2 - 1, ACS_URCORNER);
  mvwaddch(stdscr, bottom_y, 0, ACS_LLCORNER);
  mvwaddch(stdscr, bottom_y, right_x * 2 - 1, ACS_LRCORNER);

  for (int i = 1; i < right_x * 2 - 1; i++) {
    mvwaddch(stdscr, 0, i, ACS_HLINE);
    mvwaddch(stdscr, bottom_y, i, ACS_HLINE);
  }

  for (int i = 1; i < bottom_y; i++) {
    mvwaddch(stdscr, i, 0, ACS_VLINE);
    mvwaddch(stdscr, i, right_x * 2 - 1, ACS_VLINE);
  }
}

void print_field_and_figures(params_t prms, int top_y, int left_x) {
  int color_pair = prms.tmplt_num.current + 1;
  int(*figure)[FIGURE_SIZE][FIGURE_SIZE] =
      &prms.templates[prms.tmplt_num.current];
  for (int i = 0; i < ROWS_MAP; i++) {
    for (int j = 0; j < BOARD_M; j++) {
      bool isFigure = false;
      if (i >= prms.pos.y && i < prms.pos.y + FIGURE_SIZE && j >= prms.pos.x &&
          j < prms.pos.x + FIGURE_SIZE) {
        if ((*figure)[i - prms.pos.y][j - prms.pos.x] == 1) {
          isFigure = true;
        }
      }
      if (isFigure) {
        attron(COLOR_PAIR(color_pair));
        mvprintw(top_y + i, left_x + j * 2, "  ");
        attroff(COLOR_PAIR(color_pair));
      } else if (prms.game_info.field[i][j]) {
        attron(COLOR_PAIR(prms.field_colors[i][j]));
        mvprintw(top_y + i, left_x + j * 2, "  ");
        attroff(COLOR_PAIR(prms.field_colors[i][j]));
      } else if (j != BOARD_M) {
        mvprintw(top_y + i, left_x + j * 2, ". ");
      } else {
        mvprintw(top_y + i, left_x + j * 2, ".");
      }
    }
  }
}

int show_start_screen() {
  int status = -1;
  clear();
  mvprintw(5, 10, "Press Enter to Start");
  mvprintw(7, 10, "Press ESC to Quit");
  mvprintw(14, 10, "Press Left arrow to Shift Left");
  mvprintw(16, 10, "Press Right arrow to Shift Right");
  mvprintw(18, 10, "Press x, z to Rotate");
  mvprintw(20, 10, "Press Space to Pause");
  refresh();
  int ch = 0;
  while (status != 1 && status != 0) {
    ch = getch();
    UserAction_t rc = userInput(ch, TRUE);
    if (rc == Start) {
      status = 1;
    } else if (rc == Terminate) {
      status = 0;
    }
  }
  return status;
}

void color_block(int top_y, int left_x, int i, int j, int color_pair) {
  attron(COLOR_PAIR(color_pair));
  mvprintw(top_y + i, left_x * 2 + j * 2, "  ");
  attroff(COLOR_PAIR(color_pair));
}

void print_next(int **next, int template_number, int top_y, int left_x) {
  top_y += 2;
  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      if (next[i][j]) {
        color_block(top_y, left_x - 10, i, j, template_number + 1);
      }
    }
  }
}

void print_states(params_t *prms, int left_x) {
  mvprintw(10, left_x + 5, "Level: %d", prms->game_info.level);
  mvprintw(11, left_x + 5, "Speed: %d%%",
           ((10 - prms->game_info.speed) + 1) * 10);
  mvprintw(13, left_x + 5, "Score: %d", prms->game_info.score);
  mvprintw(14, left_x + 5, "High score: %d", prms->game_info.high_score);
}

void render(params_t *prms) {
  clear();
  int left_x = 1, top_y = 1;
  int right_x = BOARD_M + 1, bottom_y = ROWS_MAP + 1;
  print_rectangle(bottom_y, right_x);
  print_field_and_figures(*prms, top_y, left_x);
  int nextPieceLeftX = BOARD_M + 15;
  int nextPieceTopY = 1;
  print_next(prms->game_info.next, prms->tmplt_num.next, nextPieceTopY,
             nextPieceLeftX);
  print_states(prms, nextPieceLeftX);
  refresh();
}