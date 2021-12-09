---
title: LOWER
permalink: /functions/lower
---

# LOWER - String

Converts the given string to lower case

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str` | String | False |  |

## Output

**String:** UTF-8 lower-case representation of string

## Usage

```joda
LOWER(<str>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': LOWER("  StRiNg  "))
```
#### Result
```json
"  string  "
```


