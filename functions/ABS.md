---
title: ABS
permalink: /functions/abs
---

# ABS - Mathematical

Calculates the absolute value of a number

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False | Input Number |

## Output

**Number:** |x|

## Usage

```joda
ABS(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': ABS(-25.5))
```
#### Result
```json
25.5
```


