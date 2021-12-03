---
title: Language
permalink: /language/
---

<a href="{{ site.baseurl }}/"><img id="logo" src="{{ '/assets/img/JODA.svg' | relative_url }}" alt="JODA" /></a>


## Syntax
The basic syntax of queries is:

```joda
LOAD     <COLLECTION>   (<SOURCES>)
(CHOOSE  <PRED>)
(AS      <PROJ>)
(AGG     <AGG>)
(STORE   <COLLECTION>  | AS FILE "<FILE>" | AS FILES "<DIR>")
(DELETE  <COLLECTION>)
```


The execution order is `LOAD`->`CHOOSE`->`AS`->`AGG`->`STORE`->`DELETE`.
### Load
`LOAD <COLLECTION>` loads an previously stored result,
for further usage in the following query, from variable `<COLLECTION>`.

`LOAD <COLLECTION> <SOURCES>` imports data from the `<SOURCES>` into `<COLLECTION>` and continues the query with it.
If `<COLLECTION>` already exists, the contents of `<SOURCES>` will be appended
to it.

`<SOURCES>` can be one or multiple single `<SOURCE>`, combined with `,`.

Instead of `<SOURCES>`, data can also be imported by an ongoing group by with the
`FROM GROUPED <EXP>` command. (For more informations see [Group-By](#group-by))

### Sources
Currently the following sources are implemented:

`FROM FILE "<FILE>"` parses a single file in the path `<FILE>`. (Supports [sampling](#sampling))
 
`FROM FILES "<DIR>"` parses all files in the directory `<DIR>`. (Supports [sampling](#sampling))

`FROM URL "<URL>"` parses documents returned by a GET call to `<URL>`. 

#### Sampling
Some sources may be sampled, to decrease memory footprint and/or improve query times.

To sample sources `SAMPLE X` has to be appended to a `<SOURCE>` command.
`X` has to be a value between (0,1).

Keep in mind that sampling only skips documents in the sources.
All results will only represent these sources.

Currently, no further extrapolations are performed by the system to project aggregations
like sums or similar functions.

### CHOOSE (Optional)
`CHOOSE <PRED>` selects JSON documents according to the given predicates.
If this command is not supplied, all documents will be retrieved.

The predicate can consist of the following:

- `<PRED> && <PRED>`: AND (Both predicates have to be fulfilled)
- `<PRED> || <PRED>`: OR (One of the predicates have to be fulfilled)
- `!<PRED>`: NEGATE (Returns the opposite of the given predicate)
- `(<PRED>)`: BRACKET (Predicates can be bracketed)
- `<VAR> <CMP> <VAR>`: COMPARE (Compares the given variables)
- `<VAR>`: Single variable/function


#### Variables
Variables can be any of the following:
- Numbers (e.g.: `1`, `+3.14`, `-284674`)
- Strings (e.g.: `"Hello World!"`)
- Boolean (`true`, `false`)
- [JSON-Pointer](#json-pointer) (e.g.: `'/test/3'`)
- [Functions](#functions)


#### Comparison
Comparisons can be any of the following:
- `==`
- `!=`
- `>=`
- `>`
- `<=`
- `<`

Each does what is expected.
For comparisions the following rules apply:

Pointer evaluate to the needed type. If the given pointer does not exist
or the evaluated type is not compatible, then false is returned.

Every comparison may be applied to two strings,
but they may not be compared to anything else.

Every comparison may be applied to two numbers,
but they may not be compared to anything else.

Booleans can only be checked for equality or unequality and may not be
compared to anything else.

### AS (Optional)
`AS <PROJ>` projects the chosen documents into the with `<PROJ>` given form.
If this command is not supplied, the documents will stay as they are.

The projection command has the following syntax:

```
<PROJ> :    ('<TOPOINTER>' : <SOURCE>), ...
```

`<TOPOINTER>`: JSON-Pointer, see section [JSON-Pointer](#json-pointer), in which the supplied source will be stored
#### Source
Source can be any of the following:

- `<FROMPOINTER>`: (Any)JSON-Pointer, see section [JSON-Pointer](#json-pointer)
- [Function](#functions)
- [Set-Function](#set-functions)

#### Set Functions
Set functions can provide multiple values, which results in duplication of
the given document with different values in the specified attributes.

Currently the following set functions are implemented:
- `FLATTEN(<POINTER>)`: (Any) If `<POINTER>` is of type array, each element contained within
is distibuted over multiple documents.

#### Examples
With the following JSON input:
```json
{
    "sub":
        {
            "array":
                [
                    1,
                    "test",
                    3
                ]
        }
}
```

`AS ('/id':ID()),('/file':FILENAME()),('/arr_elt':FLATTEN('/sub/array')) ` could result in this JSON output:

```json
{
    "id" :      12,
    "file" :    "/home/testuser/testjson.json",
    "arr_elt" : 1
}

{
    "id" :      12,
    "file" :    "/home/testuser/testjson.json",
    "arr_elt" : "test"
}

{
    "id" :      12,
    "file" :    "/home/testuser/testjson.json",
    "arr_elt" : 3
}
```


### AGG (Optional)
`AGG <AGG>` aggregates the results of the projection.
If no aggregation is supplied, the results will be returned as is.

The aggregation command has the following syntax:

```
<AGG> :     ('<TOPOINTER>' : <SOURCE>), ...

```


#### Keywords
- __\<TOPOINTER>__: JSON-Pointer, see section [JSON-Pointer](#json-pointer), in which result of the aggregation be stored

Source can be any of the following functions:
- __AVG(\<Number>)__: (Number) Calculates the average of the found numbers.
- __AVG(\<Number>,\<Bool>)__: (Number) Calculates the average of the found numbers. If the second parameter is true, then the contents of arrays are averaged up too.
- __COUNT()__: (Number) Counts the number of documents.
- __COUNT(\<ANY>)__: (Number) Counts how often the given value/pointer exists.
- __SUM(\<Number>)__: (Number) Calculates the sum of the found numbers.
- __SUM(\<Number>,\<Bool>)__: (Number) Calculates the sum of the found numbers. If the second parameter is true, then the contents of arrays are summed up too.
- __DISTINCT(\<Number/String/Bool>)__: (Array\[Any]) Returns an array of distinct found elements, non atomic values are discarded.
- __COLLECT(\<ANY>)__: (Array\[Any]) Returns an array of all elements.
- __ATTSTAT(\<Object>)__: (Object) Returns a statistic about the attributes in the given object.
- __HISTOGRAM(\<Number>,\<Number>,\<Number>,\<Number>)__: (Object) Creates a histogram of the first parameter. The following parameters represent the number of buckets, the inclusive minimum value, and the exclusive maximum value.

#### GROUP BY
Aggregations can also be grouped by a specified value.

```
<AGG>           : ('<TOPOINTER>' : GROUP <SOURCE> <AS> BY <VALUE>)
<AS> [optional] : AS <IDENT>
<IDENT>         : Any variable name string.
```

**Note: ** Only atomic data types (string, number, boolean) can be used to group. 
Arrays and objects are ignored.

#### Examples
With the following JSON input:
```json
{"num": 0, "dyn" : 1}
{"num": 1, "dyn" : 1}
{"num": 2, "dyn" : "test"}
{"num": 3, "dyn" : {"sub":1}}
{"num": 4 }
```
**Query**
```
LOAD data
AGG   ('/sum':SUM('/num')),
      ('/count':COUNT('/dyn')),
      ('/distinct':DISTINCT('/dyn')),
      ('/grouped' : GROUP COUNT('/num') AS count BY '/dyn')
```
**Result**
```json
{
    "sum" : 10,
    "count" : 4,
    "distinct" : [1, "test"],
    "grouped" : [
      {
        "group" : 1,
        "count" : 2
      },
      {
        "group" : "test",
        "count" : 2
      }
    ]
}
```

### STORE (Optional)
`STORE <VAR>` stores the result of the  query in variable `<VAR>`.

`STORE AS FILE "<FILE>"` writes the result of the query to `<FILE>`.

`STORE GROUPED BY <SOURCE>` groupes the dataset. (See [Group-By](#group-by))
### DELETE (Optional)
`DELETE <VAR>` removes the result stored in `<VAR>`.


## Group By
Joda has the ability to group multiple documents together, depending on a value.

This is achieved by storing a dataset with the `GROUPED BY` clause.

`STORE GROUPED BY <SOURCE>` groups all the documents, with the same value for `<SOURCE>`, together.
To retrieve these grouped results a new query is necessary.

`LOAD <VAR> FROM GROUPED <SOURCE>` will retrieve previously grouped results.

Multiple datasets can be grouped together, by repeatedly executing a `STORE` command with the same `<SOURCE>`.

After the `LOAD` statement, the grouping is complete and can no longer be referenced.
### Source
Source can be any of the following:

- `<FROMPOINTER>`: (Any)JSON-Pointer, see section [JSON-Pointer](#json-pointer)
- [Function](#functions)

### Examples
Lets assume we have multiple messages replying to each other:
```json
{
    "id" : 2,
    "reply_to" : 1,
    "msg" : "Hello World!",
},
{
    "id" : 3,
    "reply_to" : 1,
    "msg" : "Hello Joda!",
}
```
Now we might want to get all replies to the same message.
This can be achieved by combining these two queries:

```
 LOAD msgs
 STORE GROUPED BY '/reply_to';

 LOAD replies FROM GROUPED '/reply_to';
```

 This will return:

```json
 {
     "reply_to" : 1,
     "joins" :
        [
            {
                "id" : 2,
                "reply_to" : 1,
                "msg" : "Hello World!",
            },
             {
                 "id" : 3,
                 "reply_to" : 1,
                 "msg" : "Hello Joda!",
             }
        ]
 }
```

## Misc
### Functions
##### Metadata-/File-Functions
- (String) `FILENAME()`: Returns the filename (with full path),
or "\[PROJECTION]" if the document was projected.
- (Int) `FILEPOSSTART()`: Returns the starting position of the document, within it's file.
- (Int) `FILEPOSEND()`: Returns the end position of the document, within it's file.
- (Int) `ID()`: Returns the internal ID of the document.



##### Type-Functions
- (String) `TYPE(Any)`: Returns the type of the given attribute
("OBJECT","ARRAY","NUMBER","STRING","BOOL","NULL");
- (Bool) `EXISTS(Any)`: Checks if the given attribute exists.
- (Bool) `ISX(Any)`: Checks if the given attribute has type X:
    - `ISNULL`
    - `ISBOOL`
    - `ISNUMBER`
    - `ISSTRING`
    - `ISOBJECT`
    - `ISARRAY`
  
##### Casting-Functions
- (Number) `INT(Any)`: Returns the integer representation of the value. Floats are truncated, Strings are parsed as numbers if possible, and Boolean is converted to {1,0}.
- (Number) `FLOAT(Any)`: Returns the float representation of the value. Strings are parsed as numbers if possible, and Boolean is converted to {1,0}.
- (String) `STRING(Any)`: Returns the string representation of the value. Only works for non-object/array values.

##### String-Functions
- (Number) `LEN(String)`: Returns the length of the string.
- (String) `LOWER(String)`: Returns the string in lowercase.
- (String) `UPPER(String)`: Returns the string in uppercase.
- (String) `LTRIM(String)`: Trims spaces to the left of the string.
- (String) `RTRIM(String)`: Trims spaces to the right of the string.
- (String) `CONCAT(String,String)`: Concatenates two strings.
- (Bool) `SCONTAINS(String,String)`: Returns true if first string contains the second.
- (Bool) `STARTSWITH(String,String)`: Returns true if first string starts with the second.
- (Number) `FINDSTR(String,String)`: Returns the position of the second string in the first string, or -1 if it is not contained.
- (String) `SUBSTR(String,Number,Number)`: Returns a substring of the given string. The first number is the starting position of the substring, the second (optional) number is the length of the substring. If the second is not given, everything until the end is returned.
- (Bool) `REGEX(String,String)`: Returns true if first string matches the regular expression defined in the second.
- (Array(String)) `REGEX_EXTRACT(String,String)`: Returns a list of strings within the first parameter, matched by the regular expression in the second.
- (String) `REGEX_REPLACE(String,String,String)`: Replaces all matches, within the first string, of the regular expression, defined in the second parameter, with the third string.

##### Array-Functions
- (Number) `SIZE(Array)`: Returns the size of the array.
- (Bool) `IN(Any,Array)`: Checks if parameter 1 is contained in the array given by parameter 2.

##### Object-Functions
- (Number) `MEMCOUNT(Object)`: Returns the number of members.
- (Array(String)) `LISTATTRIBUTES(Object)`: Returns all members in the given object.


##### Mathematical-Functions
- (Number) `SUM(Number, Number)`: Calculates the sum of all arguments.
- (Number) `SUB(Number, Number)`: Subtracts the arguments.
- (Number) `PROD(Number, Number)`: Calculates the product of all arguments.
- (Number) `DIV(Number, Number)`: Divides the arguments.
- (Number) `MOD(Number, Number)`: Calculates the modulus.
- (Number) `POW(Number, Number)`: Calculates the power.
- (Number) `ABS(Number)`: Calculates the absolute.
- (Number) `CEIL(Number)`: Calculates the ceiling.
- (Number) `FLOOR(Number)`: Calculates the floor.
- (Number) `TRUNC(Number)`: Truncates the value.
- (Number) `ROUND(Number)`: Round the value.
- (Number) `SQRT(Number)`: Calculates the square root
- (Number) `DEGREES(Number)`: Radians to degrees.
- (Number) `RADIANS(Number)`: Degrees to radians.

##### Trigonometric-Functions
- (Number) `ACOS(Number)`:	inverse cosine
- (Number) `ASIN(Number)`:	inverse sine
- (Number) `ATAN(Number)`:	inverse tangent
- (Number) `ATAN2(Number, Number)`:	inverse tangent of y/x
- (Number) `COS(Number)`:	cosine
- (Number) `SIN(Number)`:	sine
- (Number) `TAN(Number)`:	tangent

##### Mathematical-Constants
- (Number) `PI()`: Returns PI

##### Time-Functions
- (Number) `NOW()`: Returns the UNIX timestamp in milliseconds.

##### Misc-Functions
- (Number) `SEQNUM()`: Returns a sequential number between 0 and the number of documents.
    - Multiple `SEQNUM()` calls within one query each return a number in the range independently from each other. (e.g.: `... AS ('/seq1':SEQNUM()),('/seq2':SEQNUM())` could result in `{"seq1":1,"seq2":3}`.
- (Number) `HASH(Any)`: Returns the hash of the value. 

#### Parameters
The parameters are defined as:
- `<NUMBER>`: (e.g.: `1`, `+3.14`, `-284674`)
- `<STRING>`: (e.g.: `"Hello World!"`)
- `<BOOL>`: (`true`, `false`)

Each parameter can be replaced by a [JSON-Pointer](#json-pointer).
If the attributed does not exist or is of wrong type, the function will return false (or another default value).

### JSON-Pointer
JSON-Pointers are pointers to specific parts of a JSON document.
The implementation conforms to the [RFC 6901](https://tools.ietf.org/html/rfc6901).

In this document:
```json
{
    "test" :
        [
            1,
            "test",
            {
                "sub" : "Hello World!"
            }
        ]
}
```
The pointer `'/test/2/sub'` would point to the string "Hello World!".

The empty pointer `''` points to the root object.

## Example Queries
```
LOAD A FROM FILES "/home/user/jsondirectory"
CHOOSE '/message/sender' == "Peter"
AS ('/filename' : FILENAME())
AGG ('/files' : DISTINCT('/filename'))
STORE AS FILE "/home/user/peters_files.json"
DELETE A
```
Loads all files in directory `/home/user/jsondirectory` and selects only those
with the value "Peter" for the attribute `/message/sender`.
From these selected documents the filenames, in which they where contained,
is stored in `/filename`.
The resulting documents are then aggregated by choosing all distinct filenames and
storing them as array in the attribute `/files`.

The result is a single document, which is then written to the file `/home/user/peters_files.json`.
Lastly the variable A is removed, so all documents are purged from the system.
