#include "shader.h"

Shader::Shader(){
}

Shader::~Shader(){
}

math::vec4f Shader::getVector(const std::map<uint32_t, BindingDescription>& bindingMap, const std::map<uint32_t, BufferObject*>& bufferMap, const uint32_t& attributeLocation, const uint32_t& index) {

	// 1 Get binding description
	auto bindingIter = bindingMap.find(attributeLocation);
	if (bindingIter == bindingMap.end()) {

		assert(false);

	}

	auto bindingDescription = bindingIter->second;

	// 2 Get vbo
	auto vboID = bindingDescription.mVboId;
	auto vboIter = bufferMap.find(vboID);
	if (vboIter == bufferMap.end()) {

		assert(false);
	}

	BufferObject* vbo = vboIter->second;

	// 3 Get data in vbo
	uint32_t dataOffset = bindingDescription.mStride * index + bindingDescription.mOffset;
	uint32_t dataSize = bindingDescription.mItemSize * sizeof(float);

	const byte* buffer = vbo->getBuffer() + dataOffset;

	math::vec4f result;

	memcpy(&result, buffer, dataSize);

	return result;

}

RGBA Shader::vectorToRGBA(const math::vec4f& v) {

	RGBA color;
	color.mR = v.x * 255.0;
	color.mG = v.y * 255.0;
	color.mB = v.z * 255.0;
	color.mA = v.w * 255.0;

	return color;

}