#pragma once
#include "../global/base.h"
#include "../math/math.h"

// VAO
struct BindingDescription {

	uint32_t mVboId{ 0 };
	size_t   mItemSize{ 0 };
	size_t   mStride{ 0 };
	size_t   mOffset{ 0 };

};

// Vertex shader output
struct VsOutput {

	math::vec4f mPosition{ 0.0f, 0.0f, 0.0f, 1.0f };
	math::vec4f mColor;
	math::vec2f mUV;

};

// Fragment shader output
struct FsOutput {

	math::vec2i mPixelPos;
	float mDepth;
	RGBA mColor;
};
