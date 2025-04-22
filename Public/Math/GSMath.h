// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include <cmath>
#include <cfloat>


namespace GS
{

template<typename RealType>
struct RealConstants;

template<>
struct RealConstants<float>
{
	static constexpr float Epsilon() { return 1.192092896e-07F /*FLT_EPSILON*/; }
	static constexpr float ZeroTolerance() { return 1e-06f; }
	
	static constexpr float NormalizeTolerance() { return Epsilon(); }

	static constexpr float MinValue() { return 1.175494351e-38F; }
	static constexpr float MinDenormValue() { return 1.401298464e-45F; }
	static constexpr float MaxValue() { return 3.402823466e+38F; }
	// sqrt(MaxValue), will not overflow if Max*Max is computed
	static constexpr float SafeMaxValue() { return 1.844674297e+19F; }
	// sqrt(MaxValue)/4, will not overflow if 3 * (Max - -Max)^2 is computed  (eg length of bounding-box diagonal)
	static constexpr float SafeMaxExtent() { return 4.611685744e+18F; }


	// TODO: get max-precision values for these? calculate in double and convert to float?
	static constexpr float Pi() { return 3.14159265358979323846f; }
	static constexpr float RadToDeg() { return 180.0f / Pi(); }
	static constexpr float DegToRad() { return Pi() / 180.0f; }
};

template<>
struct RealConstants<double>
{
	static constexpr double Epsilon() { return 2.2204460492503131e-016 /*DBL_EPSILON*/; }
	static constexpr double ZeroTolerance() { return 1e-08; }

	static constexpr double NormalizeTolerance() { return Epsilon(); }

	static constexpr double MinValue() { return 2.2250738585072014e-308; }
	static constexpr double MinDenormValue() { return 4.9406564584124654e-324; }
	static constexpr double MaxValue() { return 1.7976931348623158e+308; } 
	//! sqrt(MaxValue), will not overflow if Max*Max is computed
	static constexpr double SafeMaxValue() { return 1.3407807929942596e+154; }
	//! sqrt(MaxValue)/4, will not overflow if 3 * (Max - -Max)^2 is computed  (eg length of bounding-box diagonal)
	static constexpr double SafeMaxExtent() { return 3.3519519824856489e+153; }


	static constexpr double Pi() { return 3.1415926535897932384626433832795; }
	static constexpr double RadToDeg() { return 180.0 / Pi(); }
	static constexpr double DegToRad() { return Pi() / 180.0; }
};

template<typename RealType>
class RealMath : public RealConstants<RealType>
{
public:
};
typedef RealMath<float> Mathf;
typedef RealMath<double> Mathd;


template<typename IntType>
struct IntConstants;

template<>
struct IntConstants<uint16_t>
{
	static constexpr uint16_t MinValue() { return (uint16_t)0; }
	static constexpr uint16_t MaxValue() { return (uint16_t)0xffff; }
};

template<>
struct IntConstants<int>
{
	static constexpr int MinValue() { return (int)0; }
	static constexpr int MaxValue() { return (int)0x7fffffff; }
};

template<>
struct IntConstants<unsigned int>
{
	static constexpr unsigned int MinValue() { return (unsigned int)0; }
	static constexpr unsigned int MaxValue() { return (unsigned int)0xffffffff; }
};

template<typename IntType>
class IntMath : public IntConstants<IntType>
{
public:
};
typedef IntMath<uint16_t> Math16u;
typedef IntMath<int> Math32i;
typedef IntMath<unsigned int> Math32u;


template<typename ValueType>
void SwapTemp(ValueType& A, ValueType& B)
{
	ValueType Tmp = A; A = B; B = Tmp;
}

template<typename ValueType>
void SwapMove(ValueType& A, ValueType& B)
{
	ValueType Tmp = std::move(A);
	A = std::move(B);
	B = std::move(Tmp);
}


template<typename ValueType>
ValueType Max(ValueType A, ValueType B)
{
	return (A > B) ? A : B;
}

template<typename ValueType>
ValueType Max3(ValueType A, ValueType B, ValueType C)
{
	return Max(A, Max(B, C));
}

template<typename ValueType>
ValueType Min(ValueType A, ValueType B)
{
	return (A < B) ? A: B;
}

template<typename ValueType>
ValueType Min3(ValueType A, ValueType B, ValueType C)
{
	return Min(A, Min(B, C));
}

template<typename ValueType>
int Min3Index(ValueType A, ValueType B, ValueType C)
{
	if (A <= B) return (A <= C) ? 0 : 2;
	else return (B <= C) ? 1 : 2;
}

template<typename ValueType>
int Max3Index(ValueType A, ValueType B, ValueType C)
{
	if (A >= B) return (A >= C) ? 0 : 2;
	else return (B >= C) ? 1 : 2;
}

template<typename ValueType, typename SelectType>
SelectType Min3Select(ValueType A, ValueType B, ValueType C, SelectType SA, SelectType SB, SelectType SC)
{
	if (A <= B) return (A <= C) ? SA : SC;
	else return (B <= C) ? SB : SC;
}

template<typename ValueType, typename SelectType>
SelectType Max3Select(ValueType A, ValueType B, ValueType C, SelectType SA, SelectType SB, SelectType SC)
{
	if (A >= B) return (A >= C) ? SA : SC;
	else return (B >= C) ? SB : SC;
}

template<typename ValueType>
ValueType Clamp(ValueType Value, ValueType ClampMin, ValueType ClampMax)
{
	return (Value < ClampMin) ? ClampMin : ( (Value > ClampMax) ? ClampMax : Value );
}

//! returns -1, 0, or 1
template<typename ValueType>
int IntSign(ValueType Value) 
{ 
	return (Value > (ValueType)0) ? 1 : ((Value < (ValueType)0) ? -1 : 0); 
}

//! returns -1, 0, or 1
template<typename ValueType>
ValueType Sign(ValueType Value)
{
	return (Value > (ValueType)0) ? (ValueType)1 : ((Value < (ValueType)0) ? (ValueType)-1 : (ValueType)0);
}

//! returns -1, 0, or 1
template<typename ValueType>
bool IsPowerOfTwo(ValueType Value)
{
	static_assert(std::is_integral<ValueType>());
	return (Value > 0) && ((Value & (Value - 1)) == 0);
}

template<typename ValueType>
ValueType Lerp(ValueType A, ValueType B, ValueType Alpha)
{
	return ((ValueType)1-Alpha)*A + (Alpha*B);
}

template<typename RealType> RealType Abs(RealType Value) { return std::abs(Value); }
template<typename RealType> RealType FAbs(RealType Value) { return std::fabs(Value); }
template<typename RealType> RealType FMod(RealType X, RealType Y) { return std::fmod(X, Y); }
template<typename RealType> RealType Floor(RealType Value) { return std::floor(Value); }
template<typename RealType> RealType Ceil(RealType Value) { return std::ceil(Value); }

template<typename RealType> RealType Sqrt(RealType Value) { return std::sqrt(Value); }
template<typename RealType> RealType Pow(RealType Value, RealType Exponent) { return std::pow(Value, Exponent); }

template<typename RealType> RealType Sin(RealType Value) { return std::sin(Value); }
template<typename RealType> RealType Cos(RealType Value) { return std::cos(Value); }
template<typename RealType> RealType ACos(RealType Value) { return std::acos(Value); }
template<typename RealType> RealType ATan2(RealType Y, RealType X) { return std::atan2(Y, X); }
template<typename RealType> RealType RadToDeg(RealType Value) { return Value * RealMath<RealType>::RadToDeg(); }
template<typename RealType> RealType DegToRad(RealType Value) { return Value * RealMath<RealType>::DegToRad(); }

template<typename ValueType>
bool EpsilonEqual(ValueType A, ValueType B, ValueType Epsilon = RealConstants<ValueType>::Epsilon())
{
	return std::abs(A - B) < Epsilon;
}
template<typename ValueType>
bool ToleranceEqual(ValueType A, ValueType B, ValueType Epsilon = RealConstants<ValueType>::ZeroTolerance())
{
	return std::abs(A - B) < Epsilon;
}


}
