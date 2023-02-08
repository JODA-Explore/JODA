---
title: UNEQUAL
permalink: /functions/unequal
---

# UNEQUAL - Comparison

Checks if the given parameters do not have the same values.

## Details

This function can also be written as infix notation `<lhs> != <rhs>`.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `lhs` | String/Number/Bool/Object/Array | False |  |
| `rhs` | String/Number/Bool/Object/Array | False |  |

## Output

**Bool:** True if the parameters doe not have the same values

## Usage

```joda
UNEQUAL(<lhs>, <rhs>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': UNEQUAL(true, true))
```
#### Result
```json
false
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': UNEQUAL(5, 2))
```
#### Result
```json
true
```


