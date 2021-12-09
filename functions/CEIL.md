---
title: CEIL
permalink: /functions/ceil
---

# CEIL - Mathematical

Calculates the ceiling of the given (floating point) number

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False | Input Number |

## Output

**Number:** ⌈x⌉

## Usage

```joda
CEIL(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': CEIL(-0.9))
```
#### Result
```json
-0.0
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': CEIL(0.3))
```
#### Result
```json
1.0
```


