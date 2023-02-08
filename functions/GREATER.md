---
title: GREATER
permalink: /functions/greater
---

# GREATER - Comparison

Checks if `lhs` is greater than `rhs`.

## Details

This function can also be written as infix notation `<lhs> > <rhs>`.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `lhs` | String/Number | False |  |
| `rhs` | String/Number | False |  |

## Output

**Bool:** True if `lhs` > `rhs`

## Usage

```joda
GREATER(<lhs>, <rhs>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': GREATER(5, 2))
```
#### Result
```json
true
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': GREATER(5, 5))
```
#### Result
```json
false
```


### Example 3


#### Query
```joda
LOAD tmp
AS ('': GREATER("A", "B"))
```
#### Result
```json
false
```


