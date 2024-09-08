#include "tetris.h"

void game_loop() {
  timeout(10);

  for (int i = 1; i <= TEMPLATESAMOUNT; i++) {
    init_pair(i, COLOR_WHITE, i);
  }

  params_t *game_instance = getGameInstance();
  int *terminate_status = getTerminateStatus();

  while (!(*terminate_status)) {
    updateCurrentState();
    if (*terminate_status) {
      terminate_game(game_instance);
    }
  }
}

int main() {
  WIN_INIT();
  game_loop();
  endwin();
  return 0;
}