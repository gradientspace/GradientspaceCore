// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/gs_debug.h"

namespace GS
{

class gs_allocator
{
public:
	virtual unsigned char* allocate(size_t bytes) = 0;
	virtual void free(unsigned char* memory) = 0;
};

class gs_default_allocator
{
public:
	static unsigned char* allocate(size_t bytes)
	{
		return new unsigned char[bytes];
	}

	static void free(unsigned char* memory)
	{
		delete[] memory;
	}
};

} // end namespace GS

