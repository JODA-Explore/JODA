---
title: FLOAT
permalink: /functions/float
---

# FLOAT - Cast

Casts the given value to floating point

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Any | False | Value to convert to float |

## Output

**Number:** The floating point representation of the given value.
**Nothing:** if it can not be converted 

## Usage

```joda
FLOAT(<x>)
```

## Examples

### Example 1

#### Input Document
```json
1
-4
5.6
"  -123.3543  "
true
false
```


#### Query
```joda
LOAD tmp
AS ('': FLOAT(''))
```
#### Result
```json
1.0
-4.0
5.6
-123.3543
1.0
0.0
```


