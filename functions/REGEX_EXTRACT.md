---
title: REGEX_EXTRACT
permalink: /functions/regex_extract
---

# REGEX_EXTRACT - String

Matches all `regexp` in `str` and returns them.

## Details

Note: "\" have to be escaped in JODA strings

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str` | String | False | String to match |
| `regexp` | String | False | Regular expression |

## Output

**Array[String]:** A list of all matches

## Usage

```joda
REGEX_EXTRACT(<str>, <regexp>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': REGEX_EXTRACT("Number: 345345 Other: 2343", "(\\d+)"))
```
#### Result
```json
["345345","2343"]
```


