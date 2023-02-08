---
title: TRUTHY
permalink: /functions/truthy
---

# TRUTHY - Type

Checks whether the given value is truthy.

## Details

Truthy is defined as not falsy, where the falsy definition of [MDN](https://developer.mozilla.org/en-US/docs/Glossary/Falsy) has been used. Equivalent to `!FALSY(<x>)`.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Any | False |  |

## Output

**Bool:** true, if value is truthy, false else

## Usage

```joda
TRUTHY(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': TRUTHY(false))
```
#### Result
```json
false
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': TRUTHY(0))
```
#### Result
```json
false
```


### Example 3


#### Query
```joda
LOAD tmp
AS ('': TRUTHY(1))
```
#### Result
```json
true
```


### Example 4


#### Query
```joda
LOAD tmp
AS ('': TRUTHY(""))
```
#### Result
```json
false
```


### Example 5


#### Query
```joda
LOAD tmp
AS ('': TRUTHY("A"))
```
#### Result
```json
true
```


### Example 6


#### Query
```joda
LOAD tmp
AS ('': TRUTHY(null))
```
#### Result
```json
false
```


