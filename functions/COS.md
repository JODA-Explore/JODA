---
title: COS
permalink: /functions/cos
---

# COS - Mathematical

Calculates the cosine of the given number

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Number | False | Input Number |

## Output

**Number:** COS(x)

## Usage

```joda
COS(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': COS(90))
```
#### Result
```json
-0.4480736161291701
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': COS(-180))
```
#### Result
```json
-0.5984600690578581
```


