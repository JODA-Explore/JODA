<h1>Joda - JSON On-Demand Analysis</h1>

<div style="text-align:center">
<img src="https://dbis.informatik.uni-kl.de/files/icons/JODA.png" width="380"  />
<br>
<a href="https://dbis.informatik.uni-kl.de">
<img src="https://dbis.informatik.uni-kl.de/images/dbislogo_default.png" width="130"  />
</a>
</div>

JODA is an in-memory vertically scalable data processor for semi-structured data, with an initial emphasis on JSON datasets.
It can be used to efficiently filter, transform, and aggregate a large set of JSON documents.

- [Installation](#installation)
  - [Docker](#docker)
  - [Packages](#packages)
  - [Precompiled](#precompiled)
    - [Requirements](#requirements)
  - [Compiling](#compiling)
    - [Requirements](#requirements-1)
    - [Commands](#commands)
- [Program Flags](#program-flags)
- [CLI Commands](#cli-commands)
- [Queries](#queries)
  - [Syntax](#syntax)
    - [Load](#load)
    - [Sources](#sources)
      - [Sampling](#sampling)
    - [CHOOSE (Optional)](#choose-optional)
      - [Variables](#variables)
      - [Comparison](#comparison)
    - [AS (Optional)](#as-optional)
      - [Source](#source)
      - [Set Functions](#set-functions)
      - [Examples](#examples)
    - [AGG (Optional)](#agg-optional)
      - [Keywords](#keywords)
      - [GROUP BY](#group-by)
      - [Examples](#examples-1)
    - [STORE (Optional)](#store-optional)
    - [DELETE (Optional)](#delete-optional)
  - [Group By](#group-by-1)
    - [Source](#source-1)
    - [Examples](#examples-2)
  - [Misc](#misc)
    - [Functions](#functions)
        - [Metadata-/File-Functions](#metadata-file-functions)
        - [Type/Attribute-Functions](#typeattribute-functions)
        - [String-Functions](#string-functions)
        - [Array-Functions](#array-functions)
        - [Object-Functions](#object-functions)
        - [Mathematical-Functions](#mathematical-functions)
        - [Trigonometric-Functions](#trigonometric-functions)
        - [Mathematical-Constants](#mathematical-constants)
        - [Time-Functions](#time-functions)
        - [Misc-Functions](#misc-functions)
      - [Parameters](#parameters)
    - [JSON-Pointer](#json-pointer)
  - [Example Queries](#example-queries)
- [Citation](#citation)
  - [Bibtex:](#bibtex)

# Installation

## Docker
The easiest way to get JODA up and running is to use our Docker image available in the GitHub registry.


## Packages
Packages to use with distro package managers are currently built for:
- Debian Buster
- Ubuntu 16.04
- Ubuntu 18.04

## Precompiled
Precompiled binary executables are can be downloaded from the release section on this repo.

### Requirements
To be able to use the main software, the following packages have to be installed:

- Glog: Google logging framework (Debian Buster: libgoogle-glog0v5)
- C++ Rest SDK (Debian Buster: libcpprest)
- SSL Lib (Debian Buster: libssl1.1)
- Readline (Debian Buster: libreadline7)
- NCurses (Debian: libncursesw5)


Example:

    apt-get update
    apt-get install libcpprest libssl1.1 libreadline7 libncursesw5 libgoogle-glog0v5

## Compiling
The program can alternatively be compiled from the sources.

### Requirements
To be able to compile the main software, the following packages have to be installed:

- Glog: Google logging framework. (Debian: libgoogle-glog-dev)
- Jemalloc: (Optional Enhances performance.)  (Debian: libjemalloc-dev)
- Boost: The basic boost package + modules `system` , `regex` and `iostreams` (Debian: libboost-dev, libboost-system-dev, libboost-regex-dev, libboost-iostreams-dev)
- NCurses: (Debian: libncursesw5-dev)
- Readline (Debian Buster: libreadline-dev)
- 
For optional support for `FROM URL` imports, the following packages are also required:

- cpprest: C++ REST client (Debian: libcpprest-dev)
- OpenSSL: OpenSSL for SSL support (Debian: libssl-dev)
To use the included client for server/client mode the following additional package is needed:


Example:

    apt-get update
    apt-get install libssl-dev libcpprest-dev libjemalloc-dev libgoogle-glog-dev libboost-dev libboost-system-dev libboost-regex-dev libboost-program-options-dev libncurses5-dev libncursesw5-dev libreadline-dev


### Commands

To compile the program in Release mode, the following commands can be used.

    https://git.cs.uni-kl.de/n_schaefer11/JODA.git
    cd JODA
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
    make install

# Program Flags
These are options that may be supplied to the program during invokation.

- __-h --help__:        Shows help text and exits the program
- __--version__:        Displays the version number
- __-d --data__:        Data dictionary for benchmarks. (Default: current dictionary)
- __--tmpdir__:           Dictionary to use for temporary files.
- __-s --nostorage__:   Does not keep JSON files in memory. This results in less RAM usage, but increases execution time
- __--tmpdir__:           Temporary directory to use. (Default: "/tmp/PJDB")
- __-t --maxthreads__:  Sets the maximum number of threads to be used. Default is all available threads
- __--logtostderr__:    Writes log to terminal.
- __-c --time__:        Times the execution
- __--benchfile__:      Sets file in which time (and benchmark) results will be stored
- __--server__:         Starts the program in server mode (Needs port option)
- __-p --port__:        Port on which the server will listen

# CLI Commands
These commands can be used any time within the program.
The commands have to be entered followed by a `;`.
- `quit`:             Quits the program
- `cache`:            Toggles usage of the cache
- `sources`:     Lists all named data sources (and the # of documents stored in them)
- `results`:     Lists all temporary data sources (and the # of documents stored in them)

# Queries

## Syntax
The basic syntax of queries is:
```
LOAD    <VAR>   (<SOURCES>)
CHOOSE  <PRED>
AS      <PROJ>
AGG     <AGG>
STORE  (<VAR>  | AS FILE        "<FILE>")
DELETE  <VAR>
```


The execution order is `LOAD`->`CHOOSE`->`AS`->`AGG`->`STORE`->`DELETE`.
### Load
`LOAD <VAR>` loads an previously stored result,
for further usage in the following query, from variable `<VAR>`.

`LOAD <VAR> <SOURCES>` imports data from the `<SOURCES>` into `<VAR>` and continues the query with it.
If `<VAR>` already exists, the contents of `<SOURCES>` will be appended
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
- [Set-Function](#set-function)

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



##### Type/Attribute-Functions
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
- (Array(String)) `LISTATTRIBUTES(Object,Bool)`: Returns all named attributes in the given object or an empty list if pointed-to value is not an object. The attributes are listed recursively for all objects, if the second parameter is set to true (Default: false).

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

# Citation
If you want to cite this project in your research, please use our ICDE 2020 demo paper.

## Bibtex:

```
@inproceedings{DBLP:conf/icde/Schafer020,
  author    = {Nico Sch{\"{a}}fer and
               Sebastian Michel},
  title     = {{JODA:} {A} Vertically Scalable, Lightweight {JSON} Processor for
               Big Data Transformations},
  booktitle = {36th {IEEE} International Conference on Data Engineering, {ICDE} 2020,
               Dallas, TX, USA, April 20-24, 2020},
  pages     = {1726--1729},
  publisher = {{IEEE}},
  year      = {2020},
  url       = {https://doi.org/10.1109/ICDE48307.2020.00155},
  doi       = {10.1109/ICDE48307.2020.00155},
  timestamp = {Fri, 05 Jun 2020 17:54:57 +0200},
  biburl    = {https://dblp.org/rec/conf/icde/Schafer020.bib},
  bibsource = {dblp computer science bibliography, https://dblp.org}
}
```

