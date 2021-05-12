
# List of issues

# 1. Backspace button in serial terminal doesn't work

When pressing backspace button no characters are deleted from terminal.

Expected behaviour:
When the button is pressed the last entered character should be deleted.

# 2. Setting environment variables doesn't work

This functionality is still missing in serial console interpreter.

# 3. Getting environment variables doesn't work

This functionality is still missing in serial console interpreter.

# 4. Right after startup, MCP receives some weird characters although I am not typing

This is an issue internal to STM32 USB software stack. When the USB hardware is
initialized PC host may send data packets which are interpreted as data payload.

Workaround is to wait a few seconds until PC host finishes with USB device
negotiation. Since the MCP internal buffer will be field with this data, the
first command typed will be invalid. Execute a command like 'help' to clear
internal data buffers and then continue normally.

# 5. When reading and if last data bit is set the manchester decoder will complain

It was found that the sensor itself will not transmitt the last data bit in
appropriate manner. It seems that the sensor will change it's pin mode from 
output to input mode before last bit is sent. This issue is manifested only 
when last bit is '1'. When last bit is '0' it will not be seen since the sensor
pull-up resistor will set appropriate logic level.

This sensor behaviour was captured on oscilloscope and saved to 
``docs/images/last_bit_is_not_set.BMP``.
