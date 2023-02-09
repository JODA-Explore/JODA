---
title: REGEX_EXTRACT_FIRST
permalink: /functions/regex_extract_first
---

# REGEX_EXTRACT_FIRST - String

Matches first `regexp` in `str` and returns it.

## Details

Note: "\\" have to be escaped in JODA strings

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str` | String | False | String to match |
| `regexp` | String | False | Regular expression |

## Output

**String:** The first match, or `null` if none was found

## Usage

```joda
REGEX_EXTRACT_FIRST(<str>, <regexp>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': REGEX_EXTRACT_FIRST("Number: 345345 Other: 2343", "(\\d+)"))
```
#### Result
```json
"345345"
```


