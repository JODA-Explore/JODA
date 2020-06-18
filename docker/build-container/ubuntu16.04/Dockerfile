# Set the base image
FROM ubuntu:xenial
# Dockerfile author / maintainer
MAINTAINER Nico Schaefer <n_schaefer11@cs.uni-kl.de>
ARG BUILD_PACKAGES="build-essential software-properties-common git ca-certificates wget gcc-8 g++-8 make file graphviz"
ARG DEPENDENCY_PACKAGES="libgoogle-perftools-dev libboost-dev libboost-program-options-dev libboost-regex-dev libboost-system-dev libcpprest-dev libgoogle-glog-dev libjemalloc-dev libncurses5-dev libncursesw5-dev libreadline-dev libssl-dev"

RUN apt-get update && apt-get install -y  --fix-missing  --no-install-recommends software-properties-common

#Install GCC-8 repository
RUN add-apt-repository ppa:ubuntu-toolchain-r/test 

# Install build tools
RUN apt-get update && apt-get install -y  --fix-missing  --no-install-recommends $BUILD_PACKAGES

# Install gcovr
RUN apt-get update && apt-get install -y --no-install-recommends python3-pip python3-setuptools \
    && pip3 install wheel \
    && pip3 install git+https://github.com/gcovr/gcovr.git

# Install cmake
RUN mkdir cmake && cd cmake && wget https://cmake.org/files/v3.14/cmake-3.14.0-Linux-x86_64.sh \
    && bash cmake-3.14.0-Linux-x86_64.sh --skip-license \
    && cp -r bin /usr/ && rm -r bin \
    && cp -r share /usr/ && rm -r share \
    && cp -r doc /usr/share/ && rm -r doc \
    && cp -r man /usr/share/ && rm -r man \
    && cd .. && rm -r cmake


# Install Doxygen
RUN apt-get update && apt-get install -y --no-install-recommends flex bison \
    && git clone https://github.com/doxygen/doxygen.git \
    && cd doxygen && mkdir -p build && cd build \
    && cmake -G "Unix Makefiles" .. \
    && make \
    && make install \
    && cd ../.. && rm -r doxygen

#Install  dependencies
RUN apt-get install -y --no-install-recommends $DEPENDENCY_PACKAGES


#Cleanup
RUN apt-get purge -y wget && apt-get clean && rm -rf /var/lib/apt/lists/*

# Set compiler
RUN  update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 30 \
 && update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-8 30 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-8 30 \
 && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-8 30 \
 && update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-8 30
