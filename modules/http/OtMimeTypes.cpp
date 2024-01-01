//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>
#include <unordered_map>

#include "OtMimeTypes.h"


//
//	Globals
//

static std::unordered_map<std::string, std::string> OtMimeType = {
	{"css", "text/css"},
	{"csv", "text/csv"},

	{"txt", "text/plain"},
	{"vtt", "text/vtt"},
	{"htm", "text/html"},
	{"html", "text/html"},

	{"apng", "image/apng"},
	{"avif", "image/avif"},
	{"bmp", "image/bmp"},
	{"gif", "image/gif"},
	{"png", "image/png"},
	{"svg", "image/svg+xml"},
	{"webp", "image/webp"},
	{"ico", "image/x-icon"},
	{"tif", "image/tiff"},
	{"tiff", "image/tiff"},
	{"jpg", "image/jpeg"},
	{"jpeg", "image/jpeg"},

	{"mp4", "video/mp4"},
	{"mpeg", "video/mpeg"},
	{"webm", "video/webm"},

	{"mp3", "audio/mp3"},
	{"mpga", "audio/mpeg"},
	{"weba", "audio/webm"},
	{"wav", "audio/wave"},

	{"otf", "font/otf"},
	{"ttf", "font/ttf"},
	{"woff", "font/woff"},
	{"woff2", "font/woff2"},

	{"7z", "application/x-7z-compressed"},
	{"atom", "application/atom+xml"},
	{"pdf", "application/pdf"},
	{"js", "application/javascript"},
	{"mjs", "application/javascript"},
	{"json", "application/json"},
	{"rss", "application/rss+xml"},
	{"tar", "application/x-tar"},
	{"xhtml", "application/xhtml+xml"},
	{"xslt", "application/xslt+xml"},
	{"xml", "application/xml"},
	{"gz", "application/gzip"},
	{"tgz", "application/gzip"},
	{"zip", "application/zip"},
	{"wasm", "application/wasm"}
};


//
//	OtMimeTypeGet
//

std::string OtMimeTypeGet(std::string extension) {
	if (OtMimeType.find(extension) != OtMimeType.end()) {
		return OtMimeType[extension];

	} else {
		return "application/octet-stream";
	}
}
