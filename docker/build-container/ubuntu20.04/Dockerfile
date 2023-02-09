# Set the base image
FROM ubuntu:20.04
# Dockerfile author / maintainer
LABEL maintainer="Nico Schaefer <n_schaefer11@cs.uni-kl.de>"
ARG BUILD_PACKAGES="build-essential software-properties-common cmake doxygen gcovr git ca-certificates gcc-10 g++-10 make file graphviz"
ARG DEPENDENCY_PACKAGES="libgoogle-perftools-dev libboost-dev libboost-program-options-dev libboost-regex-dev libboost-system-dev libcpprest-dev libjemalloc-dev libncurses5-dev libncursesw5-dev libreadline-dev libssl-dev libpython3-dev"


ENV DEBIAN_FRONTEND=noninteractive 

# Install build tools
RUN apt-get update && apt-get install -y  --fix-missing  --no-install-recommends $BUILD_PACKAGES

#Install  dependencies
RUN apt-get install -y --no-install-recommends $DEPENDENCY_PACKAGES

#Cleanup
RUN apt-get clean && rm -rf /var/lib/apt/lists/*

# Set compiler
RUN  update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 30 \
  && update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-10 30 \
  && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 30 \
  && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-10 30 \
  && update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-10 30
