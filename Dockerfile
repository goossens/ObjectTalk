FROM alpine:3.13

RUN apk add --no-cache \
	gcc \
	g++ \
	gdb \
	make \
	cmake \
	musl-dev \
	libuv-dev \
	alpine-sdk \
	linux-headers

WORKDIR /ot
