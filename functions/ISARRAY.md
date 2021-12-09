---
title: ISARRAY
permalink: /functions/isarray
---

# ISARRAY - Type

Checks wether the attribute is of type array

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Any | False |  |

## Output

**Bool:** True/False depending on check

## Usage

```joda
ISARRAY(<x>)
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
AS ('': ISARRAY(''))
```
#### Result
```json
true
false
false
false
false
false
```


