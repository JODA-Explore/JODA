---
title: STARTSWITH
permalink: /functions/startswith
---

# STARTSWITH - String

Checks whether `str` starts with `substr`

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str` | String | False |  |
| `substr` | String | False |  |

## Output

**Bool:** True if `str` starts with `substr`

## Usage

```joda
STARTSWITH(<str>, <substr>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': STARTSWITH("dogsitter", "dog"))
```
#### Result
```json
true
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': STARTSWITH("dogsitter", "sitter"))
```
#### Result
```json
false
```


