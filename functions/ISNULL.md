---
title: ISNULL
permalink: /functions/isnull
---

# ISNULL - Type

Checks whether the attribute is of type null

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Any | False |  |

## Output

**Bool:** True/False depending on check

## Usage

```joda
ISNULL(<x>)
```

## Examples

### Example 1

#### Input Document
```json
[1,2]
{"doc":1}
1
"String"
true
null
```


#### Query
```joda
LOAD tmp
AS ('': ISNULL(''))
```
#### Result
```json
false
false
false
false
false
true
```


