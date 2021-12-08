---
title: Installation
permalink: /install/
---

<a href="{{ site.baseurl }}/"><img id="logo" src="{{ '/assets/img/JODA.svg' | relative_url }}" alt="JODA" /></a>

To install JODA you can choose between the following options:

## Docker 
The easiest way to install JODA is to use public Docker images.
The main image contains the JODA executable and JODA-client program.
An additional image is available for the JODA web interface.
The images can be found on DockerHub:

<div class="btn-group">
  <a href="https://github.com/JODA-Explore/JODA/pkgs/container/JODA%2Fjoda" class="button">JODA Image</a>
  <a href="https://github.com/orgs/JODA-Explore/packages/container/package/joda-web" class="button">JODA Web Image</a>
</div> 

Starting a JODA server is as simple as running the image:

```bash
docker run -p 5632:5632 --name joda ghcr.io/joda-explore/joda/joda:latest 
```

This will start a JODA server listening on port 5632.
To interface with the server, you can either use the CLI client tool, which can only execute queries, by running:

```bash
docker exec -it joda joda-client --address localhost --port 5632
```

If no server is needed and only local execution is required, JODA can also be started as a standalone CLI:

```bash
docker run -it ghcr.io/joda-explore/joda/joda:latest -c
```
You are now in a JODA shell. Type` help;` to get a list of available commands.


### JODA Web
The best and easiest way to interact with JODA is to use the web interface.
The following docker-compose file will start a JODA server and a web interface:

```yaml
version: '3'
services:
  joda:
    container_name: joda
    restart: always
    ports:
      - "5632:5632"
    volumes:
#     - <Path-To-Data-Firectory>:/data:rw  # Optional directory, if access to local datasets is desired.
      - /etc/localtime:/etc/localtime:ro
    image: ghcr.io/joda-explore/joda/joda:latest
  joda-web:
    container_name: joda-web
    restart: always
    ports:
      - "8080:8080"
    image: ghcr.io/joda-explore/joda-web:latest
    depends_on:
      - joda
    command: "http://joda:5632"
```

The web interface can now be accessed at [http://localhost:8080](http://localhost:8080).
Refer to the [Getting Started]({{ '/getting-started' | relative_url }}) page for further instructions on using the interface.

## Packages
Native packages for JODA are available for Ubuntu and Debian distributions on the [release page](https://github.com/JODA-Explore/JODA/releases).
To install the packages, run:

```bash
sudo apt install <Joda-Package>.deb
```

This will install the `joda` and `joda-client` executables on your system.



## Compiling

If you are using another system, you can compile the JODA binaries yourself.
JODA currently only supports Linux systems.
Before you can compile JODA, you have to install the following dev-dependencies:

- Jemalloc: (Optional Enhances performance.) 
- Boost: The basic boost package + modules `system` , `regex` and `iostreams` 
- NCurses with wide character support
- Readline

For optional support for `FROM URL` imports, the following packages are also required:
- cpprest: C++ REST client 
- OpenSSL: OpenSSL for SSL support 

Please refer to your system's package manager for the exact package names.

To compile and install JODA, run:

```bash
git clone https://github.com/JODA-Explore/JODA
cd JODA
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
make install
```
