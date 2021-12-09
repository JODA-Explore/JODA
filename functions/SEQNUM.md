---
title: SEQNUM
permalink: /functions/seqnum
---

# SEQNUM - Misc

Returns a sequential number for every document in the collection

## Details

Note: due to the multi-threaded nature of JODA, the documents will not necessarily be in order. But it is guaranteed that for N documents every document gets a unique number in [0,N-1]
Multiple calls of this function in a single query will return the values independently of each other.
```joda
LOAD tmp AS ('/1': SEGNUM()), ('/2': SEGNUM())
```
may result in `[1,43]` 


## Output

**Number:** a sequential number in [0,#Documents-1]

## Usage

```joda
SEQNUM()
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
```


#### Query
```joda
LOAD tmp
AS ('': SEQNUM())
```
#### Result
```json
0
1
2
3
4
5
```


