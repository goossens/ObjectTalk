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

#include "lexers/c.h"
#include "lexers/cs.h"
#include "lexers/json.h"
#include "lexers/lua.h"
#include "lexers/markdown.h"
#include "lexers/python.h"


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

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }
		for (auto& declaration : declarations) { language.declarations.insert(declaration); }

		language.isPunctuation = isCStylePunctuation;
		language.getIdentifier = getCStyleIdentifier;
		language.getNumber = getCStyleNumber;
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

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }
		for (auto& declaration : declarations) { language.declarations.insert(declaration); }

		language.isPunctuation = isCStylePunctuation;
		language.getIdentifier = getCStyleIdentifier;
		language.getNumber = getCStyleNumber;
		initialized = true;
	}

	return language;
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

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }

		language.isPunctuation = isCStylePunctuation;
		language.getIdentifier = getCStyleIdentifier;
		language.getNumber = getCsStyleNumber;
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

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }

		language.isPunctuation = isCStylePunctuation;
		language.getIdentifier = getCStyleIdentifier;
		language.getNumber = getCStyleNumber;
		initialized = true;
	}

	return language;
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

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }

		language.isPunctuation = isCStylePunctuation;
		language.getIdentifier = getCStyleIdentifier;
		language.getNumber = getLuaStyleNumber;
		initialized = true;
	}

	return language;
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

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }

		language.isPunctuation = isCStylePunctuation;
		language.getIdentifier = getCStyleIdentifier;
		language.getNumber = getPythonStyleNumber;
		initialized = true;
	}

	return language;
}


//
//	TextEditor::Language::Glsl
//

const TextEditor::Language& TextEditor::Language::Glsl() {
	static bool initialized = false;
	static TextEditor::Language language;

	if (!initialized) {
		language.name = "GLSL";
		language.preprocess = '#';
		language.singleLineComment = "//";
		language.commentStart = "/*";
		language.commentEnd = "*/";
		language.hasSingleQuotedStrings = true;
		language.hasDoubleQuotedStrings = true;
		language.stringEscape = '\\';

		// source: https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.html
		static const char* const keywords[] = {
			"atomic_uint", "attribute", "bool", "break", "buffer", "bvec2", "bvec3", "bvec4", "case", "centroid",
			"coherent", "const", "continue", "default", "discard", "dmat2", "dmat2x2", "dmat2x3", "dmat2x4", "dmat3",
			"dmat3x2", "dmat3x3", "dmat3x4", "dmat4", "dmat4x2", "dmat4x3", "dmat4x4", "do", "double", "dvec2", "dvec3",
			"dvec4", "else", "false", "flat", "float", "for", "highp", "if", "iimage1D", "iimage1DArray", "iimage2D",
			"iimage2DArray", "iimage2DMS", "iimage2DMSArray", "iimage2DRect", "iimage3D", "iimageBuffer", "iimageCube",
			"iimageCubeArray", "image1D", "image1DArray", "image2D", "image2DArray", "image2DMS", "image2DMSArray",
			"image2DRect", "image3D", "imageBuffer", "imageCube", "imageCubeArray", "in", "inout", "int", "invariant",
			"isampler1D", "isampler1DArray", "isampler2D", "isampler2DArray", "isampler2DMS", "isampler2DMSArray",
			"isampler2DRect", "isampler3D", "isamplerBuffer", "isamplerCube", "isamplerCubeArray", "ivec2", "ivec3",
			"ivec4", "layout", "lowp", "mat2", "mat2x2", "mat2x3", "mat2x4", "mat3", "mat3x2", "mat3x3", "mat3x4",
			"mat4", "mat4x2", "mat4x3", "mat4x4", "mediump", "noperspective", "out", "patch", "precise", "precision",
			"readonly", "restrict", "return", "sample", "sampler1D", "sampler1DArray", "sampler1DArrayShadow",
			"sampler1DShadow", "sampler2D", "sampler2DArray", "sampler2DArrayShadow", "sampler2DMS", "sampler2DMSArray",
			"sampler2DRect", "sampler2DRectShadow", "sampler2DShadow", "sampler3D", "samplerBuffer", "samplerCube",
			"samplerCubeArray", "samplerCubeArrayShadow", "samplerCubeShadow", "shared", "smooth", "struct", "subroutine",
			"switch", "true", "uimage1D", "uimage1DArray", "uimage2D", "uimage2DArray", "uimage2DMS", "uimage2DMSArray",
			"uimage2DRect", "uimage3D", "uimageBuffer", "uimageCube", "uimageCubeArray", "uint", "uniform", "usampler1D",
			"usampler1DArray", "usampler2D", "usampler2DArray", "usampler2DMS", "usampler2DMSArray", "usampler2DRect",
			"usampler3D", "usamplerBuffer", "usamplerCube", "usamplerCubeArray", "uvec2", "uvec3", "uvec4", "varying",
			"vec2", "vec3", "vec4", "void", "volatile", "while", "writeonly"
		};

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }

		language.isPunctuation = isCStylePunctuation;
		language.getIdentifier = getCStyleIdentifier;
		language.getNumber = getCStyleNumber;
		initialized = true;
	}

	return language;
}


//
//	TextEditor::Language::Hlsl
//

const TextEditor::Language& TextEditor::Language::Hlsl() {
	static bool initialized = false;
	static TextEditor::Language language;

	if (!initialized) {
		language.name = "HLSL";
		language.preprocess = '#';
		language.singleLineComment = "//";
		language.commentStart = "/*";
		language.commentEnd = "*/";
		language.hasSingleQuotedStrings = true;
		language.hasDoubleQuotedStrings = true;
		language.stringEscape = '\\';

		static const char* const keywords[] = {
			"AppendStructuredBuffer", "asm", "asm_fragment", "BlendState", "bool", "break", "Buffer",
			"ByteAddressBuffer", "case", "cbuffer", "centroid", "class", "column_major", "compile",
			"compile_fragment", "CompileShader", "const", "continue", "ComputeShader", "ConsumeStructuredBuffer",
			"default", "DepthStencilState", "DepthStencilView", "discard", "do", "double", "DomainShader", "dword",
			"else", "export", "extern", "false", "float", "for", "fxgroup", "GeometryShader", "groupshared", "half",
			"Hullshader", "if", "in", "inline", "inout", "InputPatch", "int", "interface", "line", "lineadj",
			"linear", "LineStream", "matrix", "min16float", "min10float", "min16int", "min12int", "min16uint",
			"namespace", "nointerpolation", "noperspective", "NULL", "out", "OutputPatch", "packoffset",
			"pass", "pixelfragment", "PixelShader", "point", "PointStream", "precise", "RasterizerState",
			"RenderTargetView", "return", "register", "row_major", "RWBuffer", "RWByteAddressBuffer",
			"RWStructuredBuffer", "RWTexture1D", "RWTexture1DArray", "RWTexture2D", "RWTexture2DArray",
			"RWTexture3D", "sample", "sampler", "SamplerState", "SamplerComparisonState", "shared",
			"snorm", "stateblock", "stateblock_state", "static", "string", "struct", "switch", "StructuredBuffer",
			"tbuffer", "technique", "technique10", "technique11", "texture", "Texture1D", "Texture1DArray",
			"Texture2D", "Texture2DArray", "Texture2DMS", "Texture2DMSArray", "Texture3D", "TextureCube",
			"TextureCubeArray", "true", "typedef", "triangle", "triangleadj", "TriangleStream", "uint",
			"uniform", "unorm", "unsigned", "vector", "vertexfragment", "VertexShader", "void", "volatile", "while",
			"bool1", "bool2", "bool3", "bool4", "double1", "double2", "double3", "double4", "float1", "float2",
			"float3", "float4", "int1", "int2", "int3", "int4", "in", "out", "inout", "uint1", "uint2", "uint3",
			"uint4", "dword1", "dword2", "dword3", "dword4", "half1", "half2", "half3", "half4", "float1x1",
			"float2x1", "float3x1", "float4x1", "float1x2", "float2x2", "float3x2", "float4x2",
			"float1x3", "float2x3", "float3x3", "float4x3", "float1x4", "float2x4", "float3x4", "float4x4",
			"half1x1", "half2x1", "half3x1", "half4x1", "half1x2", "half2x2", "half3x2", "half4x2",
			"half1x3", "half2x3", "half3x3", "half4x3", "half1x4", "half2x4", "half3x4", "half4x4",
		};

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }

		language.isPunctuation = isCStylePunctuation;
		language.getIdentifier = getCStyleIdentifier;
		language.getNumber = getCStyleNumber;
		initialized = true;
	}

	return language;
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

		static const char* const keywords[] = {
			"false", "null", "true"
		};

		for (auto& keyword : keywords) { language.keywords.insert(keyword); }

		language.customTokenizer = tokenizeJson;
		initialized = true;
	}

	return language;
}


//
//	TextEditor::Language::Markdown
//

const TextEditor::Language& TextEditor::Language::Markdown() {
	static bool initialized = false;
	static TextEditor::Language language;

	if (!initialized) {
		language.name = "Markdown";
		language.singleLineComment = ">";
		language.commentStart = "```";
		language.commentEnd = "```";

		language.customTokenizer = tokenizeMarkdown;
		initialized = true;
	}

	return language;
}
