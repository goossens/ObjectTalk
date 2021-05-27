#!/bin/#!/usr/bin/env bash

#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

#
#	Helpers
#

function die() {
	tput setaf 1
	echo "$0: $*"
	tput sgr0
	exit 1
}

#
#	Ensure homebrew is installed
#

if [ -z "$(which brew 2> /dev/null)" ]
then
	die "Homebrew is not installed on your system. Please see https://brew.sh for instructions."
fi

#
#	Install required packages
#

brew install cmake libuv
