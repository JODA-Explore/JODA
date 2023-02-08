---
title: User Defined Modules
permalink: /udm/
---

<a href="{{ site.baseurl }}/"><img id="logo" src="{{ '/assets/img/JODA.svg' | relative_url }}" alt="JODA" /></a>

# User Defined Modules
JODA can be extended by supplying external modules written in python.
The name of the file will be the name of the function in the system.
Hence, no module can be named after one of the included functions or already loaded modules.

A module can be loaded in the CLI by using the  `register <path to file>;` command.
Alternatively, modules can also be supplied as command line parameter using the `-m <path to file>` option.
Each module consists of a single python file having fitting functions.
Depending on the type of the module, different functions have to be included.


## Functions
User-defined functions can be used in JODA anywhere where normal functions, pointers, or atomic values can be used.
To create a module that supplies a function the following variables and methods have to be included in the file:

```python
# Number of arguments for main function (get_value)
num_args = 1

# Return type of the function
# One of STRING, NUMBER, BOOLEAN, OBJECT, ARRAY, or ANY
ret_type = "STRING"

# Given a number of arguments, returns an arbitrary value
def get_value(arg1):
  return "Your number is " + str(arg1)
```

After this module is loaded it can be used in JODA statements by using the capitalized name of the file as function name.
If the file is named `test.py` it can be used as follows:

```joda
LOAD X
AS ('':TEST(1))
```



## Aggregation
Similarly, to create a module that supplies an aggregation function the following variables and methods have to be included in the file:
```python
# Number of arguments for main function (aggregate)
num_args = 1

# Initial state
def init_state():
  return [0, 0]

# Aggregate function, takes a state and the specified number of arguments
# and returns a new state
def aggregate(state, arg1):
  state[0] += arg1
  state[1] += 1
  return state

# Merges the own state with another state
def merge(state, other):
  state[0] += other[0]
  state[1] += other[1]
  return state

# Returns the aggregation result based on the state
def finalize(state):
  avg = state[0] / state[1]
  ret = f"AVG: {avg}. SUM: {state[0]}. COUNT: {state[1]}"
  return ret
```
This example implements a textual "average" aggregation function.
For each document the `aggregate` function is called with the current state and the value of the specified argument.
This function is called from multiple threads for different subsets of documents. 
Hence no global variables can be used.
The state is initialized for each thread using the `init_state` function.
The `merge` function is called to merge the states of different threads.
Finally, the `finalize` function is called to create and return the final result.

After this module is loaded it can be used in JODA aggregation statements by using the capitalized name of the file as function name.
If the file is named `txt_avg.py` it can be used as follows:

```joda
LOAD X
AGG ('':TXT_avg('/num'))
```

## Import
JODA also supports user-defined import modules.
These modules enable the user to load data from arbitrary sources into the JODA system.
Given an input parameter, the module has to return the values to be imported one by one.
The module has to implement the following functions:

```python
import csv

# Initializes the state, given a parameter string
def init(param):
  csvfile = open(param, newline='')
  reader = csv.DictReader(csvfile,delimiter='|')
  return reader

# Returns the next document from the input, given the state
# If no more documents are available, returns None.
def get_next(state):
  return next(state, None)

```
The `get_next` function is called repeatedly until it returns `None`.
The supported return types are `str`, `int`, `float`, `bool`, `list`, and `dict`.
Each of them will be converted to a JSON value and stored in the system.

The `init` function uses the parameter string to initialize a state, which is passed to every subsequent call of `get_next`.
It can be used to initialize a connection to a database, open a file, or anything else that is required to read the data.
In this example the parameter string points to a CSV file, which is opened and a CSV reader is initialized.

As with the previous modules, the name of the file will be the name of the import source in the system.
If this module is named `csv.py`, it can be used as follows:

```joda
LOAD X FROM CSV "path/to/file.csv"
```




## Export
Similarly to import modules, JODA also supports user-defined export modules.
Instead of loading data into the system, JSON documents can be exported to arbitrary destinations.
The module has to implement the following functions:

```python
# Initializes the state, given a parameter string
def init(param):
  return param

# Exports a single document, given the staten and the document itself
def set_next(state, doc):
  print(state + str(doc))

# Is called after the export of all documents is finished to perform cleanup
def finalize(state):
  pass

```

The `init` function uses the parameter string to initialize a state, which is passed to every subsequent call of `set_next`.
Here any connections or files can be opened.
The `set_next` function is called for each document to be exported.
Any valid JSON value can be passed as the `doc` parameter.
The `finalize` function is called after all documents have been exported.
It can be used to close connections or files. 

If the module is named `prefix.py`, it can be used as follows:

```joda
LOAD X
STORE AS PREFIX "Prefix: "
```

Now every document will be printed to the console with the prefix "Prefix: ".


## Index

The most complex type of module is an index module.
These modules can be used to improve the performance of queries by using an customized index structure.
As soon as a index module is registered in the JODA system, the filter step of each query can be executed using the index.
To implement a index module, the following functions have to be implemented:

```python
from typing import Union, List, Tuple

# Returns the initial empty state of the index
def init_index():
    return dict()

# Estimate the work remaining after the predicate has been evaluated with the index.
# None can be returned if the index is not applicable.
def estimate_usage(predicate: str, state: dict, doc_count: int) -> Union[None, int]:
    if predicate in state:
        return 0
    else:
        return None

# Execute the predicate using only the state.
# None can be returned if this is not possible.
# The return type is a tuple of a list of booleans and a boolean.
# The list of booleans contains the result of the predicate for each document.
# The boolean indicates whether the result is complete or not.
# If the result is not complete, the pre-filtered documents will be evaluated
# further using the default JODA filter implementation.
def execute_state(predicate: str, state) -> Union[None, Tuple[List[bool],bool]]:
    return (state[predicate], True)


# Execute the predicate using the state and a list of all the documents.
# The same return type as execute_state is used.
def execute_docs(predicate: str, state, docs) -> Tuple[List[bool],bool]:
    return None

# Improve the index using the final result of the query
# This function is always called after the filter step, 
# even if the index was not used to evaluate it.
def improve_index(predicate: str, state, doc_index: List[bool]):
    state[predicate] = doc_index
```

The above example implements a simple predicate cache, which stores the result of each predicate.
The `init_index` function returns the initial state of the index, in this case an empty dictionary.
The `estimate_usage` function is called to estimate the work remaining after the predicate has been evaluated with the index.
In this case, if the predicate result has been cached previously no further work is required, so 0 is returned.
Otherwise, `None` is returned to indicate that the index is not applicable.

The `execute_state` function is called to execute the predicate using only the state.
It is always called before the `execute_docs` method.
In this case, the previously cached result is returned.
The `execute_docs` function is called if the invokation of the `execute_state` method returned `None`.
In this function the actual documents can be accessed to help evaluate the predicate.
In this case, `None` is returned, as the `execute_state` function already returned a result and this function will never be called.

The `improve_index` function is called after the filter step, even if the index was not used to evaluate it.
In this case, the result of the predicate is cached in the state.

The name of the module is irrelevant, as it is not directly referenced in JODA.
Instead, all registered indices will be queried in order to find the best index for a given predicate.
JODA will execute the filter predicate with the index that returns the lowest value from the `estimate_usage` function.