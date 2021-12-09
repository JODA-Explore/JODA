---
title: INT
permalink: /functions/int
---

# INT - Cast

Casts/Parses the given value to integer

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Any | False | Value to convert to integer |

## Output

**Number:** The integer point representation of the given value.
**Nothing:** if it can not be converted.

## Usage

```joda
INT(<x>)
```

## Examples

### Example 1

#### Input Document
```json
1
5.3
5.6
"  -123  "
true
false
```


#### Query
```joda
LOAD tmp
AS ('': INT(''))
```
#### Result
```json
1
5
5
-123
1
0
```


