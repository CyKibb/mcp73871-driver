#include "mcp_bms.h"
#include <check.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Board Query Methods Check
uint8_t mcp738xx_query_stat1_state_false(void) { return 0; }
uint8_t mcp738xx_query_stat2_state_false(void) { return 0; }
uint8_t mcp738xx_query_pg_state_false(void) { return 0; }
uint8_t mcp738xx_query_stat1_state_true(void) { return 1; }
uint8_t mcp738xx_query_stat2_state_true(void) { return 1; }
uint8_t mcp738xx_query_pg_state_true(void) { return 1; }
float mcp738xx_query_over_voltage(void) { return 4.25; }
float mcp738xx_query_charged_voltage(void) { return 4.21; }
float mcp738xx_query_low_voltage(void) { return 3.1; }
float mcp738xx_query_under_voltage(void) { return 2.8; }
float mcp738xx_query_nominal_voltage(void) { return 3.6; }

START_TEST(test_mcp738xx_ctor) {
  mcp738xx_driver_s *mcp738xx_driver = mcp738xx_ctor(
      LITHIUM_ION_POLYMER, 1000, 1, mcp738xx_query_nominal_voltage,
      mcp738xx_query_stat1_state_false, mcp738xx_query_stat2_state_false,
      mcp738xx_query_pg_state_false);

  ck_assert_ptr_ne(mcp738xx_driver, NULL);
  ck_assert_int_eq(mcp738xx_get_battery_type(mcp738xx_driver),
                   LITHIUM_ION_POLYMER);
  ck_assert_int_eq(mcp738xx_get_batt_capacity(mcp738xx_driver), 1000);
  ck_assert_int_eq(mcp738xx_get_cell_num(mcp738xx_driver), 1);
  ck_assert_float_eq(mcp738xx_get_voltage(mcp738xx_driver), 3.6);
  ck_assert_int_eq(mcp738xx_get_pg_state(mcp738xx_driver), 0);
  ck_assert_int_eq(mcp738xx_get_stat1_state(mcp738xx_driver), 0);
  ck_assert_int_eq(mcp738xx_get_stat2_state(mcp738xx_driver), 0);
  ck_assert_float_eq(mcp738xx_get_voltage_limits(mcp738xx_driver).max_voltage,
                     4.2);
  ck_assert_float_eq(mcp738xx_get_voltage_limits(mcp738xx_driver).min_voltage,
                     3.0);
  ck_assert_int_eq(mcp738xx_get_bms_state(mcp738xx_driver).mcp_bms_state,
                   TEMPERATURE_TIMER_FAULT);
  // cleanup
  mcp738xx_free_data(mcp738xx_driver);
  mcp738xx_driver = NULL;
}
END_TEST

// stat1stat2pg
START_TEST(test_mcp738xx_get_state_methods) {
  mcp738xx_driver_s *mcp738xx_driver = mcp738xx_ctor(
      LITHIUM_ION_POLYMER, 1000, 1, mcp738xx_query_nominal_voltage,
      mcp738xx_query_stat1_state_false, mcp738xx_query_stat2_state_false,
      mcp738xx_query_pg_state_true);
  ck_assert_ptr_ne(mcp738xx_driver, NULL);
  ck_assert_int_eq(mcp738xx_get_bms_state(mcp738xx_driver).mcp_bms_state,
                   CHARGING_INPUT_PRESENT);
  ck_assert_int_eq(
      mcp738xx_get_bms_state(mcp738xx_driver).battery_voltage_state,
      BATTERY_NOMINAL_VOLTAGE);
  mcp738xx_free_data(mcp738xx_driver);
  mcp738xx_driver = NULL;

  mcp738xx_driver = mcp738xx_ctor(
      LITHIUM_ION_POLYMER, 1000, 1, mcp738xx_query_nominal_voltage,
      mcp738xx_query_stat1_state_false, mcp738xx_query_stat2_state_true,
      mcp738xx_query_pg_state_false);
  ck_assert_ptr_ne(mcp738xx_driver, NULL);
  ck_assert_int_eq(mcp738xx_get_bms_state(mcp738xx_driver).mcp_bms_state,
                   CHARGING_IN_PROGRESS);
  ck_assert_int_eq(
      mcp738xx_get_bms_state(mcp738xx_driver).battery_voltage_state,
      BATTERY_NOMINAL_VOLTAGE);
  mcp738xx_free_data(mcp738xx_driver);
  mcp738xx_driver = NULL;

  mcp738xx_driver = mcp738xx_ctor(
      LITHIUM_ION_POLYMER, 1000, 1, mcp738xx_query_low_voltage,
      mcp738xx_query_stat1_state_false, mcp738xx_query_stat2_state_true,
      mcp738xx_query_pg_state_true);
  ck_assert_ptr_ne(mcp738xx_driver, NULL);
  ck_assert_int_eq(mcp738xx_get_bms_state(mcp738xx_driver).mcp_bms_state,
                   LOW_BATTERY_OUTPUT);
  ck_assert_int_eq(
      mcp738xx_get_bms_state(mcp738xx_driver).battery_voltage_state,
      BATTERY_NOMINAL_VOLTAGE);
  mcp738xx_free_data(mcp738xx_driver);
  mcp738xx_driver = NULL;

  mcp738xx_driver = mcp738xx_ctor(
      LITHIUM_ION_POLYMER, 1000, 1, mcp738xx_query_charged_voltage,
      mcp738xx_query_stat1_state_true, mcp738xx_query_stat2_state_false,
      mcp738xx_query_pg_state_false);
  ck_assert_ptr_ne(mcp738xx_driver, NULL);
  ck_assert_int_eq(mcp738xx_get_bms_state(mcp738xx_driver).mcp_bms_state,
                   CHARGE_COMPLETE_STANDBY);
  ck_assert_int_eq(
      mcp738xx_get_bms_state(mcp738xx_driver).battery_voltage_state,
      BATTERY_NOMINAL_VOLTAGE);
  mcp738xx_free_data(mcp738xx_driver);
  mcp738xx_driver = NULL;

  mcp738xx_driver = mcp738xx_ctor(
      LITHIUM_ION_POLYMER, 1000, 1, mcp738xx_query_nominal_voltage,
      mcp738xx_query_stat1_state_true, mcp738xx_query_stat2_state_false,
      mcp738xx_query_pg_state_true);
  ck_assert_ptr_eq(mcp738xx_driver, NULL);

  mcp738xx_driver = mcp738xx_ctor(
      LITHIUM_ION_POLYMER, 1000, 1, mcp738xx_query_nominal_voltage,
      mcp738xx_query_stat1_state_true, mcp738xx_query_stat2_state_true,
      mcp738xx_query_pg_state_false);
  ck_assert_ptr_eq(mcp738xx_driver, NULL);
}
END_TEST

Suite *str_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("MCP738XX Driver Tests");

  /* Core test case */
  tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_mcp738xx_ctor);
  tcase_add_test(tc_core, test_mcp738xx_get_state_methods);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = str_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
