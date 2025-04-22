// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSMath.h"
#include "Math/GSVector3.h"
#include "Math/GSVector4.h"
#include "Math/GSQuaternion.h"
#include "Core/FunctionRef.h"

#include <array>

namespace GS
{

/**
 * Represents a series of steps of simple transformations
 * (currently only separate translate, rotate, or scales)
 */
template<typename RealType>
struct TransformList
{
	// transform types
	enum class ETransformTypes : uint8_t
	{
		GenericType = 0,
		TranslationType = 1,
		RotationType = 2,
		ScaleType = 3
	};

	// todo this is not ideal...at minimum wasting storage for translate & scale.
	// since we are always going in linear order we could just have a buffer of RealType
	// and pack the values in (and then uniform scale could be 1 element)
	struct Transform
	{
		RealType X;
		RealType Y;
		RealType Z;
		RealType W;
		ETransformTypes Type;
	};
	std::array<Transform, 12> Transforms;
	uint16_t Count = 0;
	bool bScaleInvertsOrientation = false;

	void AppendTranslation(const Vector3<RealType>& Translation)
	{
		if (Count > 0 && Transforms[Count-1].Type == ETransformTypes::TranslationType)
		{
			Transforms[Count-1].X += Translation.X;
			Transforms[Count-1].Y += Translation.Y;
			Transforms[Count-1].Z += Translation.Z;
		}
		else
		{
			Transforms[Count].X = Translation.X;
			Transforms[Count].Y = Translation.Y;
			Transforms[Count].Z = Translation.Z;
			Transforms[Count].W = 0;
			Transforms[Count].Type = ETransformTypes::TranslationType;
			Count++;
		}
	}


	void Reset()
	{
		Count = 0;
	}

	void AppendRotation(const Quaternion<RealType>& Rotation)
	{
		if (Count > 0 && Transforms[Count-1].Type == ETransformTypes::RotationType)
		{
			Quaternion<RealType> CurRotation(Transforms[Count].X, Transforms[Count].Y, Transforms[Count].Z, (RealType)1);
			CurRotation = Rotation * CurRotation;
			CurRotation.Normalize();
			Transforms[Count-1].X = CurRotation.X;
			Transforms[Count-1].Y = CurRotation.Y;
			Transforms[Count-1].Z = CurRotation.Z;
			Transforms[Count-1].W = CurRotation.W;
		}
		else
		{
			Quaternion<RealType> CurRotation = Rotation;
			CurRotation.Normalize();
			Transforms[Count].X = CurRotation.X;
			Transforms[Count].Y = CurRotation.Y;
			Transforms[Count].Z = CurRotation.Z;
			Transforms[Count].W = CurRotation.W;
			Transforms[Count].Type = ETransformTypes::RotationType;
			Count++;
		}
	}

	void AppendScale(const Vector3<RealType>& Scale)
	{
		if (Scale.X * Scale.Y * Scale.Z < 0) bScaleInvertsOrientation = !bScaleInvertsOrientation;

		if (Count > 0 && Transforms[Count-1].Type == ETransformTypes::ScaleType)
		{
			Transforms[Count-1].X *= Scale.X;
			Transforms[Count-1].Y *= Scale.Y;
			Transforms[Count-1].Z *= Scale.Z;
		}
		else
		{
			Transforms[Count].X = Scale.X;
			Transforms[Count].Y = Scale.Y;
			Transforms[Count].Z = Scale.Z;
			Transforms[Count].W = 0;
			Transforms[Count].Type = ETransformTypes::ScaleType;
			Count++;
		}
	}



	Vector3<RealType> TransformPosition(Vector3<RealType> P) const
	{
		for (uint16_t i = 0; i < Count; ++i)
		{
			const Transform& CurTransform = Transforms[i];
			switch (CurTransform.Type )
			{
				case ETransformTypes::TranslationType: {
					P.X += CurTransform.X; P.Y += CurTransform.Y; P.Z += CurTransform.Z;
				} break;

				case ETransformTypes::ScaleType: {
					P.X *= CurTransform.X; P.Y *= CurTransform.Y; P.Z *= CurTransform.Z;
				} break;

				case ETransformTypes::RotationType: {
					Quaternion<RealType> CurRotation(CurTransform.X, CurTransform.Y, CurTransform.Z, CurTransform.W);
					// todo could possibly do faster because we know this is a unit-quaternion
					P = CurRotation * P;
				} break;
			}
		}
		return P;
	}

	template<typename RealType>
	RealType SafeReciprocal(RealType Value, RealType Tolerance = RealConstants<RealType>::ZeroTolerance()) const
	{
		return (GS::Abs(Value) < Tolerance) ? (RealType)0 : ((RealType)1 / Value);
	}

	template<typename RealType>
	Vector3<RealType> SafeInverseScale(const Vector3<RealType>& Scale) const
	{
		RealType Determinant = Scale.X * Scale.Y * Scale.Z;
		RealType UseSign = (Determinant < (RealType)0) ? (RealType)-1 : (RealType)1;
		return Vector3<RealType>(Scale.Y*Scale.Z*UseSign, Scale.X*Scale.Z*UseSign, Scale.X*Scale.Y*UseSign);
	}


	Vector3<RealType> InverseTransformPosition(Vector3<RealType> P) const
	{
		for (int16_t i = (int16_t)Count-1; i >= 0; --i)
		{
			const Transform& CurTransform = Transforms[i];
			switch (CurTransform.Type)
			{
				case ETransformTypes::TranslationType: {
					P.X -= CurTransform.X; P.Y -= CurTransform.Y; P.Z -= CurTransform.Z;
				} break;

				case ETransformTypes::ScaleType: {
					P.X *= SafeReciprocal(CurTransform.X); P.Y *= SafeReciprocal(CurTransform.Y); P.Z *= SafeReciprocal(CurTransform.Z);
				} break;

				case ETransformTypes::RotationType: {
					Quaternion<RealType> CurRotation(CurTransform.X, CurTransform.Y, CurTransform.Z, CurTransform.W);
					// todo could possibly do faster because we know this is a unit-quaternion
					P = CurRotation.InverseMultiply(P);
				} break;
			}
		}
		return P;
	}


	Vector3<RealType> TransformVector(Vector3<RealType> V) const
	{
		for (uint16_t i = 0; i < Count; ++i)
		{
			const Transform& CurTransform = Transforms[i];
			switch (CurTransform.Type)
			{
				case ETransformTypes::ScaleType: {
					V.X *= CurTransform.X; V.Y *= CurTransform.Y; V.Z *= CurTransform.Z;
				} break;

				case ETransformTypes::RotationType: {
					Quaternion<RealType> CurRotation(CurTransform.X, CurTransform.Y, CurTransform.Z, CurTransform.W);
					// todo could possibly do faster because we know this is a unit-quaternion
					V = CurRotation * V;
				} break;
			}
		}
		return V;
	}

	Vector3<RealType> InverseTransformVector(Vector3<RealType> V) const
	{
		for (int16_t i = (int16_t)Count-1; i >= 0; --i)
		{
			const Transform& CurTransform = Transforms[i];
			switch (CurTransform.Type)
			{
				case ETransformTypes::ScaleType: {
					V.X *= SafeReciprocal(CurTransform.X); V.Y *= SafeReciprocal(CurTransform.Y); V.Z *= SafeReciprocal(CurTransform.Z);
				} break;

				case ETransformTypes::RotationType: {
					Quaternion<RealType> CurRotation(CurTransform.X, CurTransform.Y, CurTransform.Z, CurTransform.W);
					// todo could possibly do faster because we know this is a unit-quaternion
					V = CurRotation.InverseMultiply(V);
				} break;
			}
		}
		return V;
	}


	Vector3<RealType> TransformNormal(Vector3<RealType> N) const
	{
		for (uint16_t i = 0; i < Count; ++i)
		{
			const Transform& CurTransform = Transforms[i];
			switch (CurTransform.Type)
			{
				case ETransformTypes::ScaleType: {
					Vector3<RealType> InvScale = SafeInverseScale(Vector3<RealType>(CurTransform.X, CurTransform.Y, CurTransform.Z));
					N.X *= InvScale.X; N.Y *= InvScale.Y; N.Z *= InvScale.Z;
					N.Normalize();
				} break;

				case ETransformTypes::RotationType: {
					Quaternion<RealType> CurRotation(CurTransform.X, CurTransform.Y, CurTransform.Z, CurTransform.W);
					// todo could possibly do faster because we know this is a unit-quaternion
					N = CurRotation * N;
				} break;
			}
		}
		return N;
	}

	Vector3<RealType> InverseTransformNormal(Vector3<RealType> N) const
	{
		for (int16_t i = (int16_t)Count-1; i >= 0; --i)
		{
			const Transform& CurTransform = Transforms[i];
			switch (CurTransform.Type)
			{
				case ETransformTypes::ScaleType: {
					N.X *= CurTransform.X; N.Y *= CurTransform.Y; N.Z *= CurTransform.Z;
				} break;

				case ETransformTypes::RotationType: {
					Quaternion<RealType> CurRotation(CurTransform.X, CurTransform.Y, CurTransform.Z, CurTransform.W);
					// todo could possibly do faster because we know this is a unit-quaternion
					N = CurRotation.InverseMultiply(N);
				} break;
			}
		}
		return N;
	}


	void EnumerateTransforms(FunctionRef<void(const Vector3<RealType>& Translation, const Vector3<RealType>& Scale, const Quaternion<RealType>& Rotation)> NextTransformFunc) const
	{
		for (uint16_t i = 0; i < Count; ++i) {
			const Transform& CurTransform = Transforms[i];
			switch (CurTransform.Type)
			{
				case ETransformTypes::TranslationType: 
					NextTransformFunc(Vector3<RealType>(CurTransform.X, CurTransform.Y, CurTransform.Z), Vector3<RealType>::One(), Quaternion<RealType>::Identity() );
					break;
				case ETransformTypes::ScaleType: 
					NextTransformFunc(Vector3<RealType>::Zero(), Vector3<RealType>(CurTransform.X, CurTransform.Y, CurTransform.Z), Quaternion<RealType>::Identity());
					break;

				case ETransformTypes::RotationType: 
					NextTransformFunc(Vector3<RealType>::Zero(), Vector3<RealType>::One(), Quaternion<RealType>(CurTransform.X, CurTransform.Y, CurTransform.Z, CurTransform.W));
					break;
			}
		}
	}

};
typedef TransformList<float> TransformListf;
typedef TransformList<double> TransformListd;



}
