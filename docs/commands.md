# Introduction

This document describes the commands available at the MCP terminal.

# Command list

- ``help`` - Prints the list of available commands
- ``set`` - Set a value in MCP configuration
- ``get`` - Get a value from MCP configuration
- ``write`` - Write data to anyCS
- ``read`` - Read data from anyCS
- ``xchg`` - Exchange data with anyCS

# Command syntax

```
<> - mandatory field
[] - optional field
| - OR operator
```

## Command 'help'

Print the list of available commands.

```
help [set|get|write|read|xchg]
```

## Command 'set'

```
set <variable name> <variable value>
```

## Command 'get'

```
get <variable name>
```

## Command 'write'

```
write <hex addr> <hex data>
```

## Command 'read'

```
read <hex addr> <data size>
```

## Command 'xchg'

```
xchg <out payload> <data size>
```

# Variables

- ``echo`` - Controls echoing of input characters. Allowable values are:
  - ``on`` - Enable echo
  - ``off`` - Disable echo
- ``bitrate`` - A value of kbits/s for communication.
