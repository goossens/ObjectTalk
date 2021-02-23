//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.


static std::map<std::string, std::string> OtMimeType = {
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

inline std::string OtMimeTypeGet(std::string extension) {
	if (OtMimeType.find(extension) != OtMimeType.end()) {
		return OtMimeType[extension];

	} else {
		return "application/octet-stream";
	}

}
