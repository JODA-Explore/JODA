---
title: TAN
permalink: /functions/tan
---

# TAN - Mathematical

Calculates the tangent of x

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False |  |

## Output

**Number:** tan(x)

## Usage

```joda
TAN(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': TAN(90))
```
#### Result
```json
-1.995200412208242
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': TAN(10))
```
#### Result
```json
0.6483608274590866
```


### Example 3


#### Query
```joda
LOAD tmp
AS ('': TAN(-15))
```
#### Result
```json
0.8559934009085187
```


