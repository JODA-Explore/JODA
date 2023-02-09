---
title: CONCAT
permalink: /functions/concat
---

# CONCAT - String

Concatenates two string values

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str1` | String | False | First string to concatenate |
| `str2` | String | False | Second string to concatenate |

## Output

**String:** combination of `str1` and `str2`

## Usage

```joda
CONCAT(<str1>, <str2>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': CONCAT("Hello", "World"))
```
#### Result
```json
"HelloWorld"
```


