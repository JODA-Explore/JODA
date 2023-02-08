---
title: DIV
permalink: /functions/div
---

# DIV - Mathematical

Divides x by y

## Details

If both numbers are integers, integer division is used. Otherwise the normal floating point division is used

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False |  |
| `y` | Number | False |  |

## Output

**Number:** x/y

## Usage

```joda
DIV(<x>, <y>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': DIV(10, 3))
```
#### Result
```json
3
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': DIV(10.0, 3))
```
#### Result
```json
3.3333333333333335
```


