---
title: LTRIM
permalink: /functions/ltrim
---

# LTRIM - String

Trims all whitespace to the left of the string

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str` | String | False |  |

## Output

**String:** `str` without whitespace to the left

## Usage

```joda
LTRIM(<str>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': LTRIM("  StRiNg  "))
```
#### Result
```json
"StRiNg  "
```


