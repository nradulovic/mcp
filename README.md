# Manchester Code Proxy

## Introduction

Manchester Code Proxy or MCP is an USB device with two USB endpoints:
- CDC - USB CDC communications device class can  be used to emulate a serial 
  port providing a virtualCOM port UART interface.
- MSC - USB MSC mass storage device class that makes a USB device accessible to 
  a host computing device and enables file transfers between the host and the 
  USB device.

On other side of proxy there is a single wire data interface. It uses a 3-state
digital pin which works in bidirectional mode. The data is encoded using 
Manchester code. The following Manchester coding conventions are supported:
- IEEE802.3
- IEEE802.4
- G.E. Thomas

## Getting started

For current list of issues see [ISSUES.md](ISSUES.md).
For list of serial console commands refer to [COMMANDS.md](COMMANDS.md). 

### Prerequisites

* STM32CubeIDE
* Development board STM32F411E-DISCO

### Importing project

1. Choose `File` -> `Import...`
2. In Import window select `General` -> `Existing Projects into Workspace`, click `Next`
3. Choose `Select root directory` then click on `Browse...` button
4. Navigate to downloaded project folder and click `Open`
5. In `Projects:` text box select `mcp` project
6. Make sure that Option `Copy projects into workspace` is checked, too
7. Click `Finish`

### Refreshing index for code highlighting and browsing

After importing the project `mcp` should appear in `Project Explorer` window.

Right click on project name and execute the following options:
1. `Refresh`
2. `Index` -> `Rebuild`
3. `Index` -> `Freshen all files`

These commands will rebuild Eclipse indexer which would make code highlighting 
and code browsing.

### Building

Right click on project name and execute `Build project`.

When the project building is done you should get something like the following:

```
Finished building target: mcp.elf
 
arm-none-eabi-objdump -h -S  mcp.elf  > "mcp.list"
arm-none-eabi-objcopy  -O binary  mcp.elf  "mcp.bin"
arm-none-eabi-size   mcp.elf 
   text	   data	    bss	    dec	    hex	filename
  22664	    520	  42368	  65552	  10010	mcp.elf
Finished building: default.size.stdout
 
Finished building: mcp.bin
 
Finished building: mcp.list
```

### Debugging

1. In the menu click `Run` -> `Debug configuration...`. 
2. In new window double click on `STM32 Cortex-M C/C++ Application`. This will 
   create a new item which has appeared just below 
   `STM32 Cortex-M C/C++ Application` called `mcp Debug`.
3. Click on `Debug` button. The ST-LINK LED should start blinking on the board.
4. Click `Run` -> `Resume` to run the program.


