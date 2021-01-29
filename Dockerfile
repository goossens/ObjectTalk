FROM alpine:3.13

RUN apk add --no-cache \
	gcc \
	g++ \
	make \
	cmake \
	musl-dev \
	libuv-dev \
	alpine-sdk

WORKDIR /ot
