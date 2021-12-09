---
title: HASH
permalink: /functions/hash
---

# HASH - Misc

Computes a hash value of the given value

## Details

In case of arrays or objects, the hash is calculated for the complete subtree. Hence, the hash value can be used to check jsonify objects for equality.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Any | False |  |

## Output

**Number:** a hash number, representing the underlying value

## Usage

```joda
HASH(<x>)
```

## Examples

### Example 1

#### Input Document
```json
1
-4
5.6
"  -123.3543  "
true
false
[1,2,3]
{"doc":1} 
```


#### Query
```joda
LOAD tmp
AS ('': HASH(''))
```
#### Result
```json
2654435770
2654435765
15434769447420412623
15361542772138653858
2654435770
2654435769
11093822460243
7615289692239180477
```


