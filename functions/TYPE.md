---
title: TYPE
permalink: /functions/type
---

# TYPE - Type

Returns the type of the given attribute

## Details

The type may be any of the following: 
- OBJECT
- ARRAY
- NUMBER
- STRING
- BOOL
- NULL



## Output

**String:** The string name of the type

## Usage

```joda
TYPE()
```

## Examples

### Example 1

#### Input Document
```json
[1,2]
{"doc":1}
1
"String"
true
null
```


#### Query
```joda
LOAD tmp
AS ('': TYPE(''))
```
#### Result
```json
"ARRAY"
"OBJECT"
"NUMBER"
"STRING"
"BOOL"
"NULL"
```


