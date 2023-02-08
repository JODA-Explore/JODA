---
title: NOT
permalink: /functions/not
---

# NOT - Boolean Algebra

Negates the Boolean value

## Details

This function can also be written as `!<x>`.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Bool | False |  |

## Output

**Bool:** `!<x>`

## Usage

```joda
NOT(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': NOT(true))
```
#### Result
```json
false
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': NOT(false))
```
#### Result
```json
true
```


