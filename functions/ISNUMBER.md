---
title: ISNUMBER
permalink: /functions/isnumber
---

# ISNUMBER - Type

Checks wether the attribute is of numerical type

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Any | False |  |

## Output

**Bool:** True/False depending on check

## Usage

```joda
ISNUMBER(<x>)
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
AS ('': ISNUMBER(''))
```
#### Result
```json
false
false
true
false
false
false
```


