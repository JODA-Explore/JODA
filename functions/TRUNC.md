---
title: TRUNC
permalink: /functions/trunc
---

# TRUNC - Mathematical

Truncates the floating point number

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False |  |

## Output

**Number:** trunc(x)

## Usage

```joda
TRUNC(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': TRUNC(1.5))
```
#### Result
```json
1.0
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': TRUNC(1.1))
```
#### Result
```json
1.0
```


### Example 3


#### Query
```joda
LOAD tmp
AS ('': TRUNC(-1.9))
```
#### Result
```json
-1.0
```


