---
title: ALL
permalink: /functions/all
---

# ALL - Iterator

Checks if the second parameter is true for all children of the first parameter.

## Details

The first parameter has to be an array or object pointer. For every member value or array child the second parameter is evaluated. To actually use the values of the children, a relative pointer should be used.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `iteratable` | Object/Array | False | Object or Array to iterate |
| `predicate` | Bool | False | Predicate to evaluate for each child |

## Output

**Bool:** True if predicate is true for all children

## Usage

```joda
ALL(<iteratable>, <predicate>)
```

## Examples

### Example 1

#### Input Document
```json
[1,2,3,4]
```


#### Query
```joda
LOAD tmp
AS ('': ALL('',  ISNUMBER($'')))
```
#### Result
```json
true
```


### Example 2

#### Input Document
```json
[{"num":1},{"num":2},{"num":3},{"num":4}]
```


#### Query
```joda
LOAD tmp
AS ('': ALL('',  $'/num' > 2))
```
#### Result
```json
false
```


