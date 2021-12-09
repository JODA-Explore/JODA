---
title: RTRIM
permalink: /functions/rtrim
---

# RTRIM - String

Trims all whitespace to the right of the string

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str` | String | False |  |

## Output

**String:** `str` without whitespace to the right

## Usage

```joda
RTRIM(<str>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': RTRIM("  StRiNg  "))
```
#### Result
```json
"  StRiNg"
```


