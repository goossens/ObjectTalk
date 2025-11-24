//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "glm/glm.hpp"
#include "nanovg.h"

#include "OtColorParser.h"
#include "OtFrameBuffer.h"
#include "OtIndexBuffer.h"
#include "OtRenderPass.h"
#include "OtRenderPipeline.h"
#include "OtSampler.h"
#include "OtTexture.h"
#include "OtVertex.h"
#include "OtVertexBuffer.h"


//
//	OtCanvas
//

class OtCanvas {
public:
	// constructor/destructor
	OtCanvas();
	~OtCanvas();

	// image repeat options
	static constexpr int imageRepeatX = NVG_IMAGE_REPEATX;
	static constexpr int imageRepeatY = NVG_IMAGE_REPEATY;
	static constexpr int imageFlipY = NVG_IMAGE_FLIPY;
	static constexpr int imageNearest = NVG_IMAGE_NEAREST;

	// line caps
	static constexpr int lineButtCap = NVG_BUTT;
	static constexpr int lineRoundCap = NVG_ROUND;
	static constexpr int lineSquareCap = NVG_SQUARE;

	// winding order
	static constexpr int windingCcw = NVG_SOLID;
	static constexpr int windingCw = NVG_HOLE;

	// alignment options
	static constexpr int alignLeft = NVG_ALIGN_LEFT;
	static constexpr int alignCenter = NVG_ALIGN_CENTER;
	static constexpr int alignRight = NVG_ALIGN_RIGHT;
	static constexpr int alignTop = NVG_ALIGN_TOP;
	static constexpr int alignMiddle = NVG_ALIGN_MIDDLE;
	static constexpr int alignBottom = NVG_ALIGN_BOTTOM;
	static constexpr int alignBaseline = NVG_ALIGN_BASELINE;

	// manipulate rendering state
	inline void save() { nvgSave(context); }
	inline void restore() { nvgRestore(context); }
	inline void reset() { nvgReset(context); }

	// manage images
	int loadImage(const std::string& path, int flags);
	inline void deleteImage(int image) { nvgDeleteImage(context, image); }
	inline float getImageWidth(int image) { int w, h; nvgImageSize(context, image, &w, &h); return float(w); }
	inline float getImageHeight(int image) { int w, h; nvgImageSize(context, image, &w, &h); return float(h); }

	// manage paints
	int createLinearGradient(float sx, float sy, float ex, float ey, const std::string& startColor, const std::string& endColor);
	int createBoxGradient(float x, float y, float w, float h, float r, float f, const std::string& startColor, const std::string& endColor);
	int createRadialGradient(float cx, float cy, float inner, float outer, const std::string& startColor, const std::string& endColor);
	int createTexturePattern(float sx, float sy, float ex, float ey, float angle, int texture, float alpha);
	void deletePaint(int id);

	// manage fonts
	int loadFont(const std::string& path);

	// manipulate styles
	inline void antiAlias(bool on) { nvgShapeAntiAlias(context, on); }
	inline void strokeColor(const std::string& color) { auto c = OtColorParser::toVec4(color); nvgStrokeColor(context, nvgRGBAf(c.r, c.g, c.b, c.a)); }
	inline void strokeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { nvgStrokeColor(context, nvgRGBA(r, g, b, a)); }
	inline void strokeColor(float r, float g, float b, float a) { nvgStrokeColor(context, nvgRGBAf(r, g, b, a)); }
	inline void strokePaint(int id) { nvgStrokePaint(context, paints[id]); }
	inline void fillColor(const std::string& color) { auto c = OtColorParser::toVec4(color); nvgFillColor(context, nvgRGBAf(c.r, c.g, c.b, c.a)); }
	inline void fillColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { nvgFillColor(context, nvgRGBA(r, g, b, a)); }
	inline void fillColor(float r, float g, float b, float a) { nvgFillColor(context, nvgRGBAf(r, g, b, a)); }
	inline void fillPaint(int id) { nvgFillPaint(context, paints[id]); }
	inline void miterLimit(float limit) { nvgMiterLimit(context, limit); }
	inline void strokeWidth(float w) { nvgStrokeWidth(context, w); }
	inline void lineCap(int cap) { nvgLineCap(context, cap); }
	inline void lineJoin(int join) { nvgLineJoin(context, join); }
	inline void globalAlpha(float alpha) { nvgGlobalAlpha(context, alpha); }

	// manipulate transform
	inline void translate(float x, float y) { nvgTranslate(context, x, y); }
	inline void rotate(float angle) { nvgRotate(context, angle); }
	inline void scale(float x, float y) { nvgScale(context, x, y); }
	inline void skewX(float angle) { nvgSkewX(context, angle); }
	inline void skewY(float angle) { nvgSkewY(context, angle); }
	inline void resetTransform() { nvgResetTransform(context); }

	// manipulate path
	inline void beginPath() { nvgBeginPath(context); }
	inline void moveTo(float x, float y) { nvgMoveTo(context, x, y); }
	inline void lineTo(float x, float y) { nvgLineTo(context, x, y); }
	inline void bezierCurveTo(float c1x, float c1y, float c2x, float c2y, float x, float y) { nvgBezierTo(context, c1x, c1y, c2x, c2y, x, y); }
	inline void quadraticCurveTo(float cx, float cy, float x, float y) { nvgQuadTo(context, cx, cy, x, y); }
	inline void arcTo(float x1, float y1, float x2, float y2, float radius) { nvgArcTo(context, x1, y1, x2, y2, radius); }
	inline void closePath() { nvgClosePath(context); }
	inline void pathWinding(int winding) { nvgPathWinding(context, NVGwinding(winding)); }

	inline void arc(float cx, float cy, float r, float a0, float a1, int dir) { nvgArc(context, cx, cy, r, a0, a1, dir); }
	inline void rect(float x, float y, float w, float h) { nvgRect(context, x, y, w, h); }
	inline void roundedRect(float x, float y, float w, float h, float r) { nvgRoundedRect(context, x, y, w, h, r); }
	inline void ellipse(float cx, float cy, float rx, float ry) { nvgEllipse(context, cx, cy, rx, ry); }
	inline void circle(float cx, float cy, float r) { nvgCircle(context, cx, cy, r); }

	// render path
	inline void stroke() { nvgStroke(context); }
	inline void fill() { nvgFill(context); }

	void drawImage(int image, float x, float y);
	void drawImage(int image, float x, float y, float w, float h);
	void drawImage(int image, float sx, float sy, float sw, float sh, float x, float y, float w, float h);

	// render text
	inline void fontFace(int id) { nvgFontFaceId(context, id); }
	inline void fontSize(float size) { nvgFontSize(context, size); }
	inline void fontBlur(float blur) { nvgFontBlur(context, blur); }
	inline void fontLetterSpacing(float spacing) { nvgTextLetterSpacing(context, spacing); }
	inline void fontLineHeight(float h) { nvgTextLineHeight(context, h); }
	inline void fontAlign(int align) { nvgTextAlign(context, align); }
	inline float text(float x, float y, const std::string& s) { return nvgText(context, x, y, s.c_str(), nullptr); }
	inline void textBox(float x, float y, float w, const std::string& s) { nvgTextBox(context, x, y, w, s.c_str(), nullptr); }

	glm::vec2 textSize(const std::string& string);
	glm::vec2 textBoxSize(const std::string& string, float w);

	// access state
	inline void enable() { enabled = true; dirty = true; }
	inline void disable() { enabled = false; }
	inline bool isEnabled() { return enabled; }

	inline void requestRerender() { dirty = true; }
	inline void markClean() { dirty = false; }
	inline bool needsRerender() { return dirty; }

	// render canvas to framebuffer
	void render(OtFrameBuffer& framebuffer, float scale, std::function<void()> renderer);

	// access size
	inline float getWidth() { return width; }
	inline float getHeight() { return height; }

private:
	// properties
	NVGcontext* context;
	std::unordered_map<int, NVGpaint> paints;

	struct Texture {
		OtTexture texture;
		OtSampler sampler;
		bool flip;
	};

	std::unordered_map<int, Texture> textures;

	bool enabled = true;
	bool dirty = true;
	int nextTextureID = 1;

	float width;
	float height;

	// helper functions
	int addPaint(const NVGpaint& paint);
	int paintID = 1;

	// drawing call data
	enum class CallType {
		none,
		concaveFill,
		convexFill,
		stroke,
		triangles
	};

	struct Call {
		CallType type;
		int textureID;
		size_t shapeOffset;
		size_t shapeCount;
		size_t strokeOffset;
		size_t strokeCount;
		size_t fillOffset;
		size_t fillCount;
		size_t uniformOffset;
	};

	// rendering functions
	void concaveFill(OtRenderPass& pass, Call& call);
	void convexFill(OtRenderPass& pass, Call& call);
	void stroke(OtRenderPass& pass, Call& call);
	void triangles(OtRenderPass& pass, Call& call);

	// driver functions
	int renderCreate();
	int renderCreateTexture(int type, int w, int h, int imageFlags, const unsigned char* data);
	int renderDeleteTexture(int texture);
	int renderUpdateTexture(int texture, int x, int y, int w, int h, const unsigned char* data);
	int renderGetTextureSize(int texture, int* w, int* h);
	void renderFill(NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor, float fringe, const float* bounds, const NVGpath* paths, int npaths);
	void renderStroke(NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor, float fringe, float strokeWidth, const NVGpath* paths, int npaths);
	void renderTriangles(NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor, const NVGvertex* verts, int nverts, float fringe);
	void renderDelete();

	static constexpr int transparentTexture = 0;
	static constexpr int rTexture = 1;
	static constexpr int rgbaTexture = 2;

	static constexpr int simpleShader = 0;
	static constexpr int fillGradientShader = 1;
	static constexpr int fillTextureShader = 2;
	static constexpr int textureShader = 3;

	struct FragmentUniforms {
		glm::vec4 scissorMatCol1;
		glm::vec4 scissorMatCol2;
		glm::vec4 scissorMatCol3;
		glm::vec4 paintMatCol1;
		glm::vec4 paintMatCol2;
		glm::vec4 paintMatCol3;
		glm::vec4 innerColor;
		glm::vec4 outerColor;
		glm::vec2 scissorExt;
		glm::vec2 scissorScale;
		glm::vec2 extent;
		float radius;
		float feather;
		float strokeMult;
		float strokeThr;
		uint32_t texType;
		uint32_t shaderType;
	};

	std::vector<Call> calls;
	std::vector<OtVertexPosUv2D> vertices;
	std::vector<uint32_t> indices;
	std::vector<FragmentUniforms> fragmentUniforms;

	OtVertexBuffer vertexBuffer;
	OtIndexBuffer indexBuffer;
	OtRenderPipeline convexPipeline;

	OtRenderPipeline fillShapesPipeline;
	OtRenderPipeline fillFragmentsPipeline;
	OtRenderPipeline fillPipeline;
	OtRenderPipeline strokeBasePipeline;
	OtRenderPipeline strokeFragmentPipeline;
	OtRenderPipeline clearStencilPipeline;

	size_t paintToUniforms(NVGpaint* paint, NVGscissor* scissor, float width, float fringe, float strokeThr, int shaderType);

	void setVertexUniforms(OtRenderPass& pass);
	void setFragmentUniforms(OtRenderPass& pass, size_t uniformOffset, int textureID);
};
