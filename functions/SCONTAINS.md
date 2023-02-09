---
title: SCONTAINS
permalink: /functions/scontains
---

# SCONTAINS - String

Checks whether `str` contains `substr`

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str` | String | False |  |
| `substr` | String | False |  |

## Output

**Bool:** True if `str` contains `substr`

## Usage

```joda
SCONTAINS(<str>, <substr>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': SCONTAINS("dogsitter", "dog"))
```
#### Result
```json
true
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': SCONTAINS("dogsitter", "cat"))
```
#### Result
```json
false
```


