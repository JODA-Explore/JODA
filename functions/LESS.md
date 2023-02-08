---
title: LESS
permalink: /functions/less
---

# LESS - Comparison

Checks if `lhs` is less than `rhs`.

## Details

This function can also be written as infix notation `<lhs> < <rhs>`.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `lhs` | String/Number | False |  |
| `rhs` | String/Number | False |  |

## Output

**Bool:** True if `lhs` < `rhs`

## Usage

```joda
LESS(<lhs>, <rhs>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': LESS(5, 2))
```
#### Result
```json
false
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': LESS(5, 5))
```
#### Result
```json
false
```


### Example 3


#### Query
```joda
LOAD tmp
AS ('': LESS("A", "B"))
```
#### Result
```json
true
```


