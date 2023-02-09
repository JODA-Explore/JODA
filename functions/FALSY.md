---
title: FALSY
permalink: /functions/falsy
---

# FALSY - Type

Checks whether the given value is falsy.

## Details

The falsy definition of [MDN](https://developer.mozilla.org/en-US/docs/Glossary/Falsy) has been used.

## Input parameters

| Name | Type | Optional | Description |
| --- | --- | --- | --- |
| `x` | Any | False |  |

## Output

**Bool:** true, if value is falsy, false else

## Usage

```joda
FALSY(<x>)
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': FALSY(false))
```
#### Result
```json
true
```


### Example 2


#### Query
```joda
LOAD tmp
AS ('': FALSY(0))
```
#### Result
```json
true
```


### Example 3


#### Query
```joda
LOAD tmp
AS ('': FALSY(1))
```
#### Result
```json
false
```


### Example 4


#### Query
```joda
LOAD tmp
AS ('': FALSY(""))
```
#### Result
```json
true
```


### Example 5


#### Query
```joda
LOAD tmp
AS ('': FALSY("A"))
```
#### Result
```json
false
```


### Example 6


#### Query
```joda
LOAD tmp
AS ('': FALSY(null))
```
#### Result
```json
true
```


