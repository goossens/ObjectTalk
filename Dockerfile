FROM alpine:3.11

RUN apk add --no-cache gcc g++ make cmake upx musl-dev libuv-dev alpine-sdk
WORKDIR /ot
