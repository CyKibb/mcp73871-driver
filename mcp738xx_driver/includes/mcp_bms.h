#ifndef MCP_BMS_H_INCLUDED
#define MCP_BMS_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

#define BATTERY_VOLTAGE_MEAS_MARGIN 0.2f

typedef struct mcp738xx_driver_s mcp738xx_driver_s;

typedef enum {
  TEMPERATURE_TIMER_FAULT, // 000: L L L
  CHARGING_INPUT_PRESENT,  // 001: L L Hi-Z
  CHARGING_IN_PROGRESS,    // 010: L Hi-Z L (Precond Constant Current, Voltage)
  LOW_BATTERY_OUTPUT,      // 011: Hi-Z L L
  CHARGE_COMPLETE_STANDBY, // 100: Hi-Z L L
  UNKNOWN_STATE,           // 101: Not specified
  NO_BATTERY_PRESENT,      // 110: Hi-Z Hi-Z L
  NO_INPUT_POWER_PRESENT_SHUTDOWN, // 111: Hi-Z Hi-Z Hi-Z
  MCP_ERROR_STATE
} mcp73871_state_t;

typedef enum {
  BATTERY_UNDER_VOLTAGE,
  BATTERY_OVER_VOLTAGE,
  BATTERY_NOMINAL_VOLTAGE
} battery_voltage_state_t;

typedef struct {
  mcp73871_state_t mcp_bms_state;
  battery_voltage_state_t battery_voltage_state;
} bms_state_t;

typedef enum {
  LITHIUM_IRON_PHOSPHATE,               // LFP
  LITHIUM_NICKEL_MANGANESE_COBALT,      // NMC
  LITHIUM_NICKEL_COBALT_ALUMINUM_OXIDE, // NCA
  LITHIUM_ION_MANGANESE_OXIDE,          // LMO
  LITHIUM_ION_COBALT_OXIDE,             // LCO
  LITHIUM_TITANATE_OXIDE,               // LTO
  LITHIUM_ION_POLYMER                   // LiPo
} battery_type_t;

typedef struct {
  float max_voltage; // Maximum charging voltage per cell
  float min_voltage; // Minimum discharging voltage per cell
} voltage_limits_t;

// Public Class Functions
mcp738xx_driver_s *mcp738xx_ctor(battery_type_t batt_type,
                                 uint16_t batt_capacity, uint8_t num_cells,
                                 float (*mcp738xx_get_voltage)(void),
                                 uint8_t (*mcp738xx_get_stat1_state)(void),
                                 uint8_t (*mcp738xx_get_stat2_state)(void),
                                 uint8_t (*mcp738xx_get_pg_state)(void));

battery_type_t mcp738xx_get_battery_type(mcp738xx_driver_s *this);
voltage_limits_t mcp738xx_get_voltage_limits(mcp738xx_driver_s *this);
uint8_t mcp738xx_get_cell_num(mcp738xx_driver_s *this);
uint16_t mcp738xx_get_batt_capacity(mcp738xx_driver_s *this);
uint8_t mcp738xx_get_stat1_state(mcp738xx_driver_s *this);
uint8_t mcp738xx_get_stat2_state(mcp738xx_driver_s *this);
uint8_t mcp738xx_get_pg_state(mcp738xx_driver_s *this);
float mcp738xx_get_voltage(mcp738xx_driver_s *this);
bms_state_t mcp738xx_get_bms_state(mcp738xx_driver_s *this);
void mcp738xx_free_data(mcp738xx_driver_s *this);

#endif // MCP_BMS_H_INCLUDED