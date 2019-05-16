FROM alpine:3.9

RUN apk add --no-cache gcc g++ make upx musl-dev boost-dev curl-dev
WORKDIR /ot
