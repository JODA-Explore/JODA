<h1>Joda - JSON On-Demand Analysis</h1>

<div style="text-align:center">
<a href="https://joda-explore.github.io/JODA">
<img src="https://dbis.informatik.uni-kl.de/files/icons/JODA.png" width="380"  />
</a>
<br>
<a href="https://dbis.informatik.uni-kl.de">
<img src="https://dbis.informatik.uni-kl.de/images/dbislogo_default.png" width="130"  />
</a>
</div>

JODA is an in-memory vertically scalable data processor for semi-structured data, with an initial emphasis on JSON datasets.
It can be used to efficiently filter, transform, and aggregate a large set of JSON documents.
Please visit the [project website](https://joda-explore.github.io/JODA/) to learn more about what JODA is and how it can be used.

- [Installation](#installation)
- [Program Flags](#program-flags)
- [CLI Commands](#cli-commands)
- [Language specification](#language-specification)
- [Citation](#citation)
  - [Bibtex:](#bibtex)

# Installation

Detailed installation instructions can be found on the [project website](https://joda-explore.github.io/JODA/install/).


# Program Flags
These are options that may be supplied to the program during invokation.

- __-h --help__:        Shows help text and exits the program
- __--version__:        Displays the version number
- __-d --data__:        Data dictionary for benchmarks. (Default: current dictionary)
- __-s --nostorage__:   Does not keep JSON files in memory. This results in less RAM usage, but increases execution time
- __--tmpdir__:           Temporary directory to use. (Default: "/tmp/JODA")
- __-t --maxthreads__:  Sets the maximum number of threads to be used. Default is all available threads
- __--logtostderr__:    Writes log to terminal.
- __-c --time__:        Times the execution
- __--benchfile__:      Sets file in which time (and benchmark) results will be stored
- __-f --queryfile__:   A JODA query file to execute
- __--server__:         Starts the program in server mode (Needs port option)
- __-p --port__:        Port on which the server will listen

# CLI Commands
These commands can be used any time within the program.
The commands have to be entered followed by a `;`.
- `quit`:        Quits the program
- `cache`:       Toggles usage of the cache
- `sources`:     Lists all named data sources (and the # of documents stored in them)
- `results`:     Lists all temporary data sources (and the # of documents stored in them)

# Language specification
The language is also explained in-depth on our [project website](https://joda-explore.github.io/JODA/language/).
It also contains a list of all available functions with examples.




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

