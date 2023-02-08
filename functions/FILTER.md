---
title: FILTER
permalink: /functions/filter
---

# FILTER - Iterator

Filters an array with a given predicate.

## Details

The first parameter has to be an array pointer. For every array child the second parameter is evaluated. If it returns true, the value is kept. To actually use the values of the children, a relative pointer should be used.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `iteratable` | Object/Array | False | Object or Array to iterate |
| `predicate` | Bool | False | Predicate to evaluate for each child |

## Output

**Array:** An array containing only the children for which the predicate returned true

## Usage

```joda
FILTER(<iteratable>, <predicate>)
```

## Examples

### Example 1

#### Input Document
```json
[1,true,3,"4"]
```


#### Query
```joda
LOAD tmp
AS ('': FILTER('',  ISNUMBER($'')))
```
#### Result
```json
[1,3]
```


### Example 2

#### Input Document
```json
[{"num":1},{"num":2},{"num":3},{"num":4}]
```


#### Query
```joda
LOAD tmp
AS ('': FILTER('',  $'/num' > 2))
```
#### Result
```json
[{"num":3},{"num":4}]
```


