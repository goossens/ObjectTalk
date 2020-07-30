FROM alpine:3.12

RUN apk add --no-cache \
  gcc \
  g++ \
  make \
  cmake \
  musl-dev \
  libuv-dev \
  http-parser-dev \
  alpine-sdk

WORKDIR /ot
