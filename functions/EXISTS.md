---
title: EXISTS
permalink: /functions/exists
---

# EXISTS - Type

Checks if the given attribute exists.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Any | False | The attribute to check for existence |

## Output

**Bool:** True if it exists, false otherwise

## Usage

```joda
EXISTS(<x>)
```

## Examples

### Example 1

#### Input Document
```json
{"doc": 1}
```


#### Query
```joda
LOAD tmp
AS ('': EXISTS('/doc'))
```
#### Result
```json
true
```


### Example 2

#### Input Document
```json
{"doc": 1}
```


#### Query
```joda
LOAD tmp
AS ('': EXISTS('/array'))
```
#### Result
```json
false
```


