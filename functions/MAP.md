---
title: MAP
permalink: /functions/map
---

# MAP - Iterator

Maps array children into another value

## Details

The first parameter has to be an array pointer. For every array child the second parameter is evaluated and returned. To actually use the values of the children, a relative pointer should be used.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `iteratable` | Object/Array | False | Object or Array to iterate |
| `map function` | Any | False | Value to map the child into |

## Output

**Array:** An array containing the mapped values

## Usage

```joda
MAP(<iteratable>, <map function>)
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
AS ('': MAP('',  STRING($'')))
```
#### Result
```json
["1","true","3","4"]
```


### Example 2

#### Input Document
```json
[{"num":1},{"num":2},{"num":3},{"num":4}]
```


#### Query
```joda
LOAD tmp
AS ('': MAP('',  $'/num'))
```
#### Result
```json
[1,2,3,4]
```


