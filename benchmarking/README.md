# Automatic benchmarking

Every JODA query file (files with format `<name>.joda`) in this directory will be executed on the CI Benchmark server. 
The job has to be started manually in the git pipeline.

You can also put `<name>.conf` files in this directory.
The program is then executed with this config.
For an example config file check the `/config` directory.

The file `runs.txt` specifies how often the benchmark is executed.

The resulting log outputs and benchmark files can be downloaded as artifacts.


## Query file
Each query file must contain line separated JODA queries.
So one query per line.

## Datasets
The server has a couple of datasets ready in the `/data` directory, which can be used in benchmark queries.
The datasets are:

### Twitter datasets

Multiple sizes of [Twitter dumps](https://developer.twitter.com/en/docs/twitter-api/v1/data-dictionary/object-model/tweet) are available on the server in the subdirectory `/data/twitter`.

 - `109g_multiple`: many smaller `.json` files containing 109 GB of twitter data 
 - `109g_single`: one large `.json` file containing 109 GB of twitter data 
 - `50gb_multiple`: many smaller `.json` files containing 50 GB of twitter data
 - `5gb_multiple`: many smaller `.json` files containing 5 GB of twitter data
 - `810m_multiple`: many smaller `.json` files containing 810 MB of twitter data

To access the dataset you can use a query like:
```
LOAD twitter FROM FILES "/data/twitter/109g_multiple" LINESEPARATED;
```

### Reddit datasets

A dataset of Reddit posts and comments is available in the subdirectory `/data/reddit`.

 - `2013`: 12 smaller `.json` files containing 217 GB of reddit data 
 - `2014`: 12 smaller `.json` files containing 296 GB of reddit data 
 - `2015`: 5 smaller `.json` files containing 150 GB of reddit data 

Each year is divided into 12 ~30 GB large JSON files for each month, in the format `RC_<year>-<month>.json`, e.g.: `RC_2015-04.json`.
`2015` only contains months 01-05.

To access the dataset you can use a query like:
```
LOAD twitter FROM FILE "/data/reddit/2015/RC_2015-04.json" LINESEPARATED;
```

You can check out the data schema by downloading a sample file from [here](https://files.pushshift.io/reddit/comments/).

### Nobench
We generated some [NoBench](http://pages.cs.wisc.edu/~chasseur/pubs/argo-long.pdf) example data in the `/data/nobench/data` subdirectory.
It contains 100 ~1.2 GB data files with the names `data1.json`-`data100.json`.

The schema of the data can be found in the paper linked above in Chapter 4.