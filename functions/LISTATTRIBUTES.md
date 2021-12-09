---
title: LISTATTRIBUTES
permalink: /functions/listattributes
---

# LISTATTRIBUTES - Object

Returns a list of all member names in the given object

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `obj` | Object | False |  |

## Output

**Array[String]:**

## Usage

```joda
LISTATTRIBUTES(<obj>)
```

## Examples

### Example 1

#### Input Document
```json
{"a":1, "b":{"c": true}}
```


#### Query
```joda
LOAD tmp
AS ('': LISTATTRIBUTES(''))
```
#### Result
```json
["a","b"]
```


### Example 2

#### Input Document
```json
{"a":1, "b":{"c": true}}
```


#### Query
```joda
LOAD tmp
AS ('': LISTATTRIBUTES('/b'))
```
#### Result
```json
["c"]
```


