#pragma once

#include "../global/base.h"

class BufferObject {
public:

	BufferObject();
	~BufferObject();
	BufferObject(const BufferObject& bufferObject) = delete;


	void setBufferData(size_t dataSize, void* data);
	byte* getBuffer()const;

private:
	size_t mBufferSize{ 0 };
	byte* mBuffer{ nullptr };

};