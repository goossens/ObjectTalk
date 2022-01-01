FROM ubuntu:latest

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

RUN apt-get update && apt-get install -y \
	gcc \
	g++ \
	gdb \
	git \
	make \
	cmake \
	valgrind

WORKDIR /ot
