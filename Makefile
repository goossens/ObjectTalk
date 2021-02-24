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

clean:
	rm -rf debug release xcode

docker:
	docker build -t ot .
	docker run --interactive --tty --name ot --publish 8080:80 --volume $(shell pwd):/ot ot sh
	docker container rm ot

dockerclean:
	docker container rm ot; true
	docker image rm ot; true

apk:
	abuild -F release
