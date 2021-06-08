# Set the base image
FROM debian:buster-backports
# Dockerfile author / maintainer
LABEL maintainer="Nico Schaefer <n_schaefer11@cs.uni-kl.de>"
ARG BUILD_PACKAGES="build-essential software-properties-common git gcovr cmake/buster-backports ca-certificates wget gcc-8 g++-8 make file graphviz"
ARG DEPENDENCY_PACKAGES="libgoogle-perftools-dev libboost-dev libboost-program-options-dev libboost-regex-dev libboost-system-dev libcpprest-dev libgoogle-glog-dev libjemalloc-dev libncurses5-dev libncursesw5-dev libreadline-dev libssl-dev"

ENV DEBIAN_FRONTEND=noninteractive 

# Install build tools
RUN apt-get update && apt-get install -y  --fix-missing  --no-install-recommends $BUILD_PACKAGES



# Install Doxygen
RUN apt-get update && apt-get install -y --no-install-recommends flex bison \
    && git clone https://github.com/doxygen/doxygen.git \
    && cd doxygen && mkdir -p build && cd build \
    && cmake -G "Unix Makefiles" .. \
    && make \
    && make install \
    && cd ../.. && rm -r doxygen


#Install  dependencies
RUN apt-get update && apt-get install -y --no-install-recommends $DEPENDENCY_PACKAGES


#Cleanup
RUN apt-get purge -y wget && apt-get clean && rm -rf /var/lib/apt/lists/*

# Set compiler
RUN  update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 30 \
 && update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-8 30 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-8 30 \
 && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-8 30 \
 && update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-8 30
