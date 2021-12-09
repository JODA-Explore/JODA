---
title: SIZE
permalink: /functions/size
---

# SIZE - Array

Returns the size of the array

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `arr` | Array | False |  |

## Output

**Number:** Size of the array

## Usage

```joda
SIZE(<arr>)
```

## Examples

### Example 1

#### Input Document
```json
[1,2,3]
```


#### Query
```joda
LOAD tmp
AS ('': SIZE(''))
```
#### Result
```json
3
```


### Example 2

#### Input Document
```json
[1,2,3,4,"String"]
```


#### Query
```joda
LOAD tmp
AS ('': SIZE(''))
```
#### Result
```json
5
```


