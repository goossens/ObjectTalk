FROM alpine:3.12

RUN apk add --no-cache gcc g++ make cmake musl-dev libuv-dev alpine-sdk
WORKDIR /ot
