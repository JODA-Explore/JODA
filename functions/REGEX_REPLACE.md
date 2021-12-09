---
title: REGEX_REPLACE
permalink: /functions/regex_replace
---

# REGEX_REPLACE - String

Replaces all matches of `regexp` in `str` with `replace`

## Details

Note: "\" have to be escaped in JODA strings

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str` | String | False | String in which to replace |
| `regexp` | String | False | Regular expression |
| `replace` | String | False | String with which to replace |

## Output

**String:** `str` with all matches replaced by `replace`

## Usage

```joda
REGEX_REPLACE(<str>, <regexp>, <replace>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': REGEX_REPLACE("Number: 345345 Other: 2343", "(\\d+)", "<number>"))
```
#### Result
```json
"Number: <number> Other: <number>"
```


