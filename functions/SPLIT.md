---
title: SPLIT
permalink: /functions/split
---

# SPLIT - String

Splits a given string by the supplied delimiter

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str` | String | False | String to split |
| `delimiter` | String | False | Delimiter to split by |

## Output

**Array[String]:** The split strings

## Usage

```joda
SPLIT(<str>, <delimiter>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': SPLIT("Hello World", " "))
```
#### Result
```json
["Hello","World"]
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': SPLIT("5>=4>=2", ">="))
```
#### Result
```json
["5","4","2"]
```


