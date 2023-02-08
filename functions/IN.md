---
title: IN
permalink: /functions/in
---

# IN - Array

Checks whether the `element` is contained in the `array`

## Details

The `element` value can only be an atomic value (Number, String, Bool, Null)

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `element` | Any (Atomic) | False |  |
| `array` | Array | False |  |

## Output

**Bool:**

## Usage

```joda
IN(<element>, <array>)
```

## Examples

### Example 1

#### Input Document
```json
[1,2,"String"]
```


#### Query
```joda
LOAD tmp
AS ('': IN("String", ''))
```
#### Result
```json
true
```


### Example 2

#### Input Document
```json
{"arr":[1,2,"String"]}
```


#### Query
```joda
LOAD tmp
AS ('': IN(3, '/arr'))
```
#### Result
```json
false
```


