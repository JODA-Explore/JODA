---
title: ASIN
permalink: /functions/asin
---

# ASIN - Mathematical

Calculates the arcsine of the given number

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False | Input Number in the range [-1,1] |

## Output

**Number: ASIN(x)**

## Usage

```joda
ASIN(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': ASIN(-0.9))
```
#### Result
```json
-1.1197695149986343
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': ASIN(0.3))
```
#### Result
```json
0.3046926540153975
```


