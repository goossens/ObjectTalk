#	ObjectTalk Scripting Language
#	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FROM alpine:latest

RUN apk add --no-cache \
	gcc \
	g++ \
	gdb \
	make \
	cmake \
	musl-dev \
	alpine-sdk \
	linux-headers \
	openssh-client

WORKDIR /ot
