FROM alpine:3.8

RUN apk add --no-cache gcc g++ make upx musl-dev boost-dev curl-dev
WORKDIR /ot
