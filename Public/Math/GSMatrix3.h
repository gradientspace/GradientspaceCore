// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSMath.h"
#include "Math/GSHash.h"
#include "Math/GSVector3.h"

namespace GS
{

template<typename RealType>
struct Matrix3
{
	union {
		struct {
			RealType M00;
			RealType M01;
			RealType M02;

			RealType M10;
			RealType M11;
			RealType M12;

			RealType M20;
			RealType M21;
			RealType M22;
		};
		RealType M[9] = { {}, {}, {}, {}, {}, {}, {}, {}, {} };
	};

	Matrix3();
	explicit Matrix3(RealType InitialValue);
	explicit Matrix3(RealType diag00, RealType diag11, RealType diag22);
	explicit Matrix3(RealType m00, RealType m01, RealType m02,
					 RealType m10, RealType m11, RealType m12,
					 RealType m20, RealType m21, RealType m22);
	explicit Matrix3(const RealType* Values);
	explicit Matrix3(Vector3<RealType> U, Vector3<RealType> V, Vector3<RealType> W, bool bVectorsAreColumns = false);
	explicit Matrix3(Vector3<RealType> Axis, RealType Angle, bool bAngleIsDegrees);

	static Matrix3 One() { return Matrix3((RealType)1); }
	static Matrix3 Zero() { return Matrix3((RealType)0); }
	static Matrix3 Identity() { return Matrix3((RealType)1, (RealType)1, (RealType)1); }

	RealType& operator[](int index) {
		return M[index];
	}

	RealType operator[](int index) const {
		return M[index];
	}

	RealType operator()(int r, int c) const {
		return M[r * 3 + c];
	}

	RealType* AsPointer() {
		return &M[0];
	}
	const RealType* AsPointer() const {
		return &M[0];
	}

	void SetToDiagonal(RealType m00, RealType m11, RealType m22);

	Matrix3 Transposed() const;
	Matrix3 Inverse(bool* bIsUnInvertible = nullptr) const;
	RealType Determinant() const;

	constexpr Matrix3 operator-() const {
		return Matrix3(-M00, -M01, -M02, -M10, -M11, -M22, -M20, -M21, -M22);
	}

	constexpr Matrix3 operator+(const Matrix3& Mat2) const {
		return Matrix3(
			M00 + Mat2.M00, M01 + Mat2.M01, M02 + Mat2.M02,
			M10 + Mat2.M10, M11 + Mat2.M11, M12 + Mat2.M12,
			M20 + Mat2.M20, M21 + Mat2.M21, M22 + Mat2.M22);
	}
	constexpr Matrix3 operator-(const Matrix3& Mat2) const {
		return Matrix3(
			M00 - Mat2.M00, M01 - Mat2.M01, M02 - Mat2.M02,
			M10 - Mat2.M10, M11 - Mat2.M11, M12 - Mat2.M12,
			M20 - Mat2.M20, M21 - Mat2.M21, M22 - Mat2.M22);
	}

	constexpr Matrix3 operator*(RealType Scalar) const {
		return Matrix3(
			M00 * Scalar, M01 * Scalar, M02 * Scalar,
			M10 * Scalar, M11 * Scalar, M12 * Scalar,
			M20 * Scalar, M21 * Scalar, M22 * Scalar);
	}
	template<typename RealType2>
	constexpr Matrix3 operator*(RealType2 ScalarOtherType) const {
		RealType Scalar = (RealType)ScalarOtherType;
		return Matrix3(
			M00 * Scalar, M01 * Scalar, M02 * Scalar,
			M10 * Scalar, M11 * Scalar, M12 * Scalar,
			M20 * Scalar, M21 * Scalar, M22 * Scalar);
	}
	constexpr Matrix3 operator/(RealType Scalar) const {
		return Matrix3(
			M00 / Scalar, M01 / Scalar, M02 / Scalar,
			M10 / Scalar, M11 / Scalar, M12 / Scalar,
			M20 / Scalar, M21 / Scalar, M22 / Scalar);
	}
	template<typename RealType2>
	constexpr Matrix3 operator/(RealType2 ScalarOtherType) const {
		RealType Scalar = (RealType)ScalarOtherType;
		return Matrix3(
			M00 / Scalar, M01 / Scalar, M02 / Scalar,
			M10 / Scalar, M11 / Scalar, M12 / Scalar,
			M20 / Scalar, M21 / Scalar, M22 / Scalar);
	}

	constexpr Matrix3& operator+=(const Matrix3& Mat2) {
		M00 += Mat2.M00; M01 += Mat2.M01; M02 += Mat2.M02;
		M10 += Mat2.M10; M11 += Mat2.M11; M12 += Mat2.M12;
		M20 += Mat2.M20; M21 += Mat2.M21; M22 += Mat2.M22;
		return *this;
	}
	constexpr Matrix3& operator-=(const Matrix3& Mat2) {
		M00 -= Mat2.M00; M01 -= Mat2.M01; M02 -= Mat2.M02;
		M10 -= Mat2.M10; M11 -= Mat2.M11; M12 -= Mat2.M12;
		M20 -= Mat2.M20; M21 -= Mat2.M21; M22 -= Mat2.M22;
		return *this;
	}
	constexpr Matrix3& operator*=(RealType Scalar) {
		M00 *= Scalar; M01 *= Scalar; M02 *= Scalar;
		M10 *= Scalar; M11 *= Scalar; M12 *= Scalar;
		M20 *= Scalar; M21 *= Scalar; M22 *= Scalar;
		return *this;
	}
	template<typename RealType2>
	constexpr Matrix3& operator*=(RealType2 ScalarOtherType) {
		RealType Scalar = (RealType)ScalarOtherType;
		M00 *= Scalar; M01 *= Scalar; M02 *= Scalar;
		M10 *= Scalar; M11 *= Scalar; M12 *= Scalar;
		M20 *= Scalar; M21 *= Scalar; M22 *= Scalar;
		return *this;
	}
	constexpr Matrix3& operator/=(RealType Scalar) {
		M00 /= Scalar; M01 /= Scalar; M02 /= Scalar;
		M10 /= Scalar; M11 /= Scalar; M12 /= Scalar;
		M20 /= Scalar; M21 /= Scalar; M22 /= Scalar;
		return *this;
	}
	template<typename RealType2>
	constexpr Matrix3& operator/=(RealType2 ScalarOtherType) {
		RealType Scalar = (RealType)ScalarOtherType;
		M00 /= Scalar; M01 /= Scalar; M02 /= Scalar;
		M10 /= Scalar; M11 /= Scalar; M12 /= Scalar;
		M20 /= Scalar; M21 /= Scalar; M22 /= Scalar;
		return *this;
	}

	//! matrix/matrix multiply
	constexpr Matrix3 operator*(const Matrix3& Mat2) const {
		return Matrix3(
			M00*Mat2.M00 + M01*Mat2.M10 + M02*Mat2.M20,
			M00*Mat2.M01 + M01*Mat2.M11 + M02*Mat2.M21,
			M00*Mat2.M02 + M01*Mat2.M12 + M02*Mat2.M22,

			M10*Mat2.M00 + M11*Mat2.M10 + M12*Mat2.M20,
			M10*Mat2.M01 + M11*Mat2.M11 + M12*Mat2.M21,
			M10*Mat2.M02 + M11*Mat2.M12 + M12*Mat2.M22,

			M20*Mat2.M00 + M21*Mat2.M10 + M22*Mat2.M20,
			M20*Mat2.M01 + M21*Mat2.M11 + M22*Mat2.M21,
			M20*Mat2.M02 + M21*Mat2.M12 + M22*Mat2.M22 );
	}

	//! matrix/matrix multiply
	constexpr Matrix3& operator*=(const Matrix3& Mat2) const {
		(*this) = (*this) * Mat2;
		return *this;
	}

	//! matrix/vector multiply
	constexpr Vector3<RealType> operator*(const Vector3<RealType>& Vec) const {
		return Vector3<RealType>(
			M00 * Vec.X + M01 * Vec.Y + M02 * Vec.Z, 
			M10 * Vec.X + M11 * Vec.Y + M12 * Vec.Z,
			M20 * Vec.X + M21 * Vec.Y + M22 * Vec.Z);
	}


	constexpr bool operator==(const Matrix3& Other) const {
		return M00 == Other.M00 && M01 == Other.M01 && M02 == Other.M02
			&& M10 == Other.M10 && M11 == Other.M11 && M12 == Other.M12
			&& M20 == Other.M20 && M21 == Other.M21 && M22 == Other.M22;
	}
	constexpr bool operator!=(const Matrix3& Other) const {
		return !(*this == Other);
	}

	constexpr bool EpsilonEqual(const Matrix3& Other, RealType Epsilon = RealConstants<RealType>::Epsilon()) const {
		return (GS::Abs(M00-Other.M00) < Epsilon) && (GS::Abs(M01-Other.M01) < Epsilon) && (GS::Abs(M02-Other.M02) < Epsilon)
			&& (GS::Abs(M10-Other.M10) < Epsilon) && (GS::Abs(M11-Other.M11) < Epsilon) && (GS::Abs(M12-Other.M12) < Epsilon)
			&& (GS::Abs(M20-Other.M20) < Epsilon) && (GS::Abs(M21-Other.M21) < Epsilon) && (GS::Abs(M22-Other.M22) < Epsilon);
	}

	template<typename RealType2>
	explicit operator Matrix3<RealType2>() const {
		return Matrix3<RealType2>(
			(RealType2)M00, (RealType2)M01, (RealType2)M02,
			(RealType2)M10, (RealType2)M11, (RealType2)M12,
			(RealType2)M20, (RealType2)M21, (RealType2)M22);
	}

};
typedef Matrix3<float> Matrix3f;
typedef Matrix3<double> Matrix3d;


template<typename RealType>
Matrix3<RealType>::Matrix3()
{
	// is_trivial is false because of constructors
	static_assert(std::is_standard_layout<Matrix3<RealType>>());
	static_assert(std::is_trivially_copyable<Matrix3<RealType>>());
	static_assert(std::is_floating_point<RealType>());
}

template<typename RealType>
Matrix3<RealType>::Matrix3(RealType InitialValue)
{
	M00 = M01 = M02 = InitialValue;
	M10 = M11 = M12 = InitialValue;
	M20 = M21 = M22 = InitialValue;
}

template<typename RealType>
Matrix3<RealType>::Matrix3(RealType diag00, RealType diag11, RealType diag22)
{
	M00 = diag00; M11 = diag11; M22 = diag22;
	M01 = M02 = M10 = M12 = M20 = M21 = (RealType)0;
}


template<typename RealType>
Matrix3<RealType>::Matrix3(RealType m00, RealType m01, RealType m02,
	RealType m10, RealType m11, RealType m12,
	RealType m20, RealType m21, RealType m22)
{
	M00 = m00;  M01 = m01;  M02 = m02;
	M10 = m11;  M11 = m11;  M12 = m12;
	M20 = m20;  M21 = m21;  M22 = m22;
}

template<typename RealType>
Matrix3<RealType>::Matrix3(const RealType* values)
{
	M00 = values[0];  M01 = values[1];  M02 = values[2];
	M10 = values[3];  M11 = values[4];  M12 = values[5];
	M20 = values[6];  M21 = values[7];  M22 = values[8];
}

template<typename RealType>
Matrix3<RealType>::Matrix3(Vector3<RealType> U, Vector3<RealType> V, Vector3<RealType> W, bool bVectorsAreColumns)
{
	if (bVectorsAreColumns) {
		M00 = U[0];  M01 = V[0];  M02 = W[0];
		M10 = U[1];  M11 = V[1];  M12 = W[1];
		M20 = U[2];  M21 = V[2];  M22 = W[2];
	} else {
		M00 = U[0];  M01 = U[1];  M02 = U[2];
		M10 = V[0];  M11 = V[1];  M12 = V[2];
		M20 = W[0];  M21 = W[1];  M22 = W[2];
	}
}


template<typename RealType>
Matrix3<RealType>::Matrix3(Vector3<RealType> Axis, RealType Angle, bool bAngleIsDegrees)
{
	if (bAngleIsDegrees)
		Angle *= GS::RealMath<RealType>::DegToRad();

	RealType cs = GS::Cos(Angle);
	RealType sn = GS::Sin(Angle);
	RealType oneMinusCos = (RealType)1 - cs;
	RealType x2 = Axis[0] * Axis[0];
	RealType y2 = Axis[1] * Axis[1];
	RealType z2 = Axis[2] * Axis[2];
	RealType xym = Axis[0] * Axis[1] * oneMinusCos;
	RealType xzm = Axis[0] * Axis[2] * oneMinusCos;
	RealType yzm = Axis[1] * Axis[2] * oneMinusCos;
	RealType xSin = Axis[0] * sn;
	RealType ySin = Axis[1] * sn;
	RealType zSin = Axis[2] * sn;

	M00 = x2 * oneMinusCos + cs; M01 = xym - zSin; M02 = xzm + ySin;
	M10 = xym + zSin;  M11 = y2 * oneMinusCos + cs;  M12 = yzm - xSin;
	M20 = xzm - ySin;  M21 = yzm + xSin;  M22 = z2 * oneMinusCos + cs;
}



template<typename RealType>
void Matrix3<RealType>::SetToDiagonal(RealType m00, RealType m11, RealType m22) {
	*this = Matrix3(m00, m11, m22);
}

template<typename RealType>
Matrix3<RealType> Matrix3<RealType>::Transposed() const
{
	return Matrix3<RealType>(M00, M10, M20, M01, M11, M21, M02, M12, M22);
}

template<typename RealType>
Matrix3<RealType> Matrix3<RealType>::Inverse(bool *bIsUnInvertible) const
{
	// ported from g3sharp, can rewrite...
	double a11 = M00, a12 = M01, a13 = M02, a21 = M10, a22 = M11, a23 = M12, a31 = M20, a32 = M21, a33 = M22;
	RealType i00 = a33 * a22 - a32 * a23;
	RealType i01 = -(a33 * a12 - a32 * a13);
	RealType i02 = a23 * a12 - a22 * a13;

	RealType i10 = -(a33 * a21 - a31 * a23);
	RealType i11 = a33 * a11 - a31 * a13;
	RealType i12 = -(a23 * a11 - a21 * a13);

	RealType i20 = a32 * a21 - a31 * a22;
	RealType i21 = -(a32 * a11 - a31 * a12);
	RealType i22 = a22 * a11 - a21 * a12;

	RealType det = a11 * i00 + a21 * i01 + a31 * i02;
	if (GS::Abs(det) > GS::RealMath<RealType>::Epsilon()) 
	{
		det = 1.0 / det;
		if (bIsUnInvertible != nullptr)
			*bIsUnInvertible = false;
		return Matrix3<RealType>(i00 * det, i01 * det, i02 * det, i10 * det, i11 * det, i12 * det, i20 * det, i21 * det, i22 * det);
	}

	if (bIsUnInvertible != nullptr)
		*bIsUnInvertible = true;
	return Matrix3<RealType>::Zero();
}


template<typename RealType>
RealType Matrix3<RealType>::Determinant() const
{
	// ported from g3sharp, can rewrite...
	double a11 = M00, a12 = M01, a13 = M02, a21 = M10, a22 = M11, a23 = M12, a31 = M20, a32 = M21, a33 = M22;
	double i00 = a33 * a22 - a32 * a23;
	double i01 = -(a33 * a12 - a32 * a13);
	double i02 = a23 * a12 - a22 * a13;
	return a11 * i00 + a21 * i01 + a31 * i02;
}


} // end namespace GS