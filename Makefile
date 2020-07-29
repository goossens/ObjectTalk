SRC=$(wildcard app/*.cpp modules/*/*.cpp)
INC=$(wildcard include/*.h modules/*/*.h)

make:
	cmake -Bbuild
	cd build && make

test:
	cd build && make test

xcode:
	cmake -G Xcode --build build

cleanup:
	perl -i -pe 's/\s+\n/\n/' $(SRC) $(INC)
	ls $(SRC) $(INC) | xargs -o -n 1 vim -c 'set ts=4|set noet|%retab!|wq'

clean:
	rm -rf build/*

docker:
	docker build -t ot .
	docker run --interactive --tty --name ot --volume $(shell pwd):/ot ot sh
	docker container rm ot

dockerclean:
	docker container rm ot; true
	docker image rm ot; true

apk:
	abuild -F build
