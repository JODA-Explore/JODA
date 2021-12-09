---
title: ISOBJECT
permalink: /functions/isobject
---

# ISOBJECT - Type

Checks wether the attribute is of type object

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Any | False |  |

## Output

**Bool:** True/False depending on check

## Usage

```joda
ISOBJECT(<x>)
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
AS ('': ISOBJECT(''))
```
#### Result
```json
false
true
false
false
false
false
```


