---
title: ROUND
permalink: /functions/round
---

# ROUND - Mathematical

Rounds the given (floating point) number to the nearest integer number

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False | Input Number |

## Output

**Number:** round(x)

## Usage

```joda
ROUND(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': ROUND(-0.9))
```
#### Result
```json
-1.0
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': ROUND(0.3))
```
#### Result
```json
0.0
```


