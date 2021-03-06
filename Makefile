SRC=$(wildcard app/*.cpp modules/*/*.cpp)
INC=$(wildcard include/*.h modules/*/*.h)

.PHONY : debug release

debug:
	cmake -Bdebug
	cd debug && make

test: debug
	cd debug && make test

http: debug
	gdb --args ./debug/bin/ot examples/http.ot

leaks: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./debug/bin/ot examples/hello.ot

release:
	cmake -Brelease -DCMAKE_BUILD_TYPE=Release
	cd release && make

install: release
	cd release && make install/strip
	ls -l /usr/local/bin/ot
	ls -l /usr/local/lib/ot

xcode:
	cmake -Bxcode -GXcode

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
	
apk:
	abuild -F release
