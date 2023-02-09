---
title: XOR
permalink: /functions/xor
---

# XOR - Boolean Algebra

Combines the two parameters with the Boolean exclusive or (XOR) operation.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `lhs` | Bool | False |  |
| `rhs` | Bool | False |  |

## Output

**Bool:** True if and only if one of the parameters is true.

## Usage

```joda
XOR(<lhs>, <rhs>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': XOR(true, false))
```
#### Result
```json
true
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': XOR(true, true))
```
#### Result
```json
false
```


