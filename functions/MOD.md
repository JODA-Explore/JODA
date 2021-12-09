---
title: MOD
permalink: /functions/mod
---

# MOD - Mathematical

Returns the remainder of the division of `x` by `y`

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False |  |
| `y` | Number | False |  |

## Output

**Number:** x % y

## Usage

```joda
MOD(<x>, <y>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': MOD(3, 2))
```
#### Result
```json
1
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': MOD(-40, 7))
```
#### Result
```json
-5
```


