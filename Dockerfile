FROM ubuntu:22.04
LABEL maintainer="Bira Lavor <umeneses@42sp.org.br>"
LABEL description="Dockerfile for 42SP's Webserver project"
LABEL version="1.0"

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    cmake \
    git \
    fish \
    libreadline-dev \
    libgtest-dev \
    valgrind

# Build and install GoogleTest
RUN cd /usr/src/gtest \
    && cmake . \
    && make

# # Copy the GoogleTest Library
RUN cd /usr/src/gtest/lib \
    && cp libgtest.a /usr/lib \
    && cp libgtest_main.a /usr/lib

# Set working directory for projects
WORKDIR /app

# Copy project files into the container
COPY . .

# Set git user configuration
# RUN git config --global user.name "Bira Lavor" \
#    && git config --global user.email "umeneses@student.42sp.org.br"

# Set the default shell to fish
SHELL ["/usr/bin/fish", "-c"]

# Default command
CMD [ "/bin/bash" ]