---
title: NOW
permalink: /functions/now
---

# NOW - Metadata

Returns the current UNIX timestamp in milliseconds


## Output

**Number:** UNIX timestamp in ms

## Usage

```joda
NOW()
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': NOW())
```
#### Result
```json
1675847671029
```


