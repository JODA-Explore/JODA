---
title: GREATEREQ
permalink: /functions/greatereq
---

# GREATEREQ - Comparison

Checks if `lhs` is greater or equal than `rhs`.

## Details

This function can also be written as infix notation `<lhs> >= <rhs>`.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `lhs` | String/Number | False |  |
| `rhs` | String/Number | False |  |

## Output

**Bool:** True if `lhs` >= `rhs`

## Usage

```joda
GREATEREQ(<lhs>, <rhs>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': GREATEREQ(5, 2))
```
#### Result
```json
true
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': GREATEREQ(5, 5))
```
#### Result
```json
true
```


### Example 3


#### Query
```joda
LOAD tmp
AS ('': GREATEREQ("A", "B"))
```
#### Result
```json
false
```


