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

	// 5 Use shader program
	void useProgram(Shader* shader);

	// 6 Draw
	void drawElement(const uint32_t& drawMode, const uint32_t& first, const uint32_t& count);

private:

	// 1 Vertex shader 
	void vertexShaderStage(std::vector<VsOutput>& vsOutputs, const VertexArrayObject* vao, const BufferObject* ebo, const uint32_t first, const uint32_t count);

	// 2 Perspective division
	void perspectiveDivision(VsOutput& vsOutput);

	// 3 Viewport transform
	void screenMapping(VsOutput& vsOutput);

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

};
