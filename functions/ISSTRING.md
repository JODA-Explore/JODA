---
title: ISSTRING
permalink: /functions/isstring
---

# ISSTRING - Type

Checks whether the attribute is of type string

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Any | False |  |

## Output

**Bool:** True/False depending on check

## Usage

```joda
ISSTRING(<x>)
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
AS ('': ISSTRING(''))
```
#### Result
```json
false
false
false
true
false
false
```


