---
title: AND
permalink: /functions/and
---

# AND - Boolean Algebra

Combines the two parameters with the Boolean AND operation.

## Details

This function can also be written as infix notation `<lhs> && <rhs>`.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `lhs` | Bool | False |  |
| `rhs` | Bool | False |  |

## Output

**Bool:** `<lhs> && <rhs>`

## Usage

```joda
AND(<lhs>, <rhs>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': AND(true, false))
```
#### Result
```json
false
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': AND(true, true))
```
#### Result
```json
true
```


