---
title: POW
permalink: /functions/pow
---

# POW - Mathematical

Calculates `x` to the power of `y`

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False |  |
| `y` | Number | False |  |

## Output

**Number:** x ^ y

## Usage

```joda
POW(<x>, <y>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': POW(2, 4))
```
#### Result
```json
16.0
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': POW(3, -1))
```
#### Result
```json
0.3333333333333333
```


