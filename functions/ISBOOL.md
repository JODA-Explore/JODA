---
title: ISBOOL
permalink: /functions/isbool
---

# ISBOOL - Type

Checks wether the attribute is of type Bool

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Any | False |  |

## Output

**Bool:** True/False depending on check

## Usage

```joda
ISBOOL(<x>)
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
AS ('': ISBOOL(''))
```
#### Result
```json
false
false
false
false
true
false
```


