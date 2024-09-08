#include <assert.h>
#include <check.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../brick_game/tetris/tetris_backend.h"

START_TEST(test_no_collision) {
  params_t *prms = getGameInstance();

  prms->tmplt_num.current = 0;
  prms->pos.x = 4;
  prms->pos.y = 0;

  ck_assert_int_eq(check_collide(*prms), false);

  for (int i = 0; i < ROWS_MAP; i++) {
    free(prms->game_info.field[i]);
  }
  free(prms->game_info.field);
}
END_TEST

START_TEST(test_collision_edge) {
  params_t *prms = getGameInstance();

  init_figures(prms);
  init_field(prms);

  prms->tmplt_num.current = 0;
  prms->pos.x = BOARD_M - 4;
  prms->pos.y = 0;

  ck_assert_int_eq(check_collide(*prms), false);

  for (int i = 0; i < ROWS_MAP; i++) {
    free(prms->game_info.field[i]);
  }
  free(prms->game_info.field);
}
END_TEST

START_TEST(test_collision_field) {
  params_t *prms = getGameInstance();

  init_figures(prms);
  init_field(prms);

  prms->game_info.field[1][4] = 1;

  prms->tmplt_num.current = 0;
  prms->pos.x = 4;
  prms->pos.y = 0;

  ck_assert_int_eq(check_collide(*prms), false);
  for (int i = 0; i < ROWS_MAP; i++) {
    free(prms->game_info.field[i]);
  }
  free(prms->game_info.field);
}
END_TEST

START_TEST(test_attach) {
  params_t *prms = getGameInstance();

  init_figures(prms);
  init_field(prms);

  ck_assert_ptr_ne(prms->game_info.field, NULL);
  ck_assert_ptr_ne(prms->field_colors, NULL);

  int template[FIGURE_SIZE][FIGURE_SIZE] = {{0, 1, 0, 0, 0},
                                            {0, 1, 0, 0, 0},
                                            {0, 1, 0, 0, 0},
                                            {0, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0}};
  memcpy(prms->templates[0], template, sizeof(template));

  prms->tmplt_num.current = 0;
  prms->pos.x = 5;
  prms->pos.y = 10;

  attach(prms);

  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      int field_y = prms->pos.y + i;
      int field_x = prms->pos.x + j;

      if (field_y >= 0 && field_y < ROWS_MAP && field_x >= 0 &&
          field_x < BOARD_M) {
        int expected_value = (template[i][j] && field_y >= 0) ? 1 : 0;
        int expected_color =
            (template[i][j] && field_y >= 0) ? prms->tmplt_num.current + 1 : 0;
        ck_assert_int_eq(prms->game_info.field[field_y][field_x],
                         expected_value);
        ck_assert_int_eq(prms->field_colors[field_y][field_x], expected_color);
      } else {
        ck_assert_int_eq(prms->game_info.field[field_y][field_x], 0);
        ck_assert_int_eq(prms->field_colors[field_y][field_x], 0);
      }
    }
  }
}
END_TEST

START_TEST(test_spawn_template_number_range) {
  params_t *prms = getGameInstance();
  srand(time(NULL));

  prms->tmplt_num.current = TEMPLATESAMOUNT - 1;
  prms->tmplt_num.next = TEMPLATESAMOUNT - 1;

  spawn(prms);

  ck_assert_int_ge(prms->tmplt_num.current, 0);
  ck_assert_int_lt(prms->tmplt_num.current, TEMPLATESAMOUNT);
  ck_assert_int_ge(prms->tmplt_num.next, 0);
  ck_assert_int_lt(prms->tmplt_num.next, TEMPLATESAMOUNT);
}
END_TEST

START_TEST(test_spawn_random_template) {
  params_t *prms = getGameInstance();
  srand(time(NULL));

  int first_next = prms->tmplt_num.next;
  for (int i = 0; i < 100; i++) {
    spawn(prms);
    int new_next = prms->tmplt_num.next;
    if (new_next != first_next) {
      break;
    }
  }
  ck_assert_int_ne(prms->tmplt_num.next, first_next);
}
END_TEST

START_TEST(test_load_high_score) {
  FILE *file = fopen(HIGH_SCORE_FILE, "w");
  ck_assert_ptr_nonnull(file);

  fprintf(file, "%d", 500);
  fclose(file);

  int high_score = load_high_score();
  ck_assert_int_eq(high_score, 500);

  remove(HIGH_SCORE_FILE);
}
END_TEST

START_TEST(test_rotate_no_collision) {
  params_t *prms = getGameInstance();
  prms->tmplt_num.current = 0;

  int template[FIGURE_SIZE][FIGURE_SIZE] = {{0, 1, 0, 0, 0},
                                            {0, 1, 0, 0, 0},
                                            {0, 1, 0, 0, 0},
                                            {0, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0}};
  memcpy(prms->templates[0], template, sizeof(template));

  rotate(prms);

  int expected_rotated[FIGURE_SIZE][FIGURE_SIZE] = {{0, 0, 0, 0, 0},
                                                    {0, 1, 1, 1, 1},
                                                    {0, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 0}};

  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      ck_assert_int_eq(prms->templates[0][i][j], expected_rotated[i][j]);
    }
  }
}
END_TEST

START_TEST(test_rotate_skip_for_template_1) {
  params_t *prms = getGameInstance();
  prms->tmplt_num.current = 1;

  int template[FIGURE_SIZE][FIGURE_SIZE] = {{0, 1, 0, 0, 0},
                                            {0, 1, 0, 0, 0},
                                            {0, 1, 0, 0, 0},
                                            {0, 1, 0, 0, 0},
                                            {0, 0, 0, 0, 0}};
  memcpy(prms->templates[1], template, sizeof(template));

  rotate(prms);

  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      ck_assert_int_eq(prms->templates[1][i][j], template[i][j]);
    }
  }
}
END_TEST

START_TEST(test_full_line) {
  params_t *prms = getGameInstance();

  for (int j = 0; j < BOARD_M; j++) {
    prms->game_info.field[0][j] = 1;
  }

  ck_assert(is_full_line(prms, 0) == true);
}
END_TEST

START_TEST(test_partial_line) {
  params_t *prms = getGameInstance();

  for (int j = 0; j < BOARD_M; j++) {
    prms->game_info.field[0][j] = (j < BOARD_M - 2) ? 1 : 0;
  }

  ck_assert(is_full_line(prms, 0) == false);
}
END_TEST

START_TEST(test_empty_line) {
  params_t *prms = getGameInstance();

  for (int j = 0; j < BOARD_M; j++) {
    prms->game_info.field[0][j] = 0;
  }

  ck_assert(is_full_line(prms, 0) == false);
}
END_TEST

START_TEST(test_cleanup_game_instance) {
  params_t *prms = getGameInstance();

  cleanupGameInstance(prms);

  ck_assert(1);
}
END_TEST

START_TEST(test_double_cleanup) {
  params_t *prms = getGameInstance();

  cleanupGameInstance(prms);
  ck_assert(1);
}
END_TEST

START_TEST(test_update_high_score_and_level) {
  params_t *prms = getGameInstance();

  prms->game_info.score = 0;
  int mock_high_score = 1000;

  update_high_score_and_level(prms);

  ck_assert_int_eq(prms->game_info.level, 1);

  ck_assert_int_eq(prms->game_info.speed, FINAL_LEVEL - 1);

  prms->game_info.score = 2000;
  update_high_score_and_level(prms);

  ck_assert_int_eq(prms->game_info.high_score, 2000);

  prms->game_info.score = LEVEL_SCORE * 3;
  update_high_score_and_level(prms);

  ck_assert_int_eq(prms->game_info.level, 4);

  ck_assert_int_eq(prms->game_info.speed, FINAL_LEVEL - 4);
}
END_TEST

START_TEST(test_drop_down) {
  params_t *prms = getGameInstance();

  prms->tmplt_num.current = 0;
  prms->pos.x = 5;
  prms->pos.y = 0;

  drop_down(prms);

  ck_assert_int_eq(prms->pos.y, ROWS_MAP - FIGURE_SIZE + 2);

  cleanupGameInstance(prms);
}
END_TEST

START_TEST(test_moveleft_no_collision) {
  params_t *prms = getGameInstance();

  prms->tmplt_num.current = 0;
  prms->pos.x = 5;
  prms->pos.y = 0;

  moveleft(prms);

  ck_assert_int_eq(prms->pos.x, 4);

  cleanupGameInstance(prms);
}
END_TEST

START_TEST(test_moveleft_with_collision) {
  params_t *prms = getGameInstance();

  prms->tmplt_num.current = 0;
  prms->pos.x = 0;
  prms->pos.y = 0;

  moveleft(prms);

  ck_assert_int_eq(prms->pos.x, 0);

  cleanupGameInstance(prms);
}
END_TEST

START_TEST(test_moveright_no_collision) {
  params_t *prms = getGameInstance();

  prms->tmplt_num.current = 0;
  prms->pos.x = 5;
  prms->pos.y = 0;

  moveright(prms);

  ck_assert_int_eq(prms->pos.x, 6);

  cleanupGameInstance(prms);
}
END_TEST

START_TEST(test_moveright_with_collision) {
  params_t *prms = getGameInstance();

  prms->tmplt_num.current = 0;
  prms->pos.x = BOARD_M - FIGURE_SIZE;
  prms->pos.y = 0;

  moveright(prms);

  ck_assert_int_eq(prms->pos.x, BOARD_M - FIGURE_SIZE + 1);

  cleanupGameInstance(prms);
}
END_TEST

START_TEST(test_unrotate_correctness) {
  params_t *prms = getGameInstance();

  int original_template[FIGURE_SIZE][FIGURE_SIZE] = {{1, 0, 0, 0, 0},
                                                     {1, 0, 0, 0, 0},
                                                     {1, 0, 0, 0, 0},
                                                     {1, 0, 0, 0, 0},
                                                     {0, 0, 0, 0, 0}};
  memcpy(prms->templates[0], original_template, sizeof(original_template));

  rotate(prms);

  unrotate(prms);

  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      ck_assert_int_eq(prms->templates[0][i][j], original_template[i][j]);
    }
  }

  cleanupGameInstance(prms);
}
END_TEST

START_TEST(test_unrotate_no_rotation) {
  params_t *prms = getGameInstance();

  int template[FIGURE_SIZE][FIGURE_SIZE] = {{1, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0}};
  memcpy(prms->templates[0], template, sizeof(template));

  rotate(prms);
  unrotate(prms);

  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      ck_assert_int_eq(prms->templates[0][i][j], template[i][j]);
    }
  }

  // Cleanup
  cleanupGameInstance(prms);
}
END_TEST

START_TEST(test_update_score_1_line) {
  params_t *prms = getGameInstance();

  prms->game_info.score = 0;
  update_score(prms, 1);
  ck_assert_int_eq(prms->game_info.score, 100);

  // Cleanup
  cleanupGameInstance(prms);
}
END_TEST

START_TEST(test_update_score_2_lines) {
  params_t *prms = getGameInstance();

  prms->game_info.score = 0;
  update_score(prms, 2);
  ck_assert_int_eq(prms->game_info.score, 300);

  // Cleanup
  cleanupGameInstance(prms);
}
END_TEST

START_TEST(test_update_score_3_lines) {
  params_t *prms = getGameInstance();
  prms->game_info.score = 0;
  update_score(prms, 3);
  ck_assert_int_eq(prms->game_info.score, 700);

  // Cleanup
  cleanupGameInstance(prms);
}
END_TEST

START_TEST(test_update_score_4_lines) {
  params_t *prms = getGameInstance();
  prms->game_info.score = 0;
  update_score(prms, 4);
  ck_assert_int_eq(prms->game_info.score, 1500);

  cleanupGameInstance(prms);
}
END_TEST

START_TEST(test_update_score_invalid_lines) {
  params_t *prms = getGameInstance();
  prms->game_info.score = 0;
  update_score(prms, 0);
  ck_assert_int_eq(prms->game_info.score, 0);

  update_score(prms, 5);
  ck_assert_int_eq(prms->game_info.score, 0);

  cleanupGameInstance(prms);
}
END_TEST

START_TEST(test_shift_lines_down) {
  params_t *prms = getGameInstance();

  for (int i = 0; i < ROWS_MAP; i++) {
    for (int j = 0; j < BOARD_M; j++) {
      prms->game_info.field[i][j] = i * 10 + j;
      prms->field_colors[i][j] = 1;
    }
  }

  shift_lines_down(prms, 1);

  for (int j = 0; j < BOARD_M; j++) {
    ck_assert_int_eq(prms->game_info.field[1][j], 0 * 10 + j);
    ck_assert_int_eq(prms->field_colors[1][j], 1);
  }

  cleanupGameInstance(prms);
}
END_TEST

START_TEST(test_clear_top_row) {
  params_t *prms = getGameInstance();

  for (int j = 0; j < BOARD_M; j++) {
    prms->game_info.field[0][j] = 1;
    prms->field_colors[0][j] = 1;
  }

  clear_top_row(prms);

  for (int j = 0; j < BOARD_M; j++) {
    ck_assert_int_eq(prms->game_info.field[0][j], 0);
    ck_assert_int_eq(prms->field_colors[0][j], 0);
  }
  cleanupGameInstance(prms);
}
END_TEST

Suite *line_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Lines");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_shift_lines_down);
  tcase_add_test(tc_core, test_clear_top_row);
  suite_add_tcase(s, tc_core);

  return s;
}

Suite *score_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Score");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_update_score_1_line);
  tcase_add_test(tc_core, test_update_score_2_lines);
  tcase_add_test(tc_core, test_update_score_3_lines);
  tcase_add_test(tc_core, test_update_score_4_lines);
  tcase_add_test(tc_core, test_update_score_invalid_lines);
  suite_add_tcase(s, tc_core);

  return s;
}

Suite *rotation_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Rotation");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_unrotate_correctness);
  tcase_add_test(tc_core, test_unrotate_no_rotation);
  suite_add_tcase(s, tc_core);

  return s;
}

Suite *movement_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Movement");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_drop_down);
  tcase_add_test(tc_core, test_moveleft_no_collision);
  tcase_add_test(tc_core, test_moveleft_with_collision);
  tcase_add_test(tc_core, test_moveright_no_collision);
  tcase_add_test(tc_core, test_moveright_with_collision);
  suite_add_tcase(s, tc_core);

  return s;
}

Suite *game_info_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Game Info");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_update_high_score_and_level);
  suite_add_tcase(s, tc_core);

  return s;
}

Suite *cleanup_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("CleanupGameInstance");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_cleanup_game_instance);
  tcase_add_test(tc_core, test_double_cleanup);

  suite_add_tcase(s, tc_core);

  return s;
}

Suite *is_full_line_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("IsFullLine");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_full_line);
  tcase_add_test(tc_core, test_partial_line);
  tcase_add_test(tc_core, test_empty_line);

  suite_add_tcase(s, tc_core);

  return s;
}

Suite *rotate_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Rotate");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_rotate_no_collision);
  tcase_add_test(tc_core, test_rotate_skip_for_template_1);

  suite_add_tcase(s, tc_core);

  return s;
}

Suite *high_score_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("High Score");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_load_high_score);

  suite_add_tcase(s, tc_core);

  return s;
}

Suite *tetris_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Tetris");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_no_collision);
  tcase_add_test(tc_core, test_collision_edge);
  tcase_add_test(tc_core, test_collision_field);
  suite_add_tcase(s, tc_core);

  return s;
}

Suite *attach_suite(void) {
  Suite *s1;
  TCase *tc_core;

  s1 = suite_create("Attach");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_attach);
  suite_add_tcase(s1, tc_core);

  return s1;
}

Suite *spawn_suite(void) {
  Suite *s2;
  TCase *tc_core;

  s2 = suite_create("Spawn");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_spawn_template_number_range);
  tcase_add_test(tc_core, test_spawn_random_template);
  suite_add_tcase(s2, tc_core);

  return s2;
}

int main(void) {
  printf("qwe");
  int number_failed;
  Suite *s;
  Suite *s1;
  Suite *s2;
  Suite *s3;
  Suite *s4;
  Suite *s5;
  Suite *s6;
  Suite *s7;
  Suite *s8;
  Suite *s9;
  Suite *s10;
  Suite *s11;

  SRunner *sr;

  s = tetris_suite();
  s1 = attach_suite();
  s2 = spawn_suite();
  s3 = high_score_suite();
  s4 = rotate_suite();
  s5 = is_full_line_suite();
  s6 = cleanup_suite();
  s7 = game_info_suite();
  s8 = movement_suite();
  s9 = rotation_suite();
  s10 = score_suite();
  s11 = line_suite();

  sr = srunner_create(s);
  srunner_add_suite(sr, s1);
  srunner_add_suite(sr, s2);
  srunner_add_suite(sr, s3);
  srunner_add_suite(sr, s4);
  srunner_add_suite(sr, s5);
  srunner_add_suite(sr, s6);
  srunner_add_suite(sr, s7);
  srunner_add_suite(sr, s8);
  srunner_add_suite(sr, s9);
  srunner_add_suite(sr, s10);
  srunner_add_suite(sr, s11);

  srunner_run_all(sr, CK_NORMAL);

  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
