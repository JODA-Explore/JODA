# Changelog
## DEV
#### Added
#### Changed
#### Fixed


## 0.12.X
### 0.12.3
#### Changed
- Create 2 containers per thread by default
#### Fixed
- Fix statistics not being stored persistently in server mode
- Fix container scheduling 
- Improve ImportSource size estimation
### 0.12.2
#### Added
- FINDSTR and SUBSTR functions
#### Changed
- Improve ATTSTAT aggregator with additional statistics
- Improve queryfile handling
#### Fixed
- Memory leaks
- Aggregation step getting stuck if many aggregator functions are used in one query
- Document position recording when using streams
- General docker image improvements
- Many code style warnings 
- Format codebase in google c++ style convention
#### Added
#### Changed
- Docker build improvements (speedup, multi-stage builds, ...)
#### Fixed
- First query since program start not returning result size
### 0.12.0
#### Added
- Add [Delta-Tree](https://dbis.informatik.uni-kl.de/files/papers/edbt2020_joda_data_wrangling.pdf) feature to significantly reduce memory usage
- Add color to JSON viewer in CLI
#### Changed
- OSUtility now caches OS and kernel info as it will not change during execution
- Server requests are now timed


## 0.11.X
### 0.11.2
 This release primarily improves readability and usability of the code
 It also moves the codebase to C++17
### 0.11.1
#### Fixed
- Wrong package build files
### 0.11.0
#### Added
 - New HTTP based network API with a lot more functionality (see wiki)
 - Improved select performance by using bitvectors
 - Added Memory management. Now a maximum memory consumption can be set for JODA
#### Changed
- Default Server API to v2
#### Fixed
- Many Bugfixes (seqfault, wrong workflows)
## 0.10.X
### 0.10.0
#### Added
- `FROM URL` can be used to parse JSON documents directly from web resources.
- `NOW()` function
- `STARTSWITH(String, String)` function
#### Changed
- \[internal\]JSONContainer serialization logic changed.
- `FROM FILES` now recursively parses all files in subdirectories.
- Benchmark files are now in JSON format
#### Fixed
- Fixed floating point arithmetic
- Many Bugfixes (seqfault)
## 0.9.X
### 0.9.0 (24.05.2019):
#### Added
- **Document similarity based grouping of containers.** Two similarity measures were implemented, path jaccard and attribute jaccard.
- **Group By aggregation.** Aggregation can now be grouped by a specified value. (See docs)
- `MOD`, `POW`, `ABS`, `CEIL`, `FLOOR`, `TRUNC`, `ROUND`, `SQRT`, `DEGREES`, `RADIANS`, `ACOS`, `ASIN`, `ATAN`, `ATAN2`, `COS`, `SIN`, `TAN` and `PI`
 mathematical functions
- `LEN`, `CONCAT`, `LOWER`, `UPPER`, `LTRIM`, `RTRIM` string functions added.
- IValueProvider are now optimized before executing the query.
#### Changed
- Parsers now use a modular container scheduling
- Refactored IValue Atoms to be more extendable
- Refactored IValue functions to Unary/Binary specializations
- Numbers can now also be parsed and calculated in int format
#### Fixed
- Potential crash when exiting the program in `nostore` mode.
- Wrong AggMerge Time
- Containers always removing their content, even in store mode
## 0.8.X
### 0.8.0 (07.05.2019):
#### Added
- New interactive CLI interface for supported terminals
- COLLECT aggregation function
- Code coverage reports
#### Changed
- Moved to CPack for packaging
- Refactored Reader/Parser into a new (more flexible) architecture
- Refactored project into a modularized format
- Refactored project to use new ExportDestinations
##### Fixed
- Fixed a lot of bugs
## 0.7.X
### 0.7.1 (Client 0.2.2):
#### Added
- New CLI + Command history
- SUM/SUB/PROD/DIV functions
#### Changed
- Using boost program_options instead of cxxopts
#### Fixed
- Misc fixes
### 0.7.0 
#### Added
- STORE AS FILES for faster writing
- In-Memory Group
- Adapted default configurations
- Autosizing for containers
#### Fixed
- Performance enhancements
- Misc Bugfixes
