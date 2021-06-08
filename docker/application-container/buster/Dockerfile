# Set the base image
FROM debian:buster
# Dockerfile author / maintainer
LABEL maintainer="Nico Schaefer <n_schaefer11@cs.uni-kl.de>"

ENV DEBIAN_FRONTEND=noninteractive 
# Copy JODA executable
COPY *.deb .

#Install JODA
RUN apt-get update && apt install -y ./*.deb
#Cleanup
RUN apt-get clean && rm -rf /var/lib/apt/lists/*

# Copy config
COPY docker/application-container/buster/joda.conf joda.conf

# JODA PORT
EXPOSE 5632/tcp

# Start JODA server
CMD ["joda","--server", "--bind", "0.0.0.0", "--logtostderr", "-c"]