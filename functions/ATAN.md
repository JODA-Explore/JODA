---
title: ATAN
permalink: /functions/atan
---

# ATAN - Mathematical

Calculates the arctangent of the given number

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False | Input Number |

## Output

**Number: ATAN(x)**

## Usage

```joda
ATAN(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': ATAN(-0.9))
```
#### Result
```json
-0.7328151017865066
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': ATAN(0.3))
```
#### Result
```json
0.2914567944778671
```


