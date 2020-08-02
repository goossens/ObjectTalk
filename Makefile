SRC=$(wildcard app/*.cpp modules/*/*.cpp)
INC=$(wildcard include/*.h modules/*/*.h)

.PHONY : debug release

debug:
	cmake -Bdebug
	cd debug && make

test: debug
	cd debug && make test

release:
	cmake -Brelease -DCMAKE_BUILD_TYPE=Release
	cd release && make

install: release
	cd release && make install/strip

xcode:
	cmake -G Xcode --build build

cleanup:
	perl -i -pe 's/\s+\n/\n/' $(SRC) $(INC)
	ls $(SRC) $(INC) | xargs -o -n 1 vim -c 'set ts=4|set noet|%retab!|wq'

clean:
	rm -rf debug release

docker:
	docker build -t ot .
	docker run --interactive --tty --name ot --volume $(shell pwd):/ot ot sh
	docker container rm ot

dockerclean:
	docker container rm ot; true
	docker image rm ot; true

apk:
	abuild -F release
