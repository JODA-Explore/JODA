---
title: ATAN2
permalink: /functions/atan2
---

# ATAN2 - Mathematical

Calculates the ATAN2 of the given number

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False |  |
| `y` | Number | False |  |

## Output

**Number:* ATAN2(x,y)*

## Usage

```joda
ATAN2(<x>, <y>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': ATAN2(1, 1))
```
#### Result
```json
0.7853981633974483
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': ATAN2(-1, -1))
```
#### Result
```json
-2.356194490192345
```


