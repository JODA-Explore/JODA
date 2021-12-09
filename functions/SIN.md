---
title: SIN
permalink: /functions/sin
---

# SIN - Mathematical

Calculates the sine of the given number

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False | Input Number |

## Output

**Number:** SIN(x)

## Usage

```joda
SIN(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': SIN(90))
```
#### Result
```json
0.8939966636005579
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': SIN(-180))
```
#### Result
```json
0.8011526357338304
```


