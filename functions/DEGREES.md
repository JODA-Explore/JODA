---
title: DEGREES
permalink: /functions/degrees
---

# DEGREES - Mathematical

Converts the given radians to degrees

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False | Input radians |

## Output

**Number:** x * (180/PI())

## Usage

```joda
DEGREES(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': DEGREES(0.4))
```
#### Result
```json
22.91831180523293
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': DEGREES(0.8))
```
#### Result
```json
45.83662361046586
```


