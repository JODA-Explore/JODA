---
title: SUB
permalink: /functions/sub
---

# SUB - Mathematical

Subtracts `x` from `y`

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False |  |
| `y` | Number | False |  |

## Output

**Number:** x - y

## Usage

```joda
SUB(<x>, <y>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': SUB(3, 1))
```
#### Result
```json
2
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': SUB(-5, 12.4))
```
#### Result
```json
-17.4
```


