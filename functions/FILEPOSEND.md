---
title: FILEPOSEND
permalink: /functions/fileposend
---

# FILEPOSEND - Metadata

Returns the end position of the document, within the file.


## Output

**Number:** The end position of the document

## Usage

```joda
FILEPOSEND()
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
AS ('': FILEPOSEND())
```
#### Result
```json
10
22
```


