---
title: PROD
permalink: /functions/prod
---

# PROD - Mathematical

Multiplies `x` with `y`

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False |  |
| `y` | Number | False |  |

## Output

**Number:** x * y

## Usage

```joda
PROD(<x>, <y>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': PROD(2, 4))
```
#### Result
```json
8
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': PROD(-3, 2))
```
#### Result
```json
-6
```


### Example 3


#### Query
```joda
LOAD tmp
AS ('': PROD(0.1, 400))
```
#### Result
```json
40.0
```


