
# Command list

## Command `help`

Prints the list of available commands

This command does not accept any argument.

## Command `set`

Set a value in MCP configuration

```
set <key> <value>
```

- `key` - Key name which needs to be set.
- `value` - A value which will be associated with the key

## Command `get`

Get a value from MCP configuration

```
get <key>
```

- `key` - Key name which needs to be get.

## Command `rxchg`

Raw Exchange Data command

```
rxchg <data> <wr_bit_length> <rd_bit_length>
```

- `data` - Data array of 8-bit values written in HEX notation.
- `wr_bit_length` - Number of bits to actually write in DEC notation.
- `rd_bit_length` - Number of bits to actually read in DEC notation.


