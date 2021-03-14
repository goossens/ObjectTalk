SRC=$(wildcard app/*.cpp libot/src/*.cpp modules/*/*.cpp)
INC=$(wildcard libot/include/*.h libot/include/ot/*.h modules/*/*.h)

.PHONY : debug release xcode alpine ubuntu

debug:
	cmake -Bdebug
	cd debug && make

release:
	cmake -Brelease -DCMAKE_BUILD_TYPE=Release
	cd release && make

xcode:
	cmake -Bxcode -GXcode

test: debug
	cd debug && make test

rtest: release
	cd release && make test

install: release
	cd release && make install/strip
	ls -l /usr/local/bin/ot
	ls -l /usr/local/lib/libot.*
	ls -l /usr/local/lib/ot

xcode:
cleanup:
	perl -i -pe 's/\s+\n/\n/' $(SRC) $(INC)
	ls $(SRC) $(INC) | xargs -o -n 1 vim -c 'set ts=4|set noet|%retab!|wq'

alpine:
	cd docker/alpine && ./run

ubuntu:
	cd docker/ubuntu && ./run

clean:
	rm -rf debug release xcode

distclean: clean
	cd docker/alpine && ./clean
	cd docker/ubuntu && ./clean
