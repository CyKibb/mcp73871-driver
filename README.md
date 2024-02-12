# Device Battery Management Controller
Library containing application specific code for battery management charge controller


## Battery Management States
```
  TEMPERATURE_TIMER_FAULT, // 000: L L L
  CHARGING_INPUT_PRESENT,  // 001: L L Hi-Z
  CHARGING_IN_PROGRESS,    // 010: L Hi-Z L (Precond Constant Current, Voltage)
  LOW_BATTERY_OUTPUT,      // 011: Hi-Z L L
  CHARGE_COMPLETE_STANDBY, // 100: Hi-Z L L
  UNKNOWN_STATE,           // 101: Not specified
  NO_BATTERY_PRESENT,      // 110: Hi-Z Hi-Z L
  NO_INPUT_POWER_PRESENT_SHUTDOWN, // 111: Hi-Z Hi-Z Hi-Z
  MCP_ERROR_STATE
```

## MCP73871 Charge Controller
The MCP73871 device is a fully integrated linear solution for system load sharing and Li-Ion/Li-Polymer battery charge management with AC-DC wall adapter and USB port power sources selection. It is also capable of autonomous power source selection between input and battery. Along with its small physical size, the low number of required external components makes the device ideally suited for portable applications.

The MCP73871 device employs a constant current/constant voltage (CC/CV) charge algorithm with selectable charge termination point. To accommodate new and emerging battery charging requirements, the constant voltage regulation is fixed with four available options: 4.10V, 4.20V, 4.35V or 4.40V. The MCP73871 device also limits the charge current based on the die temperature during high power or high ambient conditions. This thermal regulation optimizes the charge cycle time while maintaining device reliability.

The MCP73871 device includes a low battery indicator, a power good indicator and two charge status indicators that allow for outputs with LEDs or communication with host microcontrollers. The MCP73871 device is fully specified over the ambient temperature range of -40°C to +85°C.


Below is a table representation of the MCP73871 state map based on the STAT1, STAT2, and PG pin states. This table includes the binary encoding of the pin states and the corresponding charge cycle state.

| STAT1 | STAT2 | PG   | Binary Encoding | Charge Cycle State         |
|-------|-------|------|-----------------|----------------------------|
| L     | L     | L    | 000             | Temperature Fault*         |
| L     | Hi-Z  | Hi-Z | 001             | Low Battery Output         |
| L     | Hi-Z  | L    | 010             | Preconditioning**          |
| Hi-Z  | L     | L    | 011             | Charge Complete - Standby  |
| -     | -     | -    | 100             | UNKNOWN_STATE              |
| -     | -     | -    | 101             | UNKNOWN_STATE              |
| Hi-Z  | Hi-Z  | L    | 110             | Shutdown (VDD = IN)***     |
| Hi-Z  | Hi-Z  | Hi-Z | 111             | Shutdown (VDD = VBAT)****  |

```
* Also represents Timer Fault due to identical pin states.
** Also represents Constant Current and Constant Voltage due to identical pin states.
*** Also interpreted as Shutdown (CE = L) and No Battery Present due to identical pin states.
**** Also interpreted as No Input Power Present due to identical pin states.
```

This table summarizes how the MCP73871's charge cycle state can be determined by reading the levels of the STAT1, STAT2, and PG pins, encoded in binary for ease of interpretation. The states marked as `UNKNOWN_STATE` represent binary combinations not explicitly defined in the provided state table, and `Hi-Z` (High Impedance) is treated as a logical high (`H` or `1`) in this context for simplicity.

Please note, the asterisks (*) indicate footnotes explaining nuances like overlapping states due to identical pin states, which are critical for understanding the limitations of this direct mapping approach.
