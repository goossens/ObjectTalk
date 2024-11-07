#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

SRC = $(wildcard *.cpp */*.cpp */*/*.cpp */*/*.glsl)
INC = $(wildcard *.h */*.h */*/*.h)
TST = $(wildcard tests/*/*.ot)

ifeq ($(OS),Windows_NT)
    SYSTEM = Windows
else
    SYSTEM = $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

.PHONY: debug
debug: ninja
	cmake --build build/$(SYSTEM) --config Debug

.PHONY: release
release: ninja
	cmake --build build/$(SYSTEM) --config Release

.PHONY: ninja
ninja:
	cmake -B build/$(SYSTEM) -G "Ninja Multi-Config"

.PHONY: shaders
shaders:
	./shaders/update

.PHONY: shaders-force
shaders-force:
	./shaders/update --force

.PHONY: xcode
xcode:
	cmake -B build/xcode -G Xcode

.PHONY: vs
vs:
	cmake -B build/vs -G "Visual Studio 17 2022" -A x64
	cmake --build build/vs

.PHONY: test
test: debug
	ctest --test-dir build/$(SYSTEM) --build-config Debug --output-on-failure

.PHONY: rtest
rtest: release
	ctest --test-dir build/$(SYSTEM) --build-config Release --output-on-failure

.PHONY: docs
docs:
	pugger --theme manual --assets --out docs docs-src

.PHONY: cleanup
cleanup:
	perl -i -pe 's/\s+\n/\n/' $(SRC) $(INC) $(TST)
	ls $(SRC) $(INC) $(TST) | xargs -o -n 1 vim -c 'set ts=4|set noet|%retab!|wq'

.PHONY: clean
clean:
	rm -rf build
