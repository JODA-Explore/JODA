---
title: IMPLICATION
permalink: /functions/implication
---

# IMPLICATION - Boolean Algebra

Combines the two parameters with the Boolean implication (->) operation.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `lhs` | Bool | False |  |
| `rhs` | Bool | False |  |

## Output

**Bool:** `<lhs> -> <rhs>`

## Usage

```joda
IMPLICATION(<lhs>, <rhs>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': IMPLICATION(true, false))
```
#### Result
```json
false
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': IMPLICATION(true, true))
```
#### Result
```json
true
```


