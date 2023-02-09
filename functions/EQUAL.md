---
title: EQUAL
permalink: /functions/equal
---

# EQUAL - Comparison

Checks if the given parameters have the same values.

## Details

This function can also be written as infix notation `<lhs> == <rhs>`.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `lhs` | String/Number/Bool/Object/Array | False |  |
| `rhs` | String/Number/Bool/Object/Array | False |  |

## Output

**Bool:** True if the parameters have the same values

## Usage

```joda
EQUAL(<lhs>, <rhs>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': EQUAL(true, true))
```
#### Result
```json
true
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': EQUAL(5, 2))
```
#### Result
```json
false
```


