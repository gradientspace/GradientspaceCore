// Copyright Gradientspace Corp. All Rights Reserved.

#pragma once

#include <memory>

namespace GS
{

template<typename T>
using SharedPtr = std::shared_ptr<T>;

// todo figure out how to define this function?
#define MakeSharedPtr std::make_shared


} // end namespace GS
