---
title: SUBSTR
permalink: /functions/substr
---

# SUBSTR - String

Returns a substring of `str` from `start` with `end` characters.

## Details

If `end` is not supplied, the substring is returned until the end of `str`

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str` | String | False | String to extract from |
| `start` | Number | False | Start position to extract from |
| `length` | Number | True | Number of characters to extract (Default: end of `str`) |

## Output

**String:** `str`[`start`:`start`+`end`]

## Usage

```joda
SUBSTR(<str>, <start>, (<length>))
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': SUBSTR("Hello World", 6))
```
#### Result
```json
"World"
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': SUBSTR("Hello World", 3, 5))
```
#### Result
```json
"lo Wo"
```


