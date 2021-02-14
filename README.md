# Manchester Code Proxy

## Introduction

Manchester Code Proxy or MCP is an USB device with two USB endpoints:
- CDC - USB CDC communications device class can  be used to emulate a serial port providing a virtualCOM port UART interface.
- MSC - USB MSC mass storage device class that makes a USB device accessible to a host computing device and enables file transfers between the host and the USB device.

On other side of proxy there is a single wire data interface. It uses a 3-state digital pin which works in bidirectional mode. The data is encoded using Manchester code. The following Manchester coding conventions are supported:
- IEEE802.3
- IEEE802.4
- G.E. Thomas