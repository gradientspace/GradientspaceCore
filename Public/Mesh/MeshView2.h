// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSVector2.h"
#include "Math/GSTriangle2.h"
#include "Math/GSIndex3.h"

namespace GS
{



template<typename RealType>
class TConstMeshView2
{
protected:
	const uint8_t* m_vertex_buffer = nullptr;
	int m_vertex_count = 0;
	int m_vertex_stride = 2*sizeof(RealType);
	const uint8_t* m_triangle_buffer = nullptr;
	int m_triangle_count = 0;
	int m_triangle_stride = 3*sizeof(int);
public:
	TConstMeshView2() {}

	~TConstMeshView2() {
		m_vertex_buffer = nullptr;
		m_vertex_count = m_vertex_stride = 0;
		m_triangle_buffer = nullptr;
		m_triangle_count = m_triangle_stride = 0;
	}

	TConstMeshView2(const TConstMeshView2& copy) = default;
	TConstMeshView2& operator=(const TConstMeshView2& copy) = default;

	TConstMeshView2(TConstMeshView2&& moved) = delete;
	TConstMeshView2& operator=(TConstMeshView2&& moved) = delete;

	TConstMeshView2(
		const uint8_t* VertexBuffer, int VertexCount,
		const uint8_t* TriangleBuffer, int TriangleCount,
		int VertexStride = 2*sizeof(RealType), int TriangleStride = 3*sizeof(int))
	{
		m_vertex_buffer = VertexBuffer;
		m_vertex_count = VertexCount;
		m_vertex_stride = VertexStride;
		m_triangle_buffer = TriangleBuffer;
		m_triangle_count = TriangleCount;
		m_triangle_stride = TriangleStride;
	}



	int GetNumVertexIDs() const {
		return m_vertex_count;
	}
	inline bool IsValidVertexID(int VertexID) const {
		return VertexID >= 0 && VertexID < m_vertex_count;
	}
	Vector2<RealType> GetVertex(int VertexID) const {
		const RealType* v = (const RealType*)(m_vertex_buffer + VertexID*m_vertex_stride);
		return Vector2<RealType>(v);
	}


	inline int GetNumTriangleIDs() const {
		return m_triangle_count;
	}
	inline bool IsValidTriangleID(int TriangleID) const {
		return TriangleID >= 0 && TriangleID < m_triangle_count;
	}
	inline Index3i GetTriangle(int TriangleID) const {
		const int* t = (const int*)(m_triangle_buffer + TriangleID*m_triangle_stride);
		return Index3i(t);
	}

	void GetTriangle(int TriangleID, Triangle2<RealType>& TriangleOut) const {
		const int* t = (const int*)(m_triangle_buffer + TriangleID*m_triangle_stride);
		TriangleOut.A = GetVertex(t[0]);
		TriangleOut.B = GetVertex(t[1]);
		TriangleOut.C = GetVertex(t[2]);
	}

	template<typename RealType2>
	void GetTriangle(int TriangleID, Triangle2<RealType2>& TriangleOut) const {
		const int* t = (const int*)(m_triangle_buffer + TriangleID*m_triangle_stride);
		TriangleOut.A = (Vector2<RealType2>)GetVertex(t[0]);;
		TriangleOut.B = (Vector2<RealType2>)GetVertex(t[1]);
		TriangleOut.C = (Vector2<RealType2>)GetVertex(t[2]);
	}
};
typedef TConstMeshView2<float> ConstMeshView2f;
typedef TConstMeshView2<double> ConstMeshView2d;



}