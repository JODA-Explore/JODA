---
title: UPPER
permalink: /functions/upper
---

# UPPER - String

Converts the given string to upper case

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str` | String | False |  |

## Output

**String:** UTF-8 upper-case representation of string

## Usage

```joda
UPPER(<str>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': UPPER("  StRiNg  "))
```
#### Result
```json
"  STRING  "
```


