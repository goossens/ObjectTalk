//	TextEditor - A syntax highlighting text editor for ImGui
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cctype>

#include "TextEditor.h"


//
//	Fast lookup tables
//

static bool identifierStart[128] = {
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
	 true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true, false, false, false, false,  true,
	false,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
	 true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true, false, false, false, false, false
};

static bool identifierNoneStart[128] = {
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	 true,  true,  true,  true,  true,  true,  true,  true,  true,  true, false, false, false, false, false, false,
	false,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
	 true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true, false, false, false, false,  true,
	false,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
	 true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true, false, false, false, false, false
};

static bool cStylePunctuation[128] = {
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false,  true, false, false, false,  true,  true, false,  true,  true,  true,  true,  true,  true,  true,  true,
	false, false, false, false, false, false, false, false, false, false,  true,  true,  true,  true,  true,  true,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false,  true, false,  true,  true, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false,  true,  true,  true,  true, false,
};

static bool luaStylePunctuation[128] = {
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false,  true, false,  true, false,  true,  true, false,  true,  true,  true,  true,  true,  true,  true,  true,
	false, false, false, false, false, false, false, false, false, false,  true,  true,  true,  true,  true,  true,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false,  true, false,  true,  true, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false,  true,  true,  true,  true, false,
};

static bool jsonStylePunctuation[128] = {
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false,
	false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false,  true, false,  true, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false,  true, false,  true, false, false,
};

static bool markdownStylePunctuation[128] = {
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false,  true, false, false, false, false, false, false,  true,  true,  true,  true, false,  true, false, false,
	false, false, false, false, false, false, false, false, false, false,  true, false,  true,  true,  true, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false,  true, false,  true, false,  true,
	 true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false,  true, false,  true, false,
};


//
//	TextEditor::Iterator::operator*
//

TextEditor::Iterator::reference TextEditor::Iterator::operator*() const {
	return static_cast<Line*>(line)->at(index).character;
}


//
//	TextEditor::Iterator::operator->
//

TextEditor::Iterator::pointer TextEditor::Iterator::operator->() const {
	return &(static_cast<Line*>(line)->at(index).character);
}


//
//	TextEditor::Language::isCStylePunctuation
//

bool TextEditor::Language::isCStylePunctuation(ImWchar character) {
	return character < 127 ? cStylePunctuation[character] : false;
}


//
//	TextEditor::Language::isCStyleWordCharacter
//

bool TextEditor::Language::isCStyleWordCharacter(ImWchar character) {
	return character < 127 ? identifierNoneStart[character] : false;
}


//
//	TextEditor::Language::getCStyleIdentifier
//

TextEditor::Iterator TextEditor::Language::getCStyleIdentifier(Iterator start, Iterator end) {
	if (*start < 128 && identifierStart[*start]) {
		start++;

		while (start < end && *start < 128 && identifierNoneStart[*start]) {
			start++;
		}
	}

	return start;
}


//
//	TextEditor::Language::getCStyleNumber
//

TextEditor::Iterator TextEditor::Language::getCStyleNumber(Iterator start, Iterator end) {
	auto i = start;
	if (i != end && (*i == '-' || *i == '+')) { i++; }
	if (i == end || !std::isdigit(*i)) { return start; }

	if (*i == '0') {
		if (++i == end) { return i;	}

		if (*i == 'b' || *i == 'B') {
			i++;
			if (i == end || !(*i == '0' || *i == '1')) { return start; }
			while (i != end && (*i == '0' || *i == '1')) { i++; }

		} else if (*i == 'o' || *i == 'O') {
			i++;
			if (i == end || !(*i >= '0' && *i <= '7')) { return start; }
			while (i != end && *i >= '0' && *i <= '7') { i++; }

		} else if (*i == 'x' || *i == 'X') {
			i++;
			if (i == end || !std::isxdigit(*i)) { return start; }
			while (i != end && std::isxdigit(*i)) { i++; }

			if (i != end && *i == '.') {
				i++;
				while (i != end && std::isdigit(*i)) { i++; }
			}

			if (i != end && (*i == 'e' || *i == 'E' || *i == 'p'|| *i == 'P')) {
				i++;
				if (i != end && (*i == '-' || *i == '+')) { i++; }
				if (i == end || !std::isdigit(*i)) { return start; }
				while (i != end && std::isdigit(*i)) { i++; }
			}

		} else {
			while (i != end && *i >= '0' && *i <= '7') { i++; }
		}

		while (i != end && (*i == 'u' || *i == 'U' || *i == 'l' || *i == 'L' || *i == 'z' || *i == 'Z')) {
			i++;
		}

	} else {
		while (i != end && std::isdigit(*i)) { i++; }

		if (i != end) {
			bool isFloat = false;

			if (*i == '.') {
				i++;
				while (i != end && std::isdigit(*i)) { i++; }
				isFloat = true;
			}

			if (i != end && (*i == 'e' || *i == 'E')) {
				i++;
				if (i != end && (*i == '-' || *i == '+')) { i++; }
				if (i == end || !std::isdigit(*i)) { return start; }
				while (i != end && std::isdigit(*i)) { i++; }
				isFloat = true;
			}

			if (isFloat) {
				if (i != end && (*i == 'f' || *i == 'F' || *i == 'l' || *i == 'L')) { i++; }

			}else {
				while (*i == 'u' || *i == 'U' || *i == 'l' || *i == 'L') {
					i++;
				}
			}
		}
	}

	return i;
}


//
//	TextEditor::Language::C
//

const TextEditor::Language& TextEditor::Language::C() {
	static bool initialized = false;
	static TextEditor::Language language;

	if (!initialized) {
		language.name = "C";
		language.preprocess = '#';
		language.singleLineComment = "//";
		language.commentStart = "/*";
		language.commentEnd = "*/";
		language.hasSingleQuotedStrings = true;
		language.hasDoubleQuotedStrings = true;
		language.stringEscape = '\\';

		static const char* const keywords[] = {
			"break", "case", "continue", "default", "do", "else", "for", "goto", "if", "return", "sizeof",
			"switch", "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic",
			"_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local"
		};

		static const char* const declarations[] = {
			"auto", "char", "const", "double", "enum", "extern", "float", "inline", "int", "long", "register",
			"restrict", "short", "signed", "static", "struct", "typedef", "union", "unsigned", "void", "volatile"
		};

		static const char* const identifiers[] = {
			"abort", "abs", "acos", "asin", "atan", "atexit", "atof", "atoi", "atol", "ceil", "clock", "cosh",
			"ctime", "div", "exit", "fabs", "floor", "fmod", "getchar", "getenv", "isalnum", "isalpha", "isdigit",
			"isgraph", "ispunct", "isspace", "isupper", "kbhit", "log10", "log2", "log", "memcmp", "modf", "pow",
			"putchar", "putenv", "puts", "rand", "remove", "rename", "sinh", "sqrt", "srand", "strcat", "strcmp",
			"strerror", "time", "tolower", "toupper"
		};

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }
		for (auto& declaration : declarations) { language.declarations.insert(declaration); }
		for (auto& identifier : identifiers) { language.identifiers.insert(identifier); }

		language.isPunctuation = TextEditor::Language::isCStylePunctuation;
		language.isWord = TextEditor::Language::isCStyleWordCharacter;
		language.getIdentifier = TextEditor::Language::getCStyleIdentifier;
		language.getNumber = TextEditor::Language::getCStyleNumber;

		initialized = true;
	}

	return language;
}


//
//	TextEditor::Language::Cpp
//

const TextEditor::Language& TextEditor::Language::Cpp() {
	static bool initialized = false;
	static TextEditor::Language language;

	if (!initialized) {
		language.name = "C++";
		language.preprocess = '#';
		language.singleLineComment = "//";
		language.commentStart = "/*";
		language.commentEnd = "*/";
		language.hasSingleQuotedStrings = true;
		language.hasDoubleQuotedStrings = true;
		language.stringEscape = '\\';

		static const char* const keywords[] = {
			"alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept",
			"bitand", "bitor", "break", "case", "catch", "compl", "const_cast", "continue", "default", "delete",
			"do", "dynamic_cast", "else", "explicit", "export", "extern", "false", "for", "goto", "if", "import",
			"new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "reinterpret_cast", "requires",
			"return", "sizeof", "static_assert", "static_cast", "switch", "synchronized", "this", "thread_local",
			"throw", "true", "try", "while", "xor", "xor_eq"
		};

		static const char* const declarations[] = {
			"auto", "bool", "char", "char16_t", "char32_t", "class", "concept", "const", "constexpr", "decltype",
			"double", "explicit", "export", "extern", "enum", "extern", "float", "friend", "inline", "int", "long",
			"module", "mutable", "namespace", "private", "protected", "public", "register", "restrict", "short",
			"signed", "static", "struct", "template", "typedef", "typeid", "typename", "union", "using", "unsigned",
			"virtual", "void", "volatile", "wchar_t"
		};

		static const char* const identifiers[] = {
			"abort", "abs", "acos", "asin", "atan", "atexit", "atof", "atoi", "atol", "ceil", "clock", "cosh", "ctime",
			"div", "exit", "fabs", "floor", "fmod", "getchar", "getenv", "isalnum", "isalpha", "isdigit", "isgraph",
			"ispunct", "isspace", "isupper", "kbhit", "log10", "log2", "log", "memcmp", "modf", "pow", "printf",
			"sprintf", "snprintf", "putchar", "putenv", "puts", "rand", "remove", "rename", "sinh", "sqrt", "srand",
			"strcat", "strcmp", "strerror", "time", "tolower", "toupper", "std", "string", "vector", "map", "unordered_map",
			"set", "unordered_set", "min", "max"
		};

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }
		for (auto& declaration : declarations) { language.declarations.insert(declaration); }
		for (auto& identifier : identifiers) { language.identifiers.insert(identifier); }

		language.isPunctuation = TextEditor::Language::isCStylePunctuation;
		language.isWord = TextEditor::Language::isCStyleWordCharacter;
		language.getIdentifier = TextEditor::Language::getCStyleIdentifier;
		language.getNumber = TextEditor::Language::getCStyleNumber;

		initialized = true;
	}

	return language;
}


//
//	TextEditor::Language::getCsStyleNumber
//

TextEditor::Iterator TextEditor::Language::getCsStyleNumber(Iterator start, Iterator end) {
	auto i = start;
	if (i != end && (*i == '-' || *i == '+')) { i++; }
	if (i == end || !std::isdigit(*i)) { return start; }

	if (*i == '0') {
		if (++i == end) { return i;	}

		if (*i == 'b' || *i == 'B') {
			i++;
			if (i == end || !(*i == '0' || *i == '1' || *i == '_')) { return start; }
			while (i != end && (*i == '0' || *i == '1' || *i == '_')) { i++; }

		} else if (*i == 'x' || *i == 'X') {
			i++;
			if (i == end || !(std::isxdigit(*i) || *i == '_')) { return start; }
			while (i != end && (std::isxdigit(*i) || *i == '_')) { i++; }

		} else {
			return start;
		}

		while (i != end && (*i == 'u' || *i == 'U' || *i == 'l' || *i == 'L')) {
			i++;
		}

	} else {
		while (i != end && (std::isdigit(*i) || *i == 'z')) { i++; }

		if (i != end) {
			bool isFloat = false;

			if (*i == '.') {
				i++;
				while (i != end && (std::isdigit(*i || *i == 'z'))) { i++; }
				isFloat = true;
			}

			if (i != end && (*i == 'e' || *i == 'E')) {
				i++;
				if (i != end && (*i == '-' || *i == '+')) { i++; }
				if (i == end || !(std::isdigit(*i) || *i == 'z')) { return start; }
				while (i != end && (std::isdigit(*i) || *i == 'z')) { i++; }
				isFloat = true;
			}

			if (isFloat) {
				if (i != end && (*i == 'f' || *i == 'F' || *i == 'd' || *i == 'D' || *i == 'm' || *i == 'M')) { i++; }

			}else {
				while (*i == 'u' || *i == 'U' || *i == 'l' || *i == 'L') {
					i++;
				}
			}
		}
	}

	return i;
}


//
//	TextEditor::Language::Cs
//

const TextEditor::Language& TextEditor::Language::Cs() {
	static bool initialized = false;
	static TextEditor::Language language;

	if (!initialized) {
		language.name = "C#";
		language.preprocess = '#';
		language.singleLineComment = "//";
		language.commentStart = "/*";
		language.commentEnd = "*/";
		language.hasSingleQuotedStrings = true;
		language.hasDoubleQuotedStrings = true;
		language.stringEscape = '\\';

		static const char* const keywords[] = {
			"abstract", "as", "base", "bool", "break", "byte", "case", "catch", "char", "checked", "class", "const", "continue",
			"decimal", "default", "delegate", "do", "double", "else", "enum", "event", "explicit", "extern", "false", "finally",
			"fixed", "float", "for", "foreach", "goto", "if", "implicit", "in", "in (generic modifier)", "int", "interface",
			"internal", "is", "lock", "long", "namespace", "new", "null", "object", "operator", "out", "override", "params",
			"private", "protected", "public", "readonly", "ref", "return", "sbyte", "sealed", "short", "sizeof", "stackalloc",
			"static", "string", "struct", "switch", "this", "throw", "true", "try", "typeof", "uint", "ulong", "unchecked",
			"unsafe", "ushort", "using", "using static", "void", "volatile", "while"
		};

		static const char* const identifiers[] = {
			"add", "alias", "ascending", "async", "await", "descending", "dynamic", "from", "get", "global", "group", "into",
			"join", "let", "orderby", "partial", "remove", "select", "set", "value", "var", "when", "where", "yield"
		};

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }
		for (auto& identifier : identifiers) { language.identifiers.insert(identifier); }

		language.isPunctuation = TextEditor::Language::isCStylePunctuation;
		language.isWord = TextEditor::Language::isCStyleWordCharacter;
		language.getIdentifier = TextEditor::Language::getCStyleIdentifier;
		language.getNumber = TextEditor::Language::getCsStyleNumber;

		initialized = true;
	}

	return language;
}


//
//	TextEditor::Language::AngelScript
//

const TextEditor::Language& TextEditor::Language::AngelScript() {
	static bool initialized = false;
	static TextEditor::Language language;

	if (!initialized) {
		language.name = "AngelScript";
		language.preprocess = '#';
		language.singleLineComment = "//";
		language.commentStart = "/*";
		language.commentEnd = "*/";
		language.hasSingleQuotedStrings = true;
		language.hasDoubleQuotedStrings = true;
		language.stringEscape = '\\';

		static const char* const keywords[] = {
			"and", "abstract", "auto", "bool", "break", "case", "cast", "class", "const", "continue", "default",
			"do", "double", "else", "enum", "false", "final", "float", "for", "from", "funcdef", "function", "get",
			"if", "import", "in", "inout", "int", "interface", "int8", "int16", "int32", "int64", "is", "mixin",
			"namespace", "not", "null", "or", "out", "override", "private", "protected", "return", "set", "shared",
			"super", "switch", "this ", "true", "typedef", "uint", "uint8", "uint16", "uint32", "uint64", "void",
			"while", "xor"
		};

		static const char* const identifiers[] = {
			"cos", "sin", "tab", "acos", "asin", "atan", "atan2", "cosh", "sinh", "tanh", "log", "log10", "pow",
			"sqrt", "abs", "ceil", "floor", "fraction", "closeTo", "fpFromIEEE", "fpToIEEE", "complex", "opEquals",
			"opAddAssign", "opSubAssign", "opMulAssign", "opDivAssign", "opAdd", "opSub", "opMul", "opDiv"
		};

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }
		for (auto& identifier : identifiers) { language.identifiers.insert(identifier); }

		language.isPunctuation = TextEditor::Language::isCStylePunctuation;
		language.isWord = TextEditor::Language::isCStyleWordCharacter;
		language.getIdentifier = TextEditor::Language::getCStyleIdentifier;
		language.getNumber = TextEditor::Language::getCStyleNumber;

		initialized = true;
	}

	return language;
}


//
//	TextEditor::Language::isLuaStylePunctuation
//

bool TextEditor::Language::isLuaStylePunctuation(ImWchar character) {
	return character < 127 ? luaStylePunctuation[character] : false;
}


//
//	TextEditor::Language::getLuaStyleNumber
//

TextEditor::Iterator TextEditor::Language::getLuaStyleNumber(Iterator start, Iterator end) {
	auto i = start;
	if (i != end && (*i == '-' || *i == '+')) { i++; }
	if (i == end || !std::isdigit(*i)) { return start; }

	if (*i == '0') {
		if (++i == end) { return i;	}

		if (*i == 'x' || *i == 'X') {
			i++;
			if (i == end || !std::isxdigit(*i)) { return start; }
			while (i != end && std::isxdigit(*i)) { i++; }

			if (i != end && *i == '.') {
				i++;
				while (i != end && std::isdigit(*i)) { i++; }
			}

			if (i != end && (*i == 'p' || *i == 'P')) {
				i++;
				if (i != end && (*i == '-' || *i == '+')) { i++; }
				if (i == end || !std::isdigit(*i)) { return start; }
				while (i != end && std::isdigit(*i)) { i++; }
			}

		} else {
			return start;
		}

	} else {
		while (i != end && std::isdigit(*i)) { i++; }

		if (i != end) {
			if (*i == '.') {
				i++;
				while (i != end && std::isdigit(*i)) { i++; }

				if (i != end && (*i == 'e'|| *i == 'E')) {
					i++;
					if (i != end && (*i == '-' || *i == '+')) { i++; }
					if (i == end || !std::isdigit(*i)) { return start; }
					while (i != end && std::isdigit(*i)) { i++; }
				}
			}
		}
	}

	return i;
}


//
//	TextEditor::Language::Lua
//

const TextEditor::Language& TextEditor::Language::Lua() {
	static bool initialized = false;
	static TextEditor::Language language;

	if (!initialized) {
		language.name = "Lua";
		language.singleLineComment = "--";
		language.commentStart = "--[[";
		language.commentEnd = "]]";
		language.hasSingleQuotedStrings = true;
		language.hasDoubleQuotedStrings = true;
		language.otherStringStart = "[[";
		language.otherStringEnd = "]]";
		language.stringEscape = '\\';

		static const char* const keywords[] = {
			"and", "break", "do", "else", "elseif", "end", "false", "for", "function", "goto", "if", "in", "local", "nil",
			"not", "or", "repeat", "return", "then", "true", "until", "while"
		};

		static const char* const identifiers[] = {
			"assert", "collectgarbage", "dofile", "error", "getmetatable", "ipairs", "loadfile", "load", "loadstring",
			"next", "pairs", "pcall", "print", "rawequal", "rawlen", "rawget", "rawset", "select", "setmetatable",
			"tonumber", "tostring", "type", "xpcall", "_G", "_VERSION","arshift", "band", "bnot", "bor", "bxor", "btest",
			"extract", "lrotate", "lshift", "replace", "rrotate", "rshift", "create", "resume", "running", "status",
			"wrap", "yield", "isyieldable", "debug","getuservalue", "gethook", "getinfo", "getlocal", "getregistry",
			"getmetatable", "getupvalue", "upvaluejoin", "upvalueid", "setuservalue", "sethook", "setlocal", "setmetatable",
			"setupvalue", "traceback", "close", "flush", "input", "lines", "open", "output", "popen", "read", "tmpfile",
			"type", "write", "close", "flush", "lines", "read", "seek", "setvbuf", "write", "__gc", "__tostring", "abs",
			"acos", "asin", "atan", "ceil", "cos", "deg", "exp", "tointeger", "floor", "fmod", "ult", "log", "max", "min",
			"modf", "rad", "random", "randomseed", "sin", "sqrt", "string", "tan", "type", "atan2", "cosh", "sinh", "tanh",
			"pow", "frexp", "ldexp", "log10", "pi", "huge", "maxinteger", "mininteger", "loadlib", "searchpath", "seeall",
			"preload", "cpath", "path", "searchers", "loaded", "module", "require", "clock", "date", "difftime", "execute",
			"exit", "getenv", "remove", "rename", "setlocale", "time", "tmpname", "byte", "char", "dump", "find", "format",
			"gmatch", "gsub", "len", "lower", "match", "rep", "reverse", "sub", "upper", "pack", "packsize", "unpack",
			"concat", "maxn", "insert", "pack", "unpack", "remove", "move", "sort", "offset", "codepoint", "char", "len",
			"codes", "charpattern", "coroutine", "table", "io", "os", "string", "utf8", "bit32", "math", "debug", "package"
		};

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }
		for (auto& identifier : identifiers) { language.identifiers.insert(identifier); }

		language.isPunctuation = TextEditor::Language::isCStylePunctuation;
		language.isWord = TextEditor::Language::isCStyleWordCharacter;
		language.getIdentifier = TextEditor::Language::getCStyleIdentifier;
		language.getNumber = TextEditor::Language::getCStyleNumber;

		initialized = true;
	}

	return language;
}


//
//	TextEditor::Language::getPythonStyleNumber
//

TextEditor::Iterator TextEditor::Language::getPythonStyleNumber(Iterator start, Iterator end) {
	auto i = start;
	if (i != end && (*i == '-' || *i == '+')) { i++; }
	if (i == end || !std::isdigit(*i)) { return start; }

	if (*i == '0') {
		if (++i == end) { return i;	}

		if (*i == 'b' || *i == 'B') {
			i++;
			if (i == end || !(*i == '0' || *i == '1' || *i == '_')) { return start; }
			while (i != end && (*i == '0' || *i == '1' || *i == '_')) { i++; }

		} else if (*i == 'o' || *i == 'O') {
			i++;
			if (i == end || !((*i >= '0' && *i <= '7' || *i == '_'))) { return start; }
			while (i != end && ((*i >= '0' && *i <= '7') || *i == '_')) { i++; }

		} else if (*i == 'x' || *i == 'X') {
			i++;
			if (i == end || !(std::isxdigit(*i)) || *i == '_') { return start; }
			while (i != end && (std::isxdigit(*i) || *i == '_')) { i++; }

		} else {
			return start;
		}

	} else {
		while (i != end && (std::isdigit(*i) || *i == '_')) { i++; }

		if (i != end) {
			if (*i == '.') {
				i++;
				while (i != end && (std::isdigit(*i) || *i == '_')) { i++; }

				if (i != end && (*i == 'e'|| *i == 'E' || *i == '+'|| *i == '-')) {
					i++;
					if (i == end || !std::isdigit(*i) || *i == '_') { return start; }
					while (i != end && std::isdigit(*i) || *i == '_') { i++; }
				}
			}
		}
	}

	return i;
}


//
//	TextEditor::Language::Python
//

const TextEditor::Language& TextEditor::Language::Python() {
	static bool initialized = false;
	static TextEditor::Language language;

	if (!initialized) {
		language.name = "Python";
		language.singleLineComment = "#";
		language.hasSingleQuotedStrings = true;
		language.hasDoubleQuotedStrings = true;
		language.otherStringStart = "\"\"\"";
		language.otherStringEnd = "\"\"\"";
		language.otherStringAltStart = "'''";
		language.otherStringAltEnd = "'''";
		language.stringEscape = '\\';

		static const char* const keywords[] = {
			"False", "await", "else", "import", "pass", "None", "break", "except", "in", "raise", "True",
			"class", "finally", "is", "return", "and", "continue", "for", "lambda", "try", "as", "def",
			"from", "nonlocal", "while", "assert", "del", "global", "not", "with", "async", "elif",
			"if", "or", "yield"
		};

		static const char* const identifiers[] = {
			"abs", "aiter", "all", "any", "anext", "ascii", "bin", "bool", "breakpoint", "bytearray",
			"bytes", "callable", "chr", "classmethod", "compile", "complex", "delattr", "dict", "dir",
			"divmod", "enumerate", "eval", "exec", "filter", "float", "format", "frozenset", "getattr",
			"globals", "hasattr", "hash", "help", "hex", "id", "input", "int", "isinstance", "issubclass",
			"iter", "len", "list", "locals", "map", "max", "memoryview", "min", "next", "object", "oct",
			"open", "ord", "pow", "print", "property", "range", "repr", "reversed", "round", "set", "setattr",
			"slice", "sorted", "staticmethod", "str", "sum", "super", "tuple", "type", "vars", "zip", "__import__"
		};

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }
		for (auto& identifier : identifiers) { language.identifiers.insert(identifier); }

		language.isPunctuation = TextEditor::Language::isCStylePunctuation;
		language.isWord = TextEditor::Language::isCStyleWordCharacter;
		language.getIdentifier = TextEditor::Language::getCStyleIdentifier;
		language.getNumber = TextEditor::Language::getPythonStyleNumber;

		initialized = true;
	}

	return language;
}


//
//	TextEditor::Language::isJsonStylePunctuation
//

bool TextEditor::Language::isJsonStylePunctuation(ImWchar character) {
	return character < 127 ? jsonStylePunctuation[character] : false;
}


//
//	TextEditor::Language::getJsonStyleNumber
//

TextEditor::Iterator TextEditor::Language::getJsonStyleNumber(Iterator start, Iterator end) {
	auto i = start;
	if (i != end && *i == '-') { i++; }
	if (i == end || !std::isdigit(*i)) { return start; }
	while (i != end && std::isdigit(*i)) { i++; }

	if (i != end) {
		if (*i == '.') {
			i++;
			while (i != end && std::isdigit(*i)) { i++; }
		}

		if (i != end && (*i == 'e'|| *i == 'E')) {
			i++;
			if (i != end && (*i == '-' || *i == '+')) { i++; }
			if (i == end || !std::isdigit(*i)) { return start; }
			while (i != end && std::isdigit(*i)) { i++; }
		}
	}

	return i;
}


//
//	TextEditor::Language::Json
//

const TextEditor::Language &TextEditor::Language::Json() {
	static bool initialized = false;
	static TextEditor::Language language;

	if (!initialized) {
		language.name = "JSON";
		language.hasDoubleQuotedStrings = true;
		language.stringEscape = '\\';

		static const char* const identifiers[] = {
			"false", "null", "true"
		};

		for (auto& identifier : identifiers) { language.identifiers.insert(identifier); }

		language.isPunctuation = TextEditor::Language::isJsonStylePunctuation;
		language.isWord = TextEditor::Language::isCStyleWordCharacter;
		language.getIdentifier = TextEditor::Language::getCStyleIdentifier;
		language.getNumber = TextEditor::Language::getJsonStyleNumber;

		initialized = true;
	}

	return language;
}


//
//	TextEditor::Language::isMarkdownStylePunctuation
//

bool TextEditor::Language::isMarkdownStylePunctuation(ImWchar character) {
	return character < 127 ? markdownStylePunctuation[character] : false;
}


//
//	TextEditor::Language::Markdown
//

const TextEditor::Language& TextEditor::Language::Markdown() {
	static bool initialized = false;
	static TextEditor::Language language;

	if (!initialized) {
		language.name = "Markdown";
		language.preprocess = '#';
		language.commentStart = "<!--";
		language.commentEnd = "-->";

		language.isPunctuation = TextEditor::Language::isMarkdownStylePunctuation;
		language.isWord = TextEditor::Language::isCStyleWordCharacter;

		initialized = true;
	}

	return language;
}
