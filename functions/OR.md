---
title: OR
permalink: /functions/or
---

# OR - Boolean Algebra

Combines the two parameters with the Boolean OR operation.

## Details

This function can also be written as infix notation `<lhs> || <rhs>`.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `lhs` | Bool | False |  |
| `rhs` | Bool | False |  |

## Output

**Bool:** `<lhs> || <rhs>`

## Usage

```joda
OR(<lhs>, <rhs>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': OR(true, false))
```
#### Result
```json
true
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': OR(true, true))
```
#### Result
```json
true
```


