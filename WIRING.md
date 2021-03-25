# Wiring

# Sensor connection

The MCU board has 5V and 3V power outputs marked in PCB silkscreen.

1. Connect sensors VCC and Debug pin on 3V power output.
2. Connect sensors data pin to MCP DATA PIN, which is configured in 
   `configuration/config_peripherals.h` file (pin A2 on MCU board).