// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include <memory>

namespace GS
{

template<typename T>
using UniquePtr = std::unique_ptr<T>;

// todo figure out how to define this function?
#define GSMakeUniquePtr std::make_unique


} // end namespace GS
