#include "mcp_bms.h"
#include "mcp_bms_internal.h"

#include <stdio.h>

mcp738xx_driver_s *mcp738xx_ctor(battery_type_t batt_type,
                                 uint16_t batt_capacity, uint8_t num_cells,
                                 float (*mcp738xx_get_voltage)(void),
                                 uint8_t (*mcp738xx_get_stat1_state)(void),
                                 uint8_t (*mcp738xx_get_stat2_state)(void),
                                 uint8_t (*mcp738xx_get_pg_state)(void)) {

  mcp738xx_driver_s *mcp738xx_driver = malloc(sizeof(mcp738xx_driver_s));
  if (NULL == mcp738xx_driver) {
    return NULL;
  }

  return mcp738xx_initialize(mcp738xx_driver, batt_type, batt_capacity,
                             num_cells, mcp738xx_get_voltage,
                             mcp738xx_get_stat1_state, mcp738xx_get_stat2_state,
                             mcp738xx_get_pg_state);
}

mcp738xx_driver_s *
mcp738xx_initialize(mcp738xx_driver_s *this, battery_type_t batt_type,
                    uint16_t batt_capacity, uint8_t num_cells,
                    float (*mcp738xx_get_voltage)(void),
                    uint8_t (*mcp738xx_get_stat1_state)(void),
                    uint8_t (*mcp738xx_get_stat2_state)(void),
                    uint8_t (*mcp738xx_get_pg_state)(void)) {

  this->battery_type = batt_type;
  this->battery_capacity = batt_capacity;
  this->battery_cell_num = num_cells;
  this->mcp738xx_get_voltage = mcp738xx_get_voltage;
  this->mcp738xx_get_stat1_state = mcp738xx_get_stat1_state;
  this->mcp738xx_get_stat2_state = mcp738xx_get_stat2_state;
  this->mcp738xx_get_pg_state = mcp738xx_get_pg_state;
  this->battery_voltage_limits = mcp738xx_get_default_voltage_limits(batt_type);

  this->battery_voltage = this->mcp738xx_get_voltage();
  if (this->battery_voltage < 0.5) {
    mcp738xx_free_data(this);
    return NULL;
  }

  this->bms_state = mcp738xx_update_bms_state(this);
  if (this->bms_state.mcp_bms_state == NO_BATTERY_PRESENT ||
      this->bms_state.mcp_bms_state == UNKNOWN_STATE ||
      this->bms_state.mcp_bms_state == MCP_ERROR_STATE) {
    mcp738xx_free_data(this);
    return NULL;
  }

  return this;
}

battery_type_t mcp738xx_get_battery_type(mcp738xx_driver_s *this) {
  return this->battery_type;
}

voltage_limits_t mcp738xx_get_voltage_limits(mcp738xx_driver_s *this) {
  return this->battery_voltage_limits;
}

uint8_t mcp738xx_get_cell_num(mcp738xx_driver_s *this) {
  return this->battery_cell_num;
}

uint16_t mcp738xx_get_batt_capacity(mcp738xx_driver_s *this) {
  return this->battery_capacity;
}

uint8_t mcp738xx_get_stat1_state(mcp738xx_driver_s *this) {
  return this->mcp738xx_get_stat1_state();
}

uint8_t mcp738xx_get_stat2_state(mcp738xx_driver_s *this) {
  return this->mcp738xx_get_stat2_state();
}

uint8_t mcp738xx_get_pg_state(mcp738xx_driver_s *this) {
  return this->mcp738xx_get_pg_state();
}

float mcp738xx_get_voltage(mcp738xx_driver_s *this) {
  return this->mcp738xx_get_voltage();
}

bms_state_t mcp738xx_get_bms_state(mcp738xx_driver_s *this) {
  return mcp738xx_update_bms_state(this);
}

bms_state_t mcp738xx_update_bms_state(mcp738xx_driver_s *this) {

  // Query board levels to capture current hardware state
  this->bms_state.mcp_bms_state =
      (mcp73871_state_t)((mcp738xx_get_stat1_state(this) << 2) |
                         (mcp738xx_get_stat2_state(this) << 1) |
                         mcp738xx_get_pg_state(this));
  this->battery_voltage = this->mcp738xx_get_voltage();

  /* Add redundancy to bms state management to ensure alignment between measured
  voltage and MCP state */
  if (this->bms_state.mcp_bms_state == CHARGE_COMPLETE_STANDBY &&
      (this->battery_voltage <= (this->battery_voltage_limits.max_voltage -
                                 BATTERY_VOLTAGE_MEAS_MARGIN))) {
    this->bms_state.mcp_bms_state = MCP_ERROR_STATE;
  }
  if (this->bms_state.mcp_bms_state == LOW_BATTERY_OUTPUT &&
      (this->battery_voltage >= this->battery_voltage_limits.min_voltage +
                                    BATTERY_VOLTAGE_MEAS_MARGIN)) {
    this->bms_state.mcp_bms_state = MCP_ERROR_STATE;
  }

  // Check for battery voltage state
  if (this->battery_voltage <
      this->battery_voltage_limits.min_voltage - BATTERY_VOLTAGE_MEAS_MARGIN) {
    this->bms_state.battery_voltage_state = BATTERY_UNDER_VOLTAGE;
  } else if (this->battery_voltage > this->battery_voltage_limits.max_voltage +
                                         BATTERY_VOLTAGE_MEAS_MARGIN) {
    this->bms_state.battery_voltage_state = BATTERY_OVER_VOLTAGE;
  } else {
    this->bms_state.battery_voltage_state = BATTERY_NOMINAL_VOLTAGE;
  }

  return this->bms_state;
}

static voltage_limits_t
mcp738xx_get_default_voltage_limits(battery_type_t battery_type) {
  switch (battery_type) {
  case LITHIUM_IRON_PHOSPHATE:
    return (voltage_limits_t){3.65, 2.5};
  case LITHIUM_NICKEL_MANGANESE_COBALT:
    return (voltage_limits_t){4.2, 2.5};
  case LITHIUM_NICKEL_COBALT_ALUMINUM_OXIDE:
    return (voltage_limits_t){4.2, 3.0};
  case LITHIUM_ION_MANGANESE_OXIDE:
    return (voltage_limits_t){4.2, 2.5};
  case LITHIUM_ION_COBALT_OXIDE:
    return (voltage_limits_t){4.2, 3.0};
  case LITHIUM_TITANATE_OXIDE:
    return (voltage_limits_t){3.0, 1.8};
  case LITHIUM_ION_POLYMER:
    return (voltage_limits_t){4.2, 3.0};
  default:
    return (voltage_limits_t){0.0, 0.0};
  }
}

// Kill Object
void mcp738xx_free_data(mcp738xx_driver_s *this) { free(this); }