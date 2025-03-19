#include "gpu.h"
#include "raster.h"
#include "clipper.h"

GPU* GPU::mInstance = nullptr;

GPU* GPU::getInstance() {

	if (!mInstance) {

		mInstance = new GPU();
	}

	return mInstance;
}

GPU::GPU(){}

GPU::~GPU(){

	if (mFrameBuffer) {

		delete mFrameBuffer;
	}

	for (auto iter : mBufferMap) {

		delete iter.second;
	}

	mBufferMap.clear();

	for (auto iter : mVaoMap) {

		delete iter.second;
	}

	mVaoMap.clear();

}

void GPU::initSurface(const uint32_t& width, const uint32_t& height, void* buffer){

	mFrameBuffer = new FrameBuffer(width, height, buffer);
	mScreenMatrix = math::screenMatrix<float>(width - 1, height - 1);
}

void GPU::clear(){

	size_t pixelSize = mFrameBuffer->mWidth * mFrameBuffer->mHeight;
	std::fill_n(mFrameBuffer->mColorBuffer, pixelSize, RGBA(21, 49, 76, 1));
	std::fill_n(mFrameBuffer->mDepthBuffer, pixelSize, 1.0f);
}

uint32_t GPU::genBuffer() {

	mBufferCounter++;
	mBufferMap.insert(std::make_pair(mBufferCounter, new BufferObject()));

	return mBufferCounter;
}

void GPU::deleteBuffer(const uint32_t& bufferID) {

	auto iter = mBufferMap.find(bufferID);
	if (iter != mBufferMap.end()) {

		delete iter->second;
	}
	else {

		return;
	}

	mBufferMap.erase(iter);
}

void GPU::bindBuffer(const uint32_t& bufferType, const uint32_t& bufferID) {
	
	if (bufferType == ARRAY_BUFFER) {
		mCurrentVBO = bufferID;
	}
	else if (bufferType == ELEMENT_ARRAY_BUFFER) {
		mCurrentEBO = bufferID;
	}
}

void GPU::bufferData(const uint32_t& bufferType, size_t dataSize, void* data) {
	uint32_t bufferID = 0;
	if (bufferType == ARRAY_BUFFER) {
		bufferID = mCurrentVBO;
	}
	else if (bufferType == ELEMENT_ARRAY_BUFFER) {
		bufferID = mCurrentEBO;
	}
	else {
		assert(false);
	}

	auto iter = mBufferMap.find(bufferID);
	if (iter == mBufferMap.end()) {
		assert(false);
	}

	BufferObject* bufferObject = iter->second;
	bufferObject->setBufferData(dataSize, data);
}

uint32_t GPU::genVertexArray() {


	mVaoCounter++;
	mVaoMap.insert(std::make_pair(mVaoCounter, new VertexArrayObject()));

	return mVaoCounter;
}

void GPU::deleteVertexArray(const uint32_t& vaoID) {

	auto iter = mVaoMap.find(vaoID);
	if (iter != mVaoMap.end()) {
		delete iter->second;
	}
	else {
		return;
	}

	mVaoMap.erase(iter);
}

void GPU::bindVertexArray(const uint32_t& vaoID) {
	mCurrentVAO = vaoID;
}

void GPU::vertexAttributePointer(const uint32_t& binding, const uint32_t& itemSize, const uint32_t& stride, const uint32_t& offset){
	
	auto iter = mVaoMap.find(mCurrentVAO);
	if (iter == mVaoMap.end()) {
		assert(false);
	}

	auto vao = iter->second;
	vao->set(binding, mCurrentVBO, itemSize, stride, offset);
}

uint32_t GPU::genTexture(){

	mTextureCounter++;
	mTextureMap.insert(std::make_pair(mTextureCounter, new Texture()));

	return mTextureCounter;

}

void GPU::deleteTexture(const uint32_t& texID){

	auto iter = mTextureMap.find(texID);
	if (iter != mTextureMap.end()) {

		delete iter->second;
	}
	else {

		return;
	}

	mTextureMap.erase(iter);
}


void GPU::bindTexture(const uint32_t& texID){

	mCurrentTexture = texID;
}


void GPU::texImage2D(const uint32_t& width, const uint32_t& height, void* data){

	if (!mCurrentTexture) {

		return;
	}

	auto iter = mTextureMap.find(mCurrentTexture);
	if (iter == mTextureMap.end()) {

		return;
	}
	auto texture = iter->second;
	texture->setBufferData(width, height, data);
}


void GPU::texParameter(const uint32_t& param, const uint32_t& value){

	if (!mCurrentTexture) {
		return;
	}

	auto iter = mTextureMap.find(mCurrentTexture);
	if (iter == mTextureMap.end()) {
		return;
	}
	auto texture = iter->second;
	texture->setParameter(param, value);
}


void GPU::useProgram(Shader* shader){

	mShader = shader;
}

void GPU::drawElement(const uint32_t& drawMode, const uint32_t& first, const uint32_t& count){

	if (mCurrentVAO == 0 || mShader == nullptr || count == 0) {

		return;
	}

	// 1 Get VAO
	auto vaoIter = mVaoMap.find(mCurrentVAO);
	if (vaoIter == mVaoMap.end()) {

		std::cerr << "Error: current vao is invalid!" << std::endl;
		return;

	}

	const VertexArrayObject* vao = vaoIter->second;
	auto bindingMap = vao->getBindingMap();

	// 2 Get EBO
	auto eboIter = mBufferMap.find(mCurrentEBO);
	if (eboIter == mBufferMap.end()) {
		std::cerr << "Error: current ebo is invalid!" << std::endl;
		return;
	}

	const BufferObject* ebo = eboIter->second;

	// 3 Vertex shader - get data
	std::vector<VsOutput> vsOutputs{};
	vertexShaderStage(vsOutputs, vao, ebo, first, count);

	if (vsOutputs.empty()) return;

	// 4 Vertex shader - clip
	std::vector<VsOutput> clipOutputs{};
	Clipper::doClipSpace(drawMode, vsOutputs, clipOutputs);
	if (clipOutputs.empty()) return;

	vsOutputs.clear();

	// 5 Vertex shader - perspective division
	for (auto& output : clipOutputs) {

		perspectiveDivision(output);
	}

	// 6 Vertex shader - face culling
	std::vector<VsOutput> cullOutputs = clipOutputs;
	if (drawMode == DRAW_TRIANGLES && mEnableCullFace) {
		cullOutputs.clear();
		for (uint32_t i = 0; i < clipOutputs.size() - 2; i += 3) {
			if (Clipper::cullFace(mFrontFace, mCullFace, clipOutputs[i], clipOutputs[i + 1], clipOutputs[i + 2])) {
				auto start = clipOutputs.begin() + i;
				auto end = clipOutputs.begin() + i + 3;
				cullOutputs.insert(cullOutputs.end(), start, end);
			}
		}
	}


	// 7 Vertex shader - viewport transform
	for (auto& output : cullOutputs) {

		screenMapping(output);

	}

	// 8 Rasterization
	std::vector<VsOutput> rasterOutputs;
	Raster::rasterize(rasterOutputs, drawMode, cullOutputs);

	if (rasterOutputs.empty()) return;

	// 9 Perspective recover
	for (auto& output : rasterOutputs) {

		perspectiveRecover(output);
	}


	// 10 Fragment shader
	FsOutput fsOutput;
	uint32_t pixelPos = 0;
	for (uint32_t i = 0; i < rasterOutputs.size(); ++i) {
		mShader->fragmentShader(rasterOutputs[i], fsOutput, mTextureMap);
		pixelPos = fsOutput.mPixelPos.y * mFrameBuffer->mWidth + fsOutput.mPixelPos.x;

		// 10.1 Depth test
		if (mEnableDepthTest && !depthTest(fsOutput)) {

			continue;
		}

		// 10.2 Blending
		RGBA color = fsOutput.mColor;
		if (mEnableBlending) {

			color = blend(fsOutput);
		}

		mFrameBuffer->mColorBuffer[pixelPos] = color;
	}

	

}

void GPU::enable(const uint32_t& value) {

	switch (value) {

	case CULL_FACE:
		mEnableCullFace = true;
		break;
	case DEPTH_TEST:
		mEnableDepthTest = true;
		break;
	case BLENDING:
		mEnableBlending = true;
		break;
	default:
		break;
	}
}

void GPU::disable(const uint32_t& value) {

	switch (value) {

	case CULL_FACE:
		mEnableCullFace = false;
		break;
	case DEPTH_TEST:
		mEnableDepthTest = false;
		break;
	case BLENDING:
		mEnableBlending = false;
	default:
		break;
	}
}

void GPU::frontFace(const uint32_t& value){

	mFrontFace = value;

}

void GPU::cullFace(const uint32_t& value){

	mCullFace = value;
}

void GPU::depthFunc(const uint32_t& depthFunc) {

	mDepthFunc = depthFunc;
}

void GPU::vertexShaderStage(std::vector<VsOutput>& vsOutputs, const VertexArrayObject* vao, const BufferObject* ebo, const uint32_t first, const uint32_t count){


	auto bindingMap = vao->getBindingMap();
	byte* indicesData = ebo->getBuffer();

	uint32_t index = 0;
	for (uint32_t i = first; i < first + count; ++i) {
		
		size_t indicesOffset = i * sizeof(uint32_t);
		memcpy(&index, indicesData + indicesOffset, sizeof(uint32_t));

		VsOutput output = mShader->vertexShader(bindingMap, mBufferMap, index);
		vsOutputs.push_back(output);
	}

}

void GPU::perspectiveDivision(VsOutput& vsOutput){

	vsOutput.mOneOverW = 1.0f / vsOutput.mPosition.w;

	vsOutput.mPosition *= vsOutput.mOneOverW;
	vsOutput.mPosition.w = 1.0f;

	vsOutput.mColor *= vsOutput.mOneOverW;
	vsOutput.mNormal *= vsOutput.mOneOverW;
	vsOutput.mUV *= vsOutput.mOneOverW;

	trim(vsOutput);
}

void GPU::perspectiveRecover(VsOutput& vsOutput) {

	vsOutput.mColor /= vsOutput.mOneOverW;
	vsOutput.mNormal /= vsOutput.mOneOverW;
	vsOutput.mUV /= vsOutput.mOneOverW;
}

void GPU::screenMapping(VsOutput& vsOutput){

	vsOutput.mPosition = mScreenMatrix * vsOutput.mPosition;

}

void GPU::trim(VsOutput& vsOutput) {

	if (vsOutput.mPosition.x < -1.0f) {
		vsOutput.mPosition.x = -1.0f;
	}

	if (vsOutput.mPosition.x > 1.0f) {
		vsOutput.mPosition.x = 1.0f;
	}

	if (vsOutput.mPosition.y < -1.0f) {
		vsOutput.mPosition.y = -1.0f;
	}

	if (vsOutput.mPosition.y > 1.0f) {
		vsOutput.mPosition.y = 1.0f;
	}

	if (vsOutput.mPosition.z < -1.0f) {
		vsOutput.mPosition.z = -1.0f;
	}

	if (vsOutput.mPosition.z > 1.0f) {
		vsOutput.mPosition.z = 1.0f;
	}
}

bool GPU::depthTest(const FsOutput& output) {

	// 1 Get current pixel depth value
	uint32_t pixelPos = output.mPixelPos.y * mFrameBuffer->mWidth + output.mPixelPos.x;

	// 2 Compare it with previous depth value
	float oldDepth = mFrameBuffer->mDepthBuffer[pixelPos];

	switch (mDepthFunc) {

	case DEPTH_LESS:
		if (output.mDepth < oldDepth) {

			mFrameBuffer->mDepthBuffer[pixelPos] = output.mDepth;
			return true;
		}
		else {

			return false;
		}
		break;

	case DEPTH_GREATER:
		if (output.mDepth > oldDepth) {

			mFrameBuffer->mDepthBuffer[pixelPos] = output.mDepth;
			return true;

		}
		else {

			return false;

		}
		break;
	default:
		return false;
		break;
	}
}

RGBA GPU::blend(const FsOutput& output){

	RGBA result;

	uint32_t pixelPos = output.mPixelPos.y * mFrameBuffer->mWidth + output.mPixelPos.x;
	RGBA dst = mFrameBuffer->mColorBuffer[pixelPos];
	RGBA src = output.mColor;

	float weight = static_cast<float>(src.mA) / 255.0f;

	result.mR = static_cast<float>(src.mR) * weight + static_cast<float>(dst.mR) * (1.0f - weight);
	result.mG = static_cast<float>(src.mG) * weight + static_cast<float>(dst.mG) * (1.0f - weight);
	result.mB = static_cast<float>(src.mB) * weight + static_cast<float>(dst.mB) * (1.0f - weight);
	result.mA = static_cast<float>(src.mA) * weight + static_cast<float>(dst.mA) * (1.0f - weight);

	return result;
}