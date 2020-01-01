make:
	cmake -Bbuild
	cd build && make

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
	docker image rm ot

apk:
	abuild -F build
