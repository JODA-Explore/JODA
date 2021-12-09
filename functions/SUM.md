---
title: SUM
permalink: /functions/sum
---

# SUM - Mathematical

Calculates the sum of `x` and `y`

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False |  |
| `y` | Number | False |  |

## Output

**Number:** x + y

## Usage

```joda
SUM(<x>, <y>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': SUM(1, 2))
```
#### Result
```json
3
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': SUM(-5, 2))
```
#### Result
```json
-3
```


### Example 3


#### Query
```joda
LOAD tmp
AS ('': SUM(2.5, 1))
```
#### Result
```json
3.5
```


