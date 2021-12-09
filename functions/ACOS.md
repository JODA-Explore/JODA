---
title: ACOS
permalink: /functions/acos
---

# ACOS - Mathematical

Calculates the arccosine of the given number

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False | Input Number in the range [-1,1] |

## Output

**Number:** ACOS(x)

## Usage

```joda
ACOS(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': ACOS(-0.9))
```
#### Result
```json
2.6905658417935309
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': ACOS(0.3))
```
#### Result
```json
1.2661036727794993
```


