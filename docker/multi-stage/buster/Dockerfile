# Set the base image
FROM debian:buster-backports
# Dockerfile author / maintainer
LABEL maintainer="Nico Schaefer <n_schaefer11@cs.uni-kl.de>"
ARG BUILD_PACKAGES="build-essential software-properties-common cmake/buster-backports git ca-certificates gcc-8 g++-8 make file"
ARG BUILD_DEPENDENCY_PACKAGES="libboost-dev libboost-program-options-dev libboost-regex-dev libboost-system-dev libcpprest-dev libgoogle-glog-dev libjemalloc-dev libncurses5-dev libncursesw5-dev libreadline-dev libssl-dev"

ENV DEBIAN_FRONTEND=noninteractive 

# Install build tools
RUN apt-get update && apt-get install -y  --fix-missing  --no-install-recommends $BUILD_PACKAGES


#Install  dependencies
RUN apt-get update && apt-get install -y --no-install-recommends $BUILD_DEPENDENCY_PACKAGES


#Cleanup
RUN apt-get clean && rm -rf /var/lib/apt/lists/*

# Set compiler
RUN  update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 30 \
 && update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-8 30 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-8 30 \
 && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-8 30 

 # Compile
 COPY . /app
 WORKDIR /app
 RUN mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(nproc)



# Set the base image
FROM debian:buster
# Dockerfile author / maintainer
LABEL maintainer="Nico Schaefer <n_schaefer11@cs.uni-kl.de>"
ARG DEPENDENCY_PACKAGES="libjemalloc2 libcpprest libssl1.1 libreadline7 libncursesw5 libgoogle-glog0v5"

ENV DEBIAN_FRONTEND=noninteractive 

# Copy JODA executable
COPY --from=0 /app/build/apps/joda/joda /bin/joda
COPY --from=0 /app/build/apps/client/joda-client /bin/joda-client

#Install JODA
RUN apt-get update && apt-get install -y $DEPENDENCY_PACKAGES
#Cleanup
RUN apt-get clean && rm -rf /var/lib/apt/lists/*

# Copy config
COPY docker/application-container/buster/joda.conf joda.conf

# JODA PORT
EXPOSE 5632/tcp

# Start JODA server
ENTRYPOINT ["joda"]
CMD ["--server", "--bind", "0.0.0.0", "--logtostderr", "-c"]