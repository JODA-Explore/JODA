---
title: Index
permalink: /functions/
---
# Mathematical

* [ABS(\<x>)](abs) - Calculates the absolute value of a number
* [ACOS(\<x>)](acos) - Calculates the arccosine of the given number
* [ASIN(\<x>)](asin) - Calculates the arcsine of the given number
* [ATAN(\<x>)](atan) - Calculates the arctangent of the given number
* [ATAN2(\<x>, \<y>)](atan2) - Calculates the ATAN2 of the given number
* [CEIL(\<x>)](ceil) - Calculates the ceiling of the given (floating point) number
* [COS(\<x>)](cos) - Calculates the cosine of the given number
* [DEGREES(\<x>)](degrees) - Converts the given radians to degrees
* [DIV(\<x>, \<y>)](div) - Divides x by y
* [FLOOR(\<x>)](floor) - Calculates the floor of the given (floating point) number
* [MOD(\<x>, \<y>)](mod) - Returns the remainder of the division of `x` by `y`
* [PI()](pi) - Constant of the number pi.
* [POW(\<x>, \<y>)](pow) - Calculates `x` to the power of `y`
* [PROD(\<x>, \<y>)](prod) - Multiplies `x` with `y`
* [RADIANS(\<x>)](radians) - Converts the given degrees to radians
* [ROUND(\<x>)](round) - Rounds the given (floating point) number to the nearest integer number
* [SIN(\<x>)](sin) - Calculates the sine of the given number
* [SQRT(\<x>)](sqrt) - Calculates the square root of `x`
* [SUB(\<x>, \<y>)](sub) - Subtracts `x` from `y`
* [SUM(\<x>, \<y>)](sum) - Calculates the sum of `x` and `y`
* [TAN(\<x>)](tan) - Calculates the tangent of x
* [TRUNC(\<x>)](trunc) - Truncates the floating point number

# Iterator

* [ALL(\<iteratable>, \<predicate>)](all) - Checks if the second parameter is true for all children of the first parameter.
* [ANY(\<iteratable>, \<predicate>)](any) - Checks if the second parameter is true for any children of the first parameter.
* [FILTER(\<iteratable>, \<predicate>)](filter) - Filters an array with a given predicate.
* [MAP(\<iteratable>, \<map function>)](map) - Maps array children into another value

# Boolean Algebra

* [AND(\<lhs>, \<rhs>)](and) - Combines the two parameters with the Boolean AND operation.
* [IMPLICATION(\<lhs>, \<rhs>)](implication) - Combines the two parameters with the Boolean implication (->) operation.
* [NOT(\<x>)](not) - Negates the Boolean value
* [OR(\<lhs>, \<rhs>)](or) - Combines the two parameters with the Boolean OR operation.
* [XOR(\<lhs>, \<rhs>)](xor) - Combines the two parameters with the Boolean exclusive or (XOR) operation.

# String

* [CONCAT(\<str1>, \<str2>)](concat) - Concatenates two string values
* [FINDSTR(\<str>, \<substr>)](findstr) - Returns the position of the second string in the first string, or -1 if it is not contained.
* [LEN(\<str>)](len) - Returns the length of the passed string
* [LOWER(\<str>)](lower) - Converts the given string to lower case
* [LTRIM(\<str>)](ltrim) - Trims all whitespace to the left of the string
* [REGEX(\<str>, \<regexp>)](regex) - Checks whether `str` matches the regular expression `regexp`
* [REGEX_EXTRACT(\<str>, \<regexp>)](regex_extract) - Matches all `regexp` in `str` and returns them.
* [REGEX_EXTRACT_FIRST(\<str>, \<regexp>)](regex_extract_first) - Matches first `regexp` in `str` and returns it.
* [REGEX_REPLACE(\<str>, \<regexp>, \<replace>)](regex_replace) - Replaces all matches of `regexp` in `str` with `replace`
* [RTRIM(\<str>)](rtrim) - Trims all whitespace to the right of the string
* [SCONTAINS(\<str>, \<substr>)](scontains) - Checks whether `str` contains `substr`
* [SPLIT(\<str>, \<delimiter>)](split) - Splits a given string by the supplied delimiter
* [STARTSWITH(\<str>, \<substr>)](startswith) - Checks whether `str` starts with `substr`
* [SUBSTR(\<str>, \<start>, (\<length>))](substr) - Returns a substring of `str` from `start` with `end` characters.
* [UPPER(\<str>)](upper) - Converts the given string to upper case

# Comparison

* [EQUAL(\<lhs>, \<rhs>)](equal) - Checks if the given parameters have the same values.
* [GREATER(\<lhs>, \<rhs>)](greater) - Checks if `lhs` is greater than `rhs`.
* [GREATEREQ(\<lhs>, \<rhs>)](greatereq) - Checks if `lhs` is greater or equal than `rhs`.
* [LESS(\<lhs>, \<rhs>)](less) - Checks if `lhs` is less than `rhs`.
* [LESSEQ(\<lhs>, \<rhs>)](lesseq) - Checks if `lhs` is less or equal than `rhs`.
* [UNEQUAL(\<lhs>, \<rhs>)](unequal) - Checks if the given parameters do not have the same values.

# Type

* [EXISTS(\<x>)](exists) - Checks if the given attribute exists.
* [FALSY(\<x>)](falsy) - Checks whether the given value is falsy.
* [ISARRAY(\<x>)](isarray) - Checks whether the attribute is of type array
* [ISBOOL(\<x>)](isbool) - Checks whether the attribute is of type Bool
* [ISNULL(\<x>)](isnull) - Checks whether the attribute is of type null
* [ISNUMBER(\<x>)](isnumber) - Checks whether the attribute is of numerical type
* [ISOBJECT(\<x>)](isobject) - Checks whether the attribute is of type object
* [ISSTRING(\<x>)](isstring) - Checks whether the attribute is of type string
* [TRUTHY(\<x>)](truthy) - Checks whether the given value is truthy.
* [TYPE()](type) - Returns the type of the given attribute

# Metadata

* [FILENAME()](filename) - Returns the filename (with full path), or "[PROJECTION]" if the document was projected
* [FILEPOSEND()](fileposend) - Returns the end position of the document, within the file.
* [FILEPOSSTART()](fileposstart) - Returns the starting position of the document, within the file.
* [ID()](id) - Returns the internal unique ID of the document.
* [NOW()](now) - Returns the current UNIX timestamp in milliseconds

# Cast

* [FLOAT(\<x>)](float) - Casts the given value to floating point
* [INT(\<x>)](int) - Casts/Parses the given value to integer
* [STRING(\<x>, (\<jsonify>))](string) - Converts an atomic value to their string representation

# Misc

* [HASH(\<x>)](hash) - Computes a hash value of the given value
* [SEQNUM()](seqnum) - Returns a sequential number for every document in the collection

# Array

* [IN(\<element>, \<array>)](in) - Checks whether the `element` is contained in the `array`
* [SIZE(\<arr>)](size) - Returns the size of the array

# Object

* [LISTATTRIBUTES(\<obj>)](listattributes) - Returns a list of all member names in the given object
* [MEMCOUNT(\<obj>)](memcount) - Returns the number of members in the object
