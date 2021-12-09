---
title: FINDSTR
permalink: /functions/findstr
---

# FINDSTR - String

Returns the position of the second string in the first string, or -1 if it is not contained.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str` | String | False | String to search in |
| `substr` | String | False | String to search for |

## Output

**Number:** offset of `substr` in `str` or -1 

## Usage

```joda
FINDSTR(<str>, <substr>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': FINDSTR("Hello World", "World"))
```
#### Result
```json
6
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': FINDSTR("Hello World", "John"))
```
#### Result
```json
-1
```


