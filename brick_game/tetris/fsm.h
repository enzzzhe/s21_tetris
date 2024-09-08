#ifndef FSM_H
#define FSM_H

#include "../../gui/cli/tetris_frontend.h"
#include "defines.h"
#include "objects.h"
#include "tetris_backend.h"

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef enum {
  STATE_START,
  STATE_PAUSED,
  STATE_GAME_OVER,
  SPAWN_STATE,
  MOVE_STATE,
  ATTACH_STATE
} GameState_t;

UserAction_t userInput(int user_input, bool hold);
GameInfo_t updateCurrentState();
UserAction_t processInput(GameState_t *current_state);
void debounceInput(clock_t *last_action_time, UserAction_t *action);
void handleStateStart(GameState_t *current_state, int *terminate,
                      clock_t *last_down_time);
void handleStateSpawn(params_t *prms, GameState_t *current_state,
                      clock_t *last_down_time);
void handleStateMove(params_t *prms, GameState_t *current_state, int *terminate,
                     UserAction_t action, clock_t *last_down_time);
void handleStatePaused(params_t *prms, GameState_t *current_state,
                       int *terminate, UserAction_t action);
void handleStateAttach(params_t *prms, GameState_t *current_state,
                       clock_t *last_down_time);
void handleStateGameOver(params_t *prms, GameState_t *current_state,
                         int *terminate, UserAction_t action);

#endif