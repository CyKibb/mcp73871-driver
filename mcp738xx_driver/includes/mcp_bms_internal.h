#ifndef MCP_BMS_INTERNAL_H_INCLUDED
#define MCP_BMS_INTERNAL_H_INCLUDED

#include "mcp_bms.h"

struct mcp738xx_driver_s {
  bms_state_t bms_state;
  float battery_voltage;
  battery_type_t battery_type;
  uint16_t battery_capacity; // in mAh
  uint8_t battery_cell_num;
  voltage_limits_t battery_voltage_limits;
  float (*mcp738xx_get_voltage)(void);
  uint8_t (*mcp738xx_get_stat1_state)(void);
  uint8_t (*mcp738xx_get_stat2_state)(void);
  uint8_t (*mcp738xx_get_pg_state)(void);
};

// Private Class Functions
bms_state_t mcp738xx_update_bms_state(mcp738xx_driver_s *this);
mcp738xx_driver_s *
mcp738xx_initialize(mcp738xx_driver_s *this, battery_type_t batt_type,
                    uint16_t batt_capacity, uint8_t num_cells,
                    float (*mcp738xx_get_voltage)(void),
                    uint8_t (*mcp738xx_get_stat1_state)(void),
                    uint8_t (*mcp738xx_get_stat2_state)(void),
                    uint8_t (*mcp738xx_get_pg_state)(void));
static voltage_limits_t
mcp738xx_get_default_voltage_limits(battery_type_t battery_type);

#endif // BMS_SYSTEM_INTERNAL_H_INCLUDED