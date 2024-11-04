#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

SRC=$(wildcard app/*.cpp libot/*/*.cpp modules/*/*.cpp  modules/*/*/*.cpp)
INC=$(wildcard libot/include/*.h libot/include/ot/*.h modules/*/*.h modules/*/*/*.h)
TST=$(wildcard tests/*/*.ot)

.PHONY: debug
debug:
	cmake -Bdebug -DCMAKE_BUILD_TYPE=Debug
	cd debug && make

.PHONY: release
release:
	cmake -Brelease -DCMAKE_BUILD_TYPE=Release
	cd release && make

.PHONY: shaders
shaders:
	./gui/shaders/update

.PHONY: shaders-force
shaders-force:
	./gui/shaders/update --force

.PHONY: xcode
xcode:
	cmake -Bxcode -GXcode

.PHONY: vs
vs:
	cmake -Bvs -G "Visual Studio 17 2022" -A x64
	cd vs && cmake --build .

.PHONY: test
test: debug
	cd debug && make test

.PHONY: rtest
rtest: release
	cd release && make test

.PHONY: docs
docs:
	pugger --theme manual --assets --out docs docs-src

.PHONY: cleanup
cleanup:
	perl -i -pe 's/\s+\n/\n/' $(SRC) $(INC) $(TST)
	ls $(SRC) $(INC) $(TST) | xargs -o -n 1 vim -c 'set ts=4|set noet|%retab!|wq'

.PHONY: clean
clean:
	rm -rf build debug release xcode vs
