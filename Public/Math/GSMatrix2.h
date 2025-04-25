// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSMath.h"
#include "Math/GSHash.h"
#include "Math/GSVector2.h"

namespace GS
{

template<typename RealType>
struct Matrix2
{
	union {
		struct {
			RealType M00;
			RealType M01;
			RealType M10;
			RealType M11;
		};
		RealType M[4] = { {}, {}, {}, {} };
	};

	Matrix2();
	explicit Matrix2(RealType InitialValue);
	explicit Matrix2(RealType diag00, RealType diag11);
	explicit Matrix2(RealType m00, RealType m01, RealType m10, RealType m11);
	explicit Matrix2(const RealType* Values);
	explicit Matrix2(RealType Angle, bool bAngleIsDegrees);
	explicit Matrix2(Vector2d U, Vector2d V, bool bVectorsAreColumns = false);

	static Matrix2 One() { return Matrix2((RealType)1); }
	static Matrix2 Zero() { return Matrix2((RealType)0); }
	static Matrix2 Identity() { return Matrix2((RealType)1, (RealType)1); }

	RealType& operator[](int index) {
		return M[index];
	}

	RealType operator[](int index) const {
		return M[index];
	}

	RealType* AsPointer() {
		return &M[0];
	}
	const RealType* AsPointer() const {
		return &M[0];
	}


	void SetToDiagonal(RealType m00, RealType m11);
	void SetToRotationRad(RealType angleRad);
	void SetToRotationDeg(RealType angleDeg);

	Matrix2 Transposed() const;
	Matrix2 Adjoint() const;
	Matrix2 Inverse() const;
	RealType Determinant() const;
	RealType ToAngle() const;

	Matrix2 Orthonormalized() const;
	void EigenDecomposition(Matrix2<RealType>& Rotation, Matrix2<RealType>& Diagonal) const;

	//! return tensor product U * V^T
	static Matrix2<RealType> TensorProduct(Vector2d U, Vector2d V);



	constexpr Matrix2 operator-() const {
		return Matrix2(-M[0], -M[1], -M[2], -M[3]);
	}

	constexpr Matrix2 operator+(const Matrix2& Mat2) const {
		return Matrix2(M[0]+Mat2.M[0], M[1]+Mat2.M[1], M[2]+Mat2.M[2], M[3]+Mat2.M[3]);
	}
	constexpr Matrix2 operator-(const Matrix2& Mat2) const {
		return Matrix2(M[0]-Mat2.M[0], M[1]-Mat2.M[1], M[2]-Mat2.M[2], M[3]-Mat2.M[3]);
	}

	constexpr Matrix2 operator*(RealType Scalar) const {
		return Matrix2(M[0]*Scalar, M[1]*Scalar, M[2]*Scalar, M[3]*Scalar);
	}
	template<typename RealType2>
	constexpr Matrix2 operator*(RealType2 Scalar) const {
		return Matrix2(M[0]*(RealType)Scalar, M[1]*(RealType)Scalar, M[2]*(RealType)Scalar, M[3]*(RealType)Scalar);
	}
	constexpr Matrix2 operator/(RealType Scalar) const {
		return Matrix2(M[0]/Scalar, M[1]/Scalar, M[2]/Scalar, M[3]/Scalar);
	}
	template<typename RealType2>
	constexpr Matrix2 operator/(RealType2 Scalar) const {
		return Matrix2(M[0]/(RealType)Scalar, M[1]/(RealType)Scalar, M[2]/(RealType)Scalar, M[3]/(RealType)Scalar);
	}

	constexpr Matrix2& operator+=(const Matrix2& Mat2) {
		M[0] += Mat2.M[0]; M[1] += Mat2.M[1]; M[2] += Mat2.M[2]; M[3] += Mat2.M[3];
		return *this;
	}
	constexpr Matrix2& operator-=(const Matrix2& Mat2) {
		M[0] -= Mat2.M[0]; M[1] -= Mat2.M[1]; M[2] -= Mat2.M[2]; M[3] -= Mat2.M[3];
		return *this;
	}
	constexpr Matrix2& operator*=(RealType Scalar) {
		M[0] *= Scalar; M[1] *= Scalar; M[2] *= Scalar; M[3] *= Scalar;
		return *this;
	}
	template<typename RealType2>
	constexpr Matrix2& operator*=(RealType2 Scalar) {
		M[0] *= (RealType)Scalar; M[1] *= (RealType)Scalar; M[2] *= (RealType)Scalar; M[3] *= (RealType)Scalar;
		return *this;
	}
	constexpr Matrix2& operator/=(RealType Scalar) {
		M[0] /= Scalar; M[1] /= Scalar; M[2] /= Scalar; M[3] /= Scalar;
		return *this;
	}
	template<typename RealType2>
	constexpr Matrix2& operator/=(RealType2 Scalar) {
		M[0] /= (RealType)Scalar; M[1] /= (RealType)Scalar; M[2] /= (RealType)Scalar; M[3] /= (RealType)Scalar;
		return *this;
	}

	//! matrix/matrix multiply
	constexpr Matrix2 operator*(const Matrix2& Mat2) const {
		return Matrix2(
			M00*Mat2.M00 + M01*Mat2.M10,
			M00*Mat2.M01 + M01*Mat2.M11,
			M10*Mat2.M00 + M11*Mat2.M10,
			M10*Mat2.M01 + M11*Mat2.M11);
	}

	//! matrix/matrix multiply
	constexpr Matrix2& operator*=(const Matrix2& Mat2) const {
		*this = *this * Mat2;
		return *this;
	}

	//! matrix/vector multiply
	constexpr Vector2<RealType> operator*(const Vector2<RealType>& Vec) const {
		return Vector2<RealType>(M00 * Vec.X + M01 * Vec.Y, M10 * Vec.X + M11 * Vec.Y);
	}


	constexpr bool operator==(const Matrix2& Other) const {
		return M00 == Other.M00 && M01 == Other.M01 && M10 == Other.M10 && M11 == Other.M11;
	}
	constexpr bool operator!=(const Matrix2& Other) const {
		return M00 != Other.M00 || M01 != Other.M01 || M10 != Other.M10 || M11 != Other.M11;
	}

	constexpr bool EpsilonEqual(const Matrix2& Other, RealType Epsilon = RealConstants<RealType>::Epsilon()) const {
		return (GS::Abs(M00 - Other.M00) < Epsilon) && (GS::Abs(M01 - Other.M01) < Epsilon) && (GS::Abs(M10 - Other.M10) < Epsilon) && (GS::Abs(M11 - Other.M11) < Epsilon);
	}

	template<typename RealType2>
	explicit operator Matrix2<RealType2>() const {
		return Matrix2<RealType2>((RealType2)M00, (RealType2)M01, (RealType2)M10, (RealType2)M11);
	}

	// TODO unreal casting
};
typedef Matrix2<float> Matrix2f;
typedef Matrix2<double> Matrix2d;



template<typename RealType>
Matrix2<RealType>::Matrix2()
{
	// is_trivial is false because of constructors
	static_assert(std::is_standard_layout<Matrix2<RealType>>());
	static_assert(std::is_trivially_copyable<Matrix2<RealType>>());
	static_assert(std::is_floating_point<RealType>());
}

template<typename RealType>
Matrix2<RealType>::Matrix2(RealType InitialValue)
{
	M[0] = M[1] = M[2] = M[3] = InitialValue;
}

template<typename RealType>
Matrix2<RealType>::Matrix2(RealType diag00, RealType diag11)
{
	M[0] = diag00;  M[1] = M[2] = (RealType)0;  M[3] = diag11;
}

template<typename RealType>
Matrix2<RealType>::Matrix2(RealType m00, RealType m01, RealType m10, RealType m11)
{
	M[0] = m00; M[1] = m01; M[2] = m10; M[3] = m11;
}

template<typename RealType>
Matrix2<RealType>::Matrix2(const RealType* Values)
{
	M[0] = Values[0]; M[1] = Values[1]; M[2] = Values[2]; M[3] = Values[3];
}

template<typename RealType>
Matrix2<RealType>::Matrix2(RealType Angle, bool bAngleIsDegrees)
{
	if (bAngleIsDegrees) SetToRotationDeg(Angle);
	else SetToRotationRad(Angle);
}

template<typename RealType>
Matrix2<RealType>::Matrix2(Vector2d U, Vector2d V, bool bVectorsAreColumns)
{
	if (bVectorsAreColumns) {
		M[0] = U[0]; M[1] = U[1]; M[2] = V[0]; M[3] = V[1];
	} else {
		M[0] = U[0]; M[1] = V[0]; M[2] = U[1]; M[3] = V[1];
	}
}


template<typename RealType>
void Matrix2<RealType>::SetToDiagonal(RealType m00, RealType m11) {
	M[0] = m00; M[1] = (RealType)0; M[2] = (RealType)0; M[3] = m11;
}

template<typename RealType>
void Matrix2<RealType>::SetToRotationRad(RealType angleRad) {
	M11 = M00 = GS::Cos(angleRad);
	M10 = GS::Sin(angleRad);
	M01 = -M10;
}
template<typename RealType>
void Matrix2<RealType>::SetToRotationDeg(RealType angleDeg) {
	SetToRotationRad(GS::RealMath<RealType>::DegToRad() * angleDeg);
}

template<typename RealType>
Matrix2<RealType> Matrix2<RealType>::Transposed() const
{
	return Matrix2<RealType>(M00, M10, M01, M11);
}

template<typename RealType>
Matrix2<RealType> Matrix2<RealType>::Adjoint() const
{
	return Matrix2<RealType>(M11, -M01, -M10, M00);
}

template<typename RealType>
Matrix2<RealType> Matrix2<RealType>::Inverse() const
{
	RealType det = M00 * M11 - M01 * M10;
	if (GS::Abs(det) > Mathd::Epsilon) {
		RealType invDet = (RealType)1 / det;
		return Matrix2<RealType>(M11 * invDet, -M01 * invDet, -M10 * invDet, M00 * invDet);
	}
	else
		return Zero();
}

template<typename RealType>
RealType Matrix2<RealType>::Determinant() const
{
	return M00 * M11 - M01 * M10;
}

template<typename RealType>
RealType Matrix2<RealType>::ToAngle() const
{
	return GS::ATan2(M10, M00);
}


template<typename RealType>
Matrix2<RealType> Matrix2<RealType>::Orthonormalized() const
{
	// Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
	// M = [m0|m1], then orthonormal output matrix is Q = [q0|q1],
	//
	//   q0 = m0/|m0|
	//   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
	//
	// where |V| indicates length of vector V and A*B indicates dot
	// product of vectors A and B.

	RealType m00 = M00, m01 = M01, m10 = M10, m11 = M11;

	// Compute q0.
	RealType invLength = (RealType)1 / GS::Sqrt(m00 * m00 + m10 * m10);
	m00 *= invLength; m10 *= invLength;

	// Compute q1.
	RealType dot0 = m00 * m01 + m10 * m11;
	m01 -= dot0 * m00; m11 -= dot0 * m10;
	invLength = (RealType)1 / GS::Sqrt(m01 * m01 + m11 * m11);
	m01 *= invLength; m11 *= invLength;

	return Matrxi2<RealType>(m00, m01, m10, m11);
}


template<typename RealType>
void Matrix2<RealType>::EigenDecomposition(Matrix2<RealType>& Rotation, Matrix2<RealType>& Diagonal) const
{
	RealType sum = GS::Abs(M00) + GS::Abs(M11);
	if (GS::Abs(M01) + sum == sum) {
		// The matrix M is diagonal (within numerical round-off).
		Rotation = Identity();
		Diagonal = GS::Matrix2(M00, M11);
		return;
	}

	RealType trace = M00 + M11;
	RealType diff = M00 - M11;
	RealType discr = GS::Sqrt(diff * diff + ((RealType)4) * M01 * M01);
	RealType eigVal0 = (RealType)0.5 * (trace - discr);
	RealType eigVal1 = (RealType)0.5 * (trace + discr);
	Diagonal = Matrix2<RealType>(eigVal0, eigVal1);

	RealType cs, sn;
	if (diff >= 0) {
		cs = M01;
		sn = eigVal0 - M00;
	} else {
		cs = eigVal0 - M11;
		sn = M01;
	}
	RealType invLength = (RealType)1 / GS::Sqrt(cs * cs + sn * sn);
	cs *= invLength; sn *= invLength;
	Rotation = Matrix2<RealType>(cs, -sn, sn, cs);
}



template<typename RealType>
Matrix2<RealType> Matrix2<RealType>::TensorProduct(Vector2d U, Vector2d V)
{
	return Matrix2<RealType>( U.X * V.X, U.X * V.Y, U.Y * V.X, U.Y * V.Y );
}


} // end namespace GS