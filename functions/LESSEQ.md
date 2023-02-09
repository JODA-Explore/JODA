---
title: LESSEQ
permalink: /functions/lesseq
---

# LESSEQ - Comparison

Checks if `lhs` is less or equal than `rhs`.

## Details

This function can also be written as infix notation `<lhs> <= <rhs>`.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `lhs` | String/Number | False |  |
| `rhs` | String/Number | False |  |

## Output

**Bool:** True if `lhs` <= `rhs`

## Usage

```joda
LESSEQ(<lhs>, <rhs>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': LESSEQ(5, 2))
```
#### Result
```json
false
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': LESSEQ(5, 5))
```
#### Result
```json
true
```


### Example 3


#### Query
```joda
LOAD tmp
AS ('': LESSEQ("A", "B"))
```
#### Result
```json
true
```


