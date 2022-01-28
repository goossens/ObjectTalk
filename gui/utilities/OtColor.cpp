//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "OtException.h"
#include "OtText.h"

#include "OtColor.h"


//
//	Internal color representation
//

struct OtRgbColor {
	OtRgbColor() = default;
	OtRgbColor(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
	float r = 0, g = 0, b = 0, a = 0;
};


//
//	Lookup table (name to color)
//

struct OtNamedColor {
	const char* name;
	int r;
	int g;
	int b;
};

static OtNamedColor namedColors[] = {
	{ "transparent", 0, 0, 0 }, { "aliceblue", 240, 248, 255 },
	{ "antiquewhite", 250, 235, 215 }, { "aqua", 0, 255, 255 },
	{ "aquamarine", 127, 255, 212 }, { "azure", 240, 255, 255 },
	{ "beige", 245, 245, 220 }, { "bisque", 255, 228, 196 },
	{ "black", 0, 0, 0 }, { "blanchedalmond", 255, 235, 205 },
	{ "blue", 0, 0, 255 }, { "blueviolet", 138, 43, 226 },
	{ "brown", 165, 42, 42 }, { "burlywood", 222, 184, 135 },
	{ "cadetblue", 95, 158, 160 }, { "chartreuse", 127, 255, 0 },
	{ "chocolate", 210, 105, 30 }, { "coral", 255, 127, 80 },
	{ "cornflowerblue", 100, 149, 237 }, { "cornsilk", 255, 248, 220 },
	{ "crimson", 220, 20, 60 }, { "cyan", 0, 255, 255 },
	{ "darkblue", 0, 0, 139 }, { "darkcyan", 0, 139, 139 },
	{ "darkgoldenrod", 184, 134, 11 }, { "darkgray", 169, 169, 169 },
	{ "darkgreen", 0, 100, 0 }, { "darkgrey", 169, 169, 169 },
	{ "darkkhaki", 189, 183, 107 }, { "darkmagenta", 139, 0, 139 },
	{ "darkolivegreen", 85, 107, 47 }, { "darkorange", 255, 140, 0 },
	{ "darkorchid", 153, 50, 204 }, { "darkred", 139, 0, 0 },
	{ "darksalmon", 233, 150, 122 }, { "darkseagreen", 143, 188, 143 },
	{ "darkslateblue", 72, 61, 139 }, { "darkslategray", 47, 79, 79 },
	{ "darkslategrey", 47, 79, 79 }, { "darkturquoise", 0, 206, 209 },
	{ "darkviolet", 148, 0, 211 }, { "deeppink", 255, 20, 147 },
	{ "deepskyblue", 0, 191, 255 }, { "dimgray", 105, 105, 105 },
	{ "dimgrey", 105, 105, 105 }, { "dodgerblue", 30, 144, 255 },
	{ "firebrick", 178, 34, 34 }, { "floralwhite", 255, 250, 240 },
	{ "forestgreen", 34, 139, 34 }, { "fuchsia", 255, 0, 255 },
	{ "gainsboro", 220, 220, 220 }, { "ghostwhite", 248, 248, 255 },
	{ "gold", 255, 215, 0 }, { "goldenrod", 218, 165, 32 },
	{ "gray", 128, 128, 128 }, { "green", 0, 128, 0 },
	{ "greenyellow", 173, 255, 47 }, { "grey", 128, 128, 128 },
	{ "honeydew", 240, 255, 240 }, { "hotpink", 255, 105, 180 },
	{ "indianred", 205, 92, 92 }, { "indigo", 75, 0, 130 },
	{ "ivory", 255, 255, 240 }, { "khaki", 240, 230, 140 },
	{ "lavender", 230, 230, 250 }, { "lavenderblush", 255, 240, 245 },
	{ "lawngreen", 124, 252, 0 }, { "lemonchiffon", 255, 250, 205 },
	{ "lightblue", 173, 216, 230 }, { "lightcoral", 240, 128, 128 },
	{ "lightcyan", 224, 255, 255 }, { "lightgoldenrodyellow", 250, 250, 210 },
	{ "lightgray", 211, 211, 211 }, { "lightgreen", 144, 238, 144 },
	{ "lightgrey", 211, 211, 211 }, { "lightpink", 255, 182, 193 },
	{ "lightsalmon", 255, 160, 122 }, { "lightseagreen", 32, 178, 170 },
	{ "lightskyblue", 135, 206, 250 }, { "lightslategray", 119, 136, 153 },
	{ "lightslategrey", 119, 136, 153 }, { "lightsteelblue", 176, 196, 222 },
	{ "lightyellow", 255, 255, 224 }, { "lime", 0, 255, 0 },
	{ "limegreen", 50, 205, 50 }, { "linen", 250, 240, 230 },
	{ "magenta", 255, 0, 255 }, { "maroon", 128, 0, 0 },
	{ "mediumaquamarine", 102, 205, 170 }, { "mediumblue", 0, 0, 205 },
	{ "mediumorchid", 186, 85, 211 }, { "mediumpurple", 147, 112, 219 },
	{ "mediumseagreen", 60, 179, 113 }, { "mediumslateblue", 123, 104, 238 },
	{ "mediumspringgreen", 0, 250, 154 }, { "mediumturquoise", 72, 209, 204 },
	{ "mediumvioletred", 199, 21, 133 }, { "midnightblue", 25, 25, 112 },
	{ "mintcream", 245, 255, 250 }, { "mistyrose", 255, 228, 225 },
	{ "moccasin", 255, 228, 181 }, { "navajowhite", 255, 222, 173 },
	{ "navy", 0, 0, 128 }, { "oldlace", 253, 245, 230 },
	{ "olive", 128, 128, 0 }, { "olivedrab", 107, 142, 35 },
	{ "orange", 255, 165, 0 }, { "orangered", 255, 69, 0 },
	{ "orchid", 218, 112, 214 }, { "palegoldenrod", 238, 232, 170 },
	{ "palegreen", 152, 251, 152 }, { "paleturquoise", 175, 238, 238 },
	{ "palevioletred", 219, 112, 147 }, { "papayawhip", 255, 239, 213 },
	{ "peachpuff", 255, 218, 185 }, { "peru", 205, 133, 63 },
	{ "pink", 255, 192, 203 }, { "plum", 221, 160, 221 },
	{ "powderblue", 176, 224, 230 }, { "purple", 128, 0, 128 },
	{ "red", 255, 0, 0 }, { "rosybrown", 188, 143, 143 },
	{ "royalblue", 65, 105, 225 }, { "saddlebrown", 139, 69, 19 },
	{ "salmon", 250, 128, 114 }, { "sandybrown", 244, 164, 96 },
	{ "seagreen", 46, 139, 87 }, { "seashell", 255, 245, 238 },
	{ "sienna", 160, 82, 45 }, { "silver", 192, 192, 192 },
	{ "skyblue", 135, 206, 235 }, { "slateblue", 106, 90, 205 },
	{ "slategray", 112, 128, 144 }, { "slategrey", 112, 128, 144 },
	{ "snow", 255, 250, 250 }, { "springgreen", 0, 255, 127 },
	{ "steelblue", 70, 130, 180 }, { "tan", 210, 180, 140 },
	{ "teal", 0, 128, 128 }, { "thistle", 216, 191, 216 },
	{ "tomato", 255, 99, 71 }, { "turquoise", 64, 224, 208 },
	{ "violet", 238, 130, 238 }, { "wheat", 245, 222, 179 },
	{ "white", 255, 255, 255 }, { "whitesmoke", 245, 245, 245 },
	{ "yellow", 255, 255, 0 }, { "yellowgreen", 154, 205, 50 }
};


//
//	Split
//

static std::vector<std::string> split(const std::string& str, char delimeter) {
	std::vector<std::string> elements;
	std::stringstream ss(str);
	std::string item;

	while (std::getline(ss, item, delimeter)) {
		elements.push_back(item);
	}

	return elements;
}


//
//	parseParts
//

static std::vector<int> parseParts(const std::string& text, size_t offset, size_t count=0) {
	// create copy of string
	std::string str = text.substr(offset);

	// sanity check
	if (str.front() != '(' || str.back() != ')') {
		throw std::invalid_argument("parentheses missing");
	}

	// extract the parts
	std::vector parts = split(str.substr(1, str.size() - 2), ',');

	// sanity check
	if (count && parts.size() != count) {
		throw std::length_error("wrong number of parameters");
	}

	// parse each part
	std::vector<int> result;

	for (const auto& part : parts) {
		if (part.back() == '%') {
			result.push_back(std::stoi(part) * 255 / 100);

		} else {
			result.push_back(std::stoi(part));
		}
	}

	// we're done
	return result;
}


//
//	hexadecimal2rgb
//

static OtRgbColor hexadecimal2rgb(const std::string& text) {
	if (text.length() == 4) {
		int iv = std::stoi(text.substr(1), 0, 16);

		return OtRgbColor(
			static_cast<uint8_t>(((iv & 0xf00) >> 4) | ((iv & 0xf00) >> 8)) / 255.0,
			static_cast<uint8_t>((iv & 0xf0) | ((iv & 0xf0) >> 4)) / 255.0,
			static_cast<uint8_t>((iv & 0xf) | ((iv & 0xf) << 4)) / 255.0,
			1.0);

	} else if (text.length() == 7) {
		int iv = std::stoi(text.substr(1), 0, 16);

		return OtRgbColor(
			static_cast<uint8_t>((iv & 0xff0000) >> 16) / 255.0,
			static_cast<uint8_t>((iv & 0xff00) >> 8) / 255.0,
			static_cast<uint8_t>(iv & 0xff) / 255.0,
			1.0);

	} else {
		throw std::length_error("hexadecimal colors are 3 or 6 digits");
	}
}


//
//	rgb2rgb
//

static OtRgbColor rgb2rgb(const std::string& text) {
	if (text.rfind("rgb(", 0) == 0) {
		std::vector<int> parts = parseParts(text, 3, 3);
		return OtRgbColor(parts[0] / 255.0, parts[1] / 255.0, parts[2] / 255.0, 1.0);

	} else {
		std::vector<int> parts = parseParts(text, 4, 4);
		return OtRgbColor(parts[0] / 255.0, parts[1] / 255.0, parts[2] / 255.0, parts[3] / 255.0);
	}
}


//
//	hue2rgb
//

static float hue2rgb(float p, float q, float t) {
	if (t < 0.0) t += 1.0;
	if (t > 1.0) t -= 1.0;
	if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
	if (t < 1.0 / 2.0) return q;
	if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
	return p;
}


//
//	hsl2rgb
//

static OtRgbColor hsl2rgb(const std::string& text) {
	std::vector<int> parts = parseParts(text, 3);

	if (parts.size() == 3) {
		parts.push_back(255);

	} else if (parts.size() != 4) {
		throw std::length_error("wrong number of parameters");
	}

	float h = parts[0] / 255.0;
	float s = parts[1] / 255.0;
	float l = parts[2] / 255.0;
	float a = parts[3] / 255.0;

	if (s == 0.0) {
		return OtRgbColor(1.0, 1.0, 1.0, a);

	} else {
		float q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
		float p = 2.0 * l - q;

		return OtRgbColor(
			hue2rgb(p, q, h + 1.0 / 3.0),
			hue2rgb(p, q, h),
			hue2rgb(p, q, h - 1.0 / 3.0),
			a);
	}
}


//
//	name2rgb
//

static OtRgbColor name2rgb(const std::string& text) {
	for (const auto& namedColor : namedColors) {
		if (text == namedColor.name) {
			return OtRgbColor(
				namedColor.r / 255.0,
				namedColor.g / 255.0,
				namedColor.b / 255.0,
				1.0);
		}
	}

	throw std::invalid_argument("unknown name");
}


//
//	parse
//

static OtRgbColor parse(const std::string& text) {
	try {
		// remove all whitespaces
		std::string str = OtText::removeAll(text);

		// convert to lowercase
		str = OtText::lower(str);

		// handle hexadecimal colors
		if (str.length() && str.front() == '#') {
			return hexadecimal2rgb(str);

		} else if (str.rfind("rgb(", 0) == 0 || str.rfind("rgba(", 0) == 0) {
			return rgb2rgb(str);

		} else if (str.rfind("hsl(", 0) == 0) {
			return hsl2rgb(str);

		} else {
			return name2rgb(str);
		}

	} catch (const std::exception& e) {
		OtExcept("Invalid color [%s], error: %s", text.c_str(), e.what());
	}

	// we never get her but we need to please the compiler
	return OtRgbColor(0, 0, 0, 0);
}


//
//	OtColorParseToVec3
//

glm::vec3 OtColorParseToVec3(const std::string& css) {
	OtRgbColor color = parse(css);
	return glm::vec3(color.r, color.g, color.b);
}


//
//	OtColorParseToVec4
//

glm::vec4 OtColorParseToVec4(const std::string& css) {
	OtRgbColor color = parse(css);
	return glm::vec4(color.r, color.g , color.b, color.a);
}


//
//	OtColorParseToUint32
//

uint32_t OtColorParseToUint32(const std::string& color) {
	glm::vec4 c = OtColorParseToVec4(color);
	int r = c.r * 255.0;
	int g = c.g * 255.0;
	int b = c.b * 255.0;
	int a = c.a * 255.0;
	return a << 24 | b << 16 | g << 8 | r;
}
