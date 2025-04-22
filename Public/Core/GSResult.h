// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"

namespace GS
{

template<typename ResultType>
struct ResultOrFail
{
	ResultType Value;
	bool bValid;

	ResultOrFail() { Value = ResultType();  bValid = false; }
	ResultOrFail(const ResultType& Val, bool valid) { Value = Val; bValid = valid; }
	ResultOrFail(const ResultType& Val) { Value = Val; bValid = true; }

	bool IsValid() const { return bValid; }
	bool Invalid() const { return !bValid; }
	operator bool() const { return bValid; }
	operator ResultType() const { return Value; }
};


}
