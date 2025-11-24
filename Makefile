#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

.PHONY: debug
debug: ninja
	cmake --build $(HOME)/build/ObjectTalk --config Debug

.PHONY: release
release: ninja
	cmake --build $(HOME)/build/ObjectTalk --config Release

.PHONY: ninja
ninja:
	cmake -B $(HOME)/build/ObjectTalk -G "Ninja Multi-Config"

.PHONY: xcode
xcode:
	cmake -B $(HOME)/build/ObjectTalk/xcode -G Xcode

.PHONY: vs
vs:
	cmake -B $(HOME)/build/ObjectTalk/vs -G "Visual Studio 17 2022" -A x64
	cmake --build $(HOME)/build/ObjectTalk

.PHONY: test
test: debug
	ctest --test-dir $(HOME)/build/ObjectTalk --build-config Debug --output-on-failure

.PHONY: rtest
rtest: release
	ctest --test-dir $(HOME)/build/ObjectTalk --build-config Release --output-on-failure

.PHONY: docs
docs:
	pugger --recursive --theme manual --assets --out docs docs-src

.PHONY: clean
clean:
	rm -rf $(HOME)/build/ObjectTalk
