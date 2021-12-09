---
title: FLOOR
permalink: /functions/floor
---

# FLOOR - Mathematical

Calculates the floor of the given (floating point) number

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False | Input Number |

## Output

**Number:** ⌊x⌋

## Usage

```joda
FLOOR(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': FLOOR(-0.9))
```
#### Result
```json
-1.0
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': FLOOR(0.3))
```
#### Result
```json
0.0
```


