---
title: MEMCOUNT
permalink: /functions/memcount
---

# MEMCOUNT - Object

Returns the number of members in the object

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `obj` | Object | False |  |

## Output

**Number:** Number of attributes

## Usage

```joda
MEMCOUNT(<obj>)
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
AS ('': MEMCOUNT(''))
```
#### Result
```json
2
```


### Example 2

#### Input Document
```json
{"a":1, "b":{"c": true}}
```


#### Query
```joda
LOAD tmp
AS ('': MEMCOUNT('/b'))
```
#### Result
```json
1
```


