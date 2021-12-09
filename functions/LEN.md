---
title: LEN
permalink: /functions/len
---

# LEN - String

Returns the length of the passed string

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `str` | String | False | String to check the length of |

## Output

**Number:** The length of the given string

## Usage

```joda
LEN(<str>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': LEN("Hello World"))
```
#### Result
```json
11
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': LEN("Hello"))
```
#### Result
```json
5
```


