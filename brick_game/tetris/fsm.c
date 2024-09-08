#include "fsm.h"

UserAction_t userInput(int user_input, bool hold) {
  UserAction_t rc = NOSIG;
  if (hold == TRUE && user_input == ENTER_KEY)
    rc = Start;
  else if (user_input == KEY_DOWN)
    rc = Down;
  else if (user_input == KEY_LEFT)
    rc = Left;
  else if (user_input == KEY_RIGHT)
    rc = Right;
  else if (user_input == ESCAPE)
    rc = Terminate;
  else if (user_input == KEY_SPACE)
    rc = Pause;
  else if (user_input == X_KEY || user_input == Z_KEY)
    rc = Action;
  return rc;
}

GameState_t *getStateInstance() {
  static GameState_t current_state = STATE_START;
  return &current_state;
}

UserAction_t processInput(GameState_t *current_state) {
  int ch = 0;
  UserAction_t action = NOSIG;

  if (*current_state == MOVE_STATE || *current_state == STATE_PAUSED ||
      *current_state == STATE_GAME_OVER) {
    ch = getch();
    action = userInput(ch, TRUE);
  }

  return action;
}

void debounceInput(clock_t *last_action_time, UserAction_t *action) {
  if (((double)(clock() - *last_action_time) / CLOCKS_PER_SEC * 200) >=
      DEBOUNCE_INTERVAL) {
    *last_action_time = clock();
  } else {
    *action = NOSIG;
  }
}

void handleStateStart(GameState_t *current_state, int *terminate,
                      clock_t *last_down_time) {
  if (show_start_screen() == 0) {
    *terminate = 1;
  } else {
    *current_state = SPAWN_STATE;
  }
  *last_down_time = clock();
}

void handleStateSpawn(params_t *prms, GameState_t *current_state,
                      clock_t *last_down_time) {
  spawn(prms);
  if (check_collide(*prms)) {
    *current_state = STATE_GAME_OVER;
  } else {
    *current_state = MOVE_STATE;
  }
  *last_down_time = clock();
}

void handleStateMove(params_t *prms, GameState_t *current_state, int *terminate,
                     UserAction_t action, clock_t *last_down_time) {
  if (action == Terminate) {
    *terminate = 1;
    return;
  } else if (action == Pause) {
    *current_state = STATE_PAUSED;
    return;
  }

  switch (action) {
    case Terminate:
      *terminate = 1;
      break;
    case Pause:
      *current_state = STATE_PAUSED;
      break;
    case Left:
      moveleft(prms);
      break;
    case Right:
      moveright(prms);
      break;
    case Down:
      drop_down(prms);
      *current_state = ATTACH_STATE;
      break;
    case Action:
      rotate(prms);
      break;
    case Up:
      break;
    case Start:
      break;
  }

  if (action != Terminate && action != Pause) {
    if (((double)(clock() - *last_down_time) / CLOCKS_PER_SEC * 200) >=
        prms->game_info.speed) {
      move_down(prms);
      *last_down_time = clock();

      if (check_collide(*prms)) {
        prms->pos.y--;
        *current_state = ATTACH_STATE;
      }
    }
  }
  render(prms);
}

void handleStatePaused(params_t *prms, GameState_t *current_state,
                       int *terminate, UserAction_t action) {
  if (action == Pause) {
    *current_state = MOVE_STATE;
  } else if (action == Terminate) {
    *terminate = 1;
  }
  render(prms);
}

void handleStateAttach(params_t *prms, GameState_t *current_state,
                       clock_t *last_down_time) {
  attach(prms);
  clear_lines(prms);

  if (prms->game_info.level == FINAL_LEVEL) {
    *current_state = STATE_GAME_OVER;
  } else {
    *current_state = SPAWN_STATE;
    *last_down_time = clock();
  }
}
void handleStateGameOver(params_t *prms, GameState_t *current_state,
                         int *terminate, UserAction_t action) {
  refresh();
  mvprintw(22, 1, "Game Over! Press Enter to Restart or ESC to Quit.");
  if (action == Start) {
    initGame(prms);
    *current_state = STATE_START;
    *terminate = 0;
  } else if (action == Terminate) {
    *terminate = 1;
  }
}

GameInfo_t updateCurrentState() {
  params_t *prms = getGameInstance();
  int *terminate = getTerminateStatus();
  GameState_t *current_state = getStateInstance();
  static clock_t last_down_time = 0;
  static clock_t last_action_time = 0;

  UserAction_t action = processInput(current_state);
  debounceInput(&last_action_time, &action);

  switch (*current_state) {
    case STATE_START:
      handleStateStart(current_state, terminate, &last_down_time);
      break;
    case SPAWN_STATE:
      handleStateSpawn(prms, current_state, &last_down_time);
      break;
    case MOVE_STATE:
      handleStateMove(prms, current_state, terminate, action, &last_down_time);
      break;
    case ATTACH_STATE:
      handleStateAttach(prms, current_state, &last_down_time);
      break;
    case STATE_PAUSED:
      handleStatePaused(prms, current_state, terminate, action);
      break;
    case STATE_GAME_OVER:
      handleStateGameOver(prms, current_state, terminate, action);
      break;
  }

  if (*terminate) {
    exit(0);
  }

  return prms->game_info;
}