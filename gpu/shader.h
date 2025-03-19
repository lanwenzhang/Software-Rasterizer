#pragma once
#include "../global/base.h"
#include "dataStructures.h"
#include "bufferObject.h"
#include "texture.h"

class Shader {
public:
	 
	Shader();
	~Shader();

	virtual VsOutput vertexShader(const std::map<uint32_t, BindingDescription>& bindingMap, const std::map<uint32_t, BufferObject*>& bufferMap, const uint32_t& index) = 0;

	virtual void fragmentShader(const VsOutput& input, FsOutput& output, const std::map<uint32_t, Texture*>& textures) = 0;

public:

	math::vec4f getVector(const std::map<uint32_t, BindingDescription>& bindingMap, const std::map<uint32_t, BufferObject*>& bufferMap, const uint32_t& attributeLocation, const uint32_t& index);

	RGBA vectorToRGBA(const math::vec4f& v);
};