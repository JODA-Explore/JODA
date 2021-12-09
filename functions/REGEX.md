---
title: REGEX
permalink: /functions/regex
---

# REGEX - String

Checks wether `str` matches the regular expression `regexp`

## Details

Note: "\" have to be escaped in JODA strings

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str` | String | False | String to match |
| `regexp` | String | False | Regular expression |

## Output

**Bool:** True if `str` matches `regexp`. false otherwise

## Usage

```joda
REGEX(<str>, <regexp>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': REGEX("dogsitter", "sitter$"))
```
#### Result
```json
true
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': REGEX("345345", "\\d+"))
```
#### Result
```json
true
```


