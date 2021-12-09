---
title: SQRT
permalink: /functions/sqrt
---

# SQRT - Mathematical

Calculates the square root of `x`

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False |  |

## Output

**Number:** √x

## Usage

```joda
SQRT(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': SQRT(16))
```
#### Result
```json
4.0
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': SQRT(42))
```
#### Result
```json
6.48074069840786
```


