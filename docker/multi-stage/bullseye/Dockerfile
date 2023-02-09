# Set the base image
FROM debian:bullseye
# Dockerfile author / maintainer
LABEL maintainer="Nico Schaefer <n_schaefer11@cs.uni-kl.de>"
ARG BUILD_PACKAGES="build-essential software-properties-common cmake git ca-certificates gcc g++ make file"
ARG BUILD_DEPENDENCY_PACKAGES="libboost-dev libboost-program-options-dev libboost-regex-dev libboost-system-dev libcpprest-dev libjemalloc-dev libncurses5-dev libncursesw5-dev libreadline-dev libssl-dev libpython3-dev"

ENV DEBIAN_FRONTEND=noninteractive 

# Install build tools
RUN apt-get update && apt-get install -y  --fix-missing  --no-install-recommends $BUILD_PACKAGES


#Install  dependencies
RUN apt-get update && apt-get install -y --no-install-recommends $BUILD_DEPENDENCY_PACKAGES


#Cleanup
RUN apt-get clean && rm -rf /var/lib/apt/lists/*



# Compile
COPY . /app
WORKDIR /app
RUN mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(nproc)



# Set the base image
FROM debian:bullseye
# Dockerfile author / maintainer
LABEL maintainer="Nico Schaefer <n_schaefer11@cs.uni-kl.de>"
ARG DEPENDENCY_PACKAGES="libjemalloc2 ca-certificates libcpprest libssl1.1 libreadline8 libncursesw6 libgoogle-glog0v5 python3 libpython3-dev python3-pip build-essential"

ENV DEBIAN_FRONTEND=noninteractive 

# Copy JODA executable
COPY --from=0 /app/build/apps/joda/joda /bin/joda
COPY --from=0 /app/build/apps/client/joda-client /bin/joda-client

#Install JODA
RUN apt-get update && apt-get install --no-install-recommends -y $DEPENDENCY_PACKAGES && apt-get clean && rm -rf /var/lib/apt/lists/*

# Copy config
# COPY docker/application-container/bullseye/joda.conf joda.conf

# JODA PORT
EXPOSE 5632/tcp

# Start JODA server
ENTRYPOINT ["joda"]
CMD ["--server", "--bind", "0.0.0.0", "--logtostderr", "-c"]