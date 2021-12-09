---
title: STRING
permalink: /functions/string
---

# STRING - Cast

Converts an atomic value to their string representation

## Details

If `jsonify` is false, only atomic values will be stringified.
If it is enabled, also Object and Array values can be stringified.
With `jsonify` enabled, the string representation of floating point numbers can be different, as the JSON backend handles them differently from the normal stringify backend. 

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Any | False | Value to convert |
| `jsonify` | Bool | True | Converts the values into valid JSON strings, if true. (Default: `false`) |

## Output

**String:** The string representation of the value

## Usage

```joda
STRING(<x>, (<jsonify>))
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': STRING(1))
```
#### Result
```json
"1"
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': STRING(-5.4))
```
#### Result
```json
"-5.400000"
```


### Example 3


#### Query
```joda
LOAD tmp
AS ('': STRING(-5.4, true))
```
#### Result
```json
"-5.4"
```


### Example 4


#### Query
```joda
LOAD tmp
AS ('': STRING(true))
```
#### Result
```json
"true"
```


### Example 5


#### Query
```joda
LOAD tmp
AS ('': STRING(null))
```
#### Result
```json
"null"
```


### Example 6

#### Input Document
```json
{"doc":1}
```


#### Query
```joda
LOAD tmp
AS ('': STRING('', true))
```
#### Result
```json
"{\"doc\":1}"
```


### Example 7

#### Input Document
```json
[1,2,3,"String", null]
```


#### Query
```joda
LOAD tmp
AS ('': STRING('', true))
```
#### Result
```json
"[1,2,3,\"String\",null]"
```


