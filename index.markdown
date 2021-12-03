---

---
<img id="logo" src="assets/img/JODA.svg" alt="JODA" />
<div class="small-logo-container">
<div class="small-logo-row">
<a class="small-logo" href="https://dbis.cs.uni-kl.de/index.php/en/"><img  src="assets/img/dbislogo_default.svg" alt="DBIS Lab" /></a>
<a class="small-logo" href="https://www.uni-kl.de/en/"><img  src="assets/img/TUKL_LOGO.svg" alt="TUK" /></a>
</div>
</div>


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


## Latest Release - {{site.github.latest_release.tag_name}}

The latest release can always be found on [GitHub](https://github.com/JODA-Explore/JODA/releases).

### {{site.github.latest_release.name}} ({{site.github.latest_release.published_at | date: "%d-%m-%Y"}})

{{site.github.latest_release.body}}


## Citation

If you use this project in your research, please cite it using our ICDE 2020 demo paper.

### Bibtex:

```
{% raw %}
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
{% endraw %}
```