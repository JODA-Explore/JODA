
FROM ubuntu:20.04
# Dockerfile author / maintainer
LABEL maintainer="Nico Schaefer <n_schaefer11@cs.uni-kl.de>"
ARG BUILD_PACKAGES="build-essential software-properties-common cmake doxygen gcovr git ca-certificates gcc-9 g++-9 make file graphviz openssh-client clang-format ninja-build gdb "
ARG DEPENDENCY_PACKAGES="libgoogle-perftools-dev libboost-dev libboost-program-options-dev libboost-regex-dev libboost-system-dev libcpprest-dev libgoogle-glog-dev libjemalloc-dev libncurses5-dev libncursesw5-dev libreadline-dev libssl-dev"

ENV DEBIAN_FRONTEND=noninteractive 

# Install build tools
RUN apt-get update && apt-get install -y  --fix-missing  --no-install-recommends $BUILD_PACKAGES $DEPENDENCY_PACKAGES

#Cleanup
RUN apt-get clean && rm -rf /var/lib/apt/lists/*

# Set compiler
RUN  update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 30 \
 && update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-9 30 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 30 \
 && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-9 30 \
 && update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-9 30