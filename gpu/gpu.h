#pragma once

#include "../global/base.h"

#include "../application/application.h"
#include "../application/image.h"
#include "../math/math.h"

#include "frameBuffer.h"
#include "dataStructures.h"
#include "bufferObject.h"
#include "vao.h"
#include "shader.h"
#include "texture.h"

#define sgl GPU::getInstance()

class GPU {

public:

	static GPU* getInstance();
	GPU();
	~GPU();

	// 1 Initial the drawing surface
	void initSurface(const uint32_t& width, const uint32_t& height, void* buffer = nullptr);

	// 2 Clear the drawing surface 
	void clear();

	// 3 Generate/bind/delete VBO/EBO
	uint32_t genBuffer();
	void deleteBuffer(const uint32_t& bufferID);
	void bindBuffer(const uint32_t& bufferType, const uint32_t& bufferID);
	void bufferData(const uint32_t& bufferType, size_t dataSize, void* data);

	// 4 Generate/bind/delete VAO/EBO
	uint32_t genVertexArray();
	void deleteVertexArray(const uint32_t& vaoID);
	void bindVertexArray(const uint32_t& vaoID);
	void vertexAttributePointer(const uint32_t& binding, const uint32_t& itemSize, const uint32_t& stride, const uint32_t& offset);

	// 5 Texture
	uint32_t genTexture();
	void deleteTexture(const uint32_t& texID);
	void bindTexture(const uint32_t& texID);
	void texImage2D(const uint32_t& width, const uint32_t& height, void* data);
	void texParameter(const uint32_t& param, const uint32_t& value);

	// 6 Use shader program
	void useProgram(Shader* shader);

	// 7 Draw
	void drawElement(const uint32_t& drawMode, const uint32_t& first, const uint32_t& count);

	// 8 Function
	void enable(const uint32_t& value);
	void disable(const uint32_t& value);

	// 8.1 Face culling
	void frontFace(const uint32_t& value);
	void cullFace(const uint32_t& value);

	// 8.2 Depth test
	void depthFunc(const uint32_t& depthFunc);

private:

	// 1 Vertex shader 
	void vertexShaderStage(std::vector<VsOutput>& vsOutputs, const VertexArrayObject* vao, const BufferObject* ebo, const uint32_t first, const uint32_t count);

	// 2 Perspective division
	void perspectiveDivision(VsOutput& vsOutput);
	void perspectiveRecover(VsOutput& vsOutput);
	void trim(VsOutput& vsOutput);

	// 3 Viewport transform
	void screenMapping(VsOutput& vsOutput);

	// 4 Depth test 
	bool depthTest(const FsOutput& output);

	// 5 Blending
	RGBA blend(const FsOutput& output);

private:

	// 1 Window
	static GPU* mInstance;
	FrameBuffer* mFrameBuffer{ nullptr };

	// 2 VBO, EBO
	uint32_t mCurrentVBO{ 0 };
	uint32_t mCurrentEBO{ 0 };
	uint32_t mBufferCounter{ 0 };
	std::map<uint32_t, BufferObject*> mBufferMap;

	// 3 VAO
	uint32_t mCurrentVAO{ 0 };
	uint32_t mVaoCounter{ 0 };
	std::map<uint32_t, VertexArrayObject*> mVaoMap;

	// 4 Shader
	Shader* mShader{ nullptr };
	math::mat4f mScreenMatrix;

	// 5 Face culling
	bool mEnableCullFace{ false };
	uint32_t mFrontFace{ FRONT_FACE_CCW };
	uint32_t mCullFace{ BACK_FACE };

	// 6 Depth test
	bool mEnableDepthTest{ true };
	uint32_t mDepthFunc{ DEPTH_LESS };

	// 7 Blending
	bool mEnableBlending{ false };

	// 8 Texture
	uint32_t mCurrentTexture{ 0 };
	uint32_t mTextureCounter{ 0 };
	std::map<uint32_t, Texture*> mTextureMap;
};

