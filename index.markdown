---

---
<img id="logo" src="assets/img/JODA.svg" alt="JODA" />
## What is JODA?
JODA is an efficient data wrangling tool for semi-structured JSON datasets.
It can handle every scale of data, from small-scale to big data.
Every system resource is fully utilized to reach the best performance.
JODA creates indices adaptively, depending on the workload, to optimize for iterative workloads.

If you are just getting started, check out the following resources:

<div class="btn-group">
  <a href="install/" class="button">Installation</a>
  <a href="getting-started/" class="button">Getting Started</a>
  <a href="language/" class="button">Language Reference</a>
  <a href="docs/" class="button">Code Documentation</a>
</div> 


## Latest Release

The latest release can always be found on [GitHub](https://github.com/JODA-Explore/JODA/releases).

### 0.12.4
#### Added
- Ability to use caches of subpredicates (e.g. A && B can use a cached result of "A" )
  
#### Changed
- Improve reparsing of containers if documents have been deleted
- Improve logging of web-API and storage
  
#### Fixed
- Aggregation functions with 0 parameters 
- Crashes if documents are requested to be reparsed multiple times
- Broken delta trees with reparsing
- Wrong aggregation with negative floats in ATTSTAT
