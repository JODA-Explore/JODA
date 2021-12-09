---
title: FILENAME
permalink: /functions/filename
---

# FILENAME - Metadata

Returns the filename (with full path), or "[PROJECTION]" if the document was projected


## Output

**String:** The name of the file

## Usage

```joda
FILENAME()
```

## Examples

### Example 1


#### Query
```joda
LOAD tmp
AS ('': FILENAME())
```
#### Result
```json
"tmp.json"
```


