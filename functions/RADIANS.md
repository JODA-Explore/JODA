---
title: RADIANS
permalink: /functions/radians
---

# RADIANS - Mathematical

Converts the given degrees to radians

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False | Input degrees |

## Output

**Number:** radian

## Usage

```joda
RADIANS(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': RADIANS(10))
```
#### Result
```json
0.17453292519943296
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': RADIANS(45))
```
#### Result
```json
0.7853981633974483
```


