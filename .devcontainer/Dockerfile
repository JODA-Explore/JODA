# Set the base image
FROM ubuntu
# Dockerfile author / maintainer 
LABEL maintainer="Nico Schaefer <nschaefer@cs.uni-kl.de>"

# Install build tools
RUN apt-get update && apt-get install -y  --no-install-recommends build-essential gcc git ca-certificates wget ninja-build gdb openssh-client clang-format graphviz

# Install cmake
RUN mkdir cmake && cd cmake && wget https://cmake.org/files/v3.14/cmake-3.14.0-Linux-x86_64.sh \
    && bash cmake-3.14.0-Linux-x86_64.sh --skip-license \
    && cp -r bin /usr/ && rm -r bin \
    && cp -r share /usr/ && rm -r share \
    && cp -r doc /usr/share/ && rm -r doc \
    && cp -r man /usr/share/ && rm -r man \
    && cd .. && rm -r cmake
        
#Install gperftools
RUN apt-get update && apt-get install -y --no-install-recommends libgoogle-perftools-dev

#Install Glog
RUN apt-get update && apt-get install -y --no-install-recommends libgoogle-glog-dev

#Install  libraries
RUN apt-get update && apt-get install -y --no-install-recommends libboost-dev libboost-program-options-dev libboost-regex-dev libboost-system-dev libssl-dev libncurses5-dev libcpprest-dev libjemalloc-dev  libncurses5-dev libncursesw5-dev libreadline-dev


#Cleanup
RUN apt-get purge -y wget && apt-get clean && rm -rf /var/lib/apt/lists/*