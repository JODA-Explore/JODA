---
title: FILEPOSSTART
permalink: /functions/fileposstart
---

# FILEPOSSTART - Metadata

Returns the starting position of the document, within the file.


## Output

**Number:** The starting position of the document

## Usage

```joda
FILEPOSSTART()
```

## Examples

### Example 1

#### Input Document
```json
{"doc1":1} 
{"doc2":2}
```


#### Query
```joda
LOAD tmp
AS ('': FILEPOSSTART())
```
#### Result
```json
0
10
```


