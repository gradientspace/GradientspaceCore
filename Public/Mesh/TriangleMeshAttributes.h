// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/unsafe_vector.h"
#include "Math/GSVector2.h"
#include "Math/GSVector3.h"
#include "Math/GSVector4.h"
#include "Math/GSTriangle2.h"
#include "Math/GSIndex3.h"
#include "Math/GSIndex4.h"
#include "Color/GSIntColor4.h"

#include <array>

namespace GS
{


struct GRADIENTSPACECORE_API MeshAttributeName
{
	std::array<char, 14> BaseName = { "\0\0\0\0\0\0\0\0\0\0\0\0\0" };
	uint8_t Index = 0;
	uint8_t Length = 0;

	MeshAttributeName(const char* UseName, uint8_t UseIndex = 0) {
		strcpy_s(&BaseName[0], 14, UseName);
		Length = (uint8_t)strnlen_s(BaseName.data(), 14);
		Index = UseIndex;
	}

	bool SameBaseName(const MeshAttributeName& OtherName) const {
		return Length == OtherName.Length && strncmp(BaseName.data(), OtherName.BaseName.data(), 14) == 0;
	}

	bool operator==(const MeshAttributeName& OtherName) const
	{
		return (Length == OtherName.Length && Index == OtherName.Index) &&
			(strncmp(BaseName.data(), OtherName.BaseName.data(), 14) == 0);
	}
	bool operator!=(const MeshAttributeName& OtherName) const
	{
		return !(*this == OtherName);
	}

	static MeshAttributeName Normal(uint8_t Index = 0);
	static MeshAttributeName UV(uint8_t Index = 0);
	static MeshAttributeName Color4f(uint8_t Index = 0);
	static MeshAttributeName Color4b(uint8_t Index = 0);
	static MeshAttributeName Material(uint8_t Index = 0);
	static MeshAttributeName TriGroup(uint8_t Index = 0);
};



struct GRADIENTSPACECORE_API MeshAttributeHeader
{
	const uint8_t Channels = 1;
	const uint8_t BytesPerChannel = 4;

	const uint8_t Unused0 = 0;
	const uint8_t Unused1 = 0;

	// can actually use another 32-bits here...
	inline int ElementSize() const { return Channels * BytesPerChannel; }

	MeshAttributeHeader() {}
	MeshAttributeHeader(uint8_t num_channels, uint8_t bytes_per_channel)
		: Channels(num_channels), BytesPerChannel(bytes_per_channel) {}

	static MeshAttributeHeader Normal3f();
	static MeshAttributeHeader UV2f();
	static MeshAttributeHeader Color4f();
	static MeshAttributeHeader Color4b();
	static MeshAttributeHeader Material();
	static MeshAttributeHeader TriGroup();
};

class GRADIENTSPACECORE_API MeshAttributeStorage
{
private:
	MeshAttributeHeader m_Header;

public:
	// todo semantic and cpp type hints? since we are probably wasting 2 bytes for alignment?
	unsafe_vector<uint8_t> Data;

	MeshAttributeStorage(const MeshAttributeHeader& HeaderIn) : m_Header(HeaderIn)
	{
	}

	const MeshAttributeHeader& Header() const { return m_Header; }

	// sizing
	inline int ElementSize() const { return m_Header.ElementSize(); }

	inline void SetNumElements(int NumElements)
	{
		Data.resize(NumElements * m_Header.ElementSize());
	}
	inline void ReserveAdd(int NumAdditionalElements)
	{
		Data.reserve( Data.size() + NumAdditionalElements* m_Header.ElementSize() );
	}
	inline int GetNumElements() const
	{
		return (int)(Data.size() / (size_t)m_Header.ElementSize());
	}
	inline void AddElement()
	{
		Data.resize(Data.size() + m_Header.ElementSize());
	}

	void* GetElementRawPointer(int Index)
	{
		return (void*)&Data[Index * m_Header.ElementSize()];
	}
	const void* GetElementRawPointer(int Index) const
	{
		return (void*)&Data[Index * m_Header.ElementSize()];
	}

	bool CheckType(int NumChannels, int BytesPerChannel) const {
		return m_Header.Channels == NumChannels && m_Header.BytesPerChannel == BytesPerChannel;
	}
};




template<typename DataType>
class MeshAttribView
{
protected:
	const MeshAttributeStorage* storage_ptr = nullptr;
	const DataType* get_data_ptr() const { return (const DataType*)storage_ptr->Data.raw_pointer(); }

public:	
	MeshAttribView() {
		storage_ptr = nullptr;
	}

	MeshAttribView(const MeshAttributeStorage* Storage) {
		storage_ptr = nullptr;
		if (Storage != nullptr) {
			gs_debug_assert(Storage->ElementSize() == sizeof(DataType));
			storage_ptr = Storage;
		}
	}

	bool IsValid() const {
		return storage_ptr != nullptr;
	}

	const DataType* GetElementPtr(int Index) const {
		return ((const DataType*)storage_ptr->Data.raw_pointer()) + (Index * storage_ptr->Header().ElementSize());
	}
};



template<typename RealType, int NumChannels>
class MeshVecAttribView : public MeshAttribView<RealType>
{
public:
	explicit MeshVecAttribView(const MeshAttributeStorage* Storage) : MeshAttribView<RealType>(Storage) {
		if (Storage)
			gs_debug_assert(Storage->CheckType(NumChannels, sizeof(RealType)));
	}
};


class GRADIENTSPACECORE_API MeshNormal3fView : public MeshVecAttribView<float, 3>
{
public:
	explicit MeshNormal3fView(const MeshAttributeStorage* Storage) : MeshVecAttribView(Storage) {}

	inline Vector3f GetNormal(int Index) const {
		return Vector3f(MeshAttribView<float>::GetElementPtr(Index));
	}
};

class GRADIENTSPACECORE_API MeshUV2fView : public MeshVecAttribView<float, 2>
{
public:
	explicit MeshUV2fView(const MeshAttributeStorage* Storage) : MeshVecAttribView(Storage) {}

	inline Vector2f GetUV(int Index) const {
		return Vector2f(MeshAttribView<float>::GetElementPtr(Index));
	}
};


class GRADIENTSPACECORE_API MeshColor4fView : public MeshVecAttribView<float, 4>
{
public:
	explicit MeshColor4fView(const MeshAttributeStorage* Storage) : MeshVecAttribView(Storage) {}

	inline Vector4f GetColor(int Index) const {
		return Vector4f(MeshAttribView<float>::GetElementPtr(Index));
	}
};

class GRADIENTSPACECORE_API MeshColor4bView : public MeshVecAttribView<uint8_t, 4>
{
public:
	explicit MeshColor4bView(const MeshAttributeStorage* Storage) : MeshVecAttribView(Storage) {}

	inline Color4b GetColor(int Index) const {
		return Color4b(MeshAttribView<uint8_t>::GetElementPtr(Index), 4);
	}
};




template<typename ScalarType>
class MeshScalarAttribView : public MeshAttribView<ScalarType>
{
public:
	explicit MeshScalarAttribView(const MeshAttributeStorage* Storage) : MeshAttribView<ScalarType>(Storage) {
		if (Storage)
			gs_debug_assert(Storage->CheckType(1, sizeof(ScalarType)));
	}

	ScalarType GetElement(int Index) const {
		return *(MeshAttribView<ScalarType>::GetElementPtr(Index));
	}
};




class GRADIENTSPACECORE_API MeshTriGroupView : public MeshScalarAttribView<int32_t>
{
public:
	explicit MeshTriGroupView(const MeshAttributeStorage* Storage) : MeshScalarAttribView<int32_t>(Storage) { }

	inline int32_t GetGroup(int TriangleID) const {
		return *(MeshAttribView<int32_t>::GetElementPtr(TriangleID));
	}
};

class GRADIENTSPACECORE_API MeshTriMaterialView : public MeshScalarAttribView<uint8_t>
{
public:
	explicit MeshTriMaterialView(const MeshAttributeStorage* Storage) : MeshScalarAttribView<uint8_t>(Storage) { }

	uint8_t GetMaterial(int TriangleID) const {
		return *(MeshAttribView<uint8_t>::GetElementPtr(TriangleID));
	}
};





template<typename DataType>
class MeshAttribEditor
{
protected:
	MeshAttributeStorage* storage_ptr = nullptr;
	DataType* get_data_ptr() const { return (DataType*)storage_ptr->Data.raw_pointer(); }

public:
	MeshAttribEditor() {
		storage_ptr = nullptr;
	}

	MeshAttribEditor(MeshAttributeStorage* Storage) {
		storage_ptr = nullptr;
		if (Storage != nullptr) {
			gs_debug_assert(Storage->Header().BytesPerChannel == sizeof(DataType));
			storage_ptr = Storage;
		}
	}

	bool IsValid() const {
		return storage_ptr != nullptr;
	}

	const DataType* GetElementPtr(int Index) const {
		int elemsize = storage_ptr->Header().ElementSize();
		return (const DataType*)(storage_ptr->Data.raw_pointer() + (Index * elemsize));
	}
	DataType* GetElementPtr(int Index) {
		int elemsize = storage_ptr->Header().ElementSize();
		return (DataType*)(storage_ptr->Data.raw_pointer() + (Index * elemsize));
	}
	void SetElement(int Index, const DataType* NewElement) {
		DataType* ptr = GetElementPtr(Index);
		int elemsize = storage_ptr->Header().ElementSize();
		memcpy_s(ptr, elemsize, NewElement, elemsize);
	}
};

template<typename RealType, int NumChannels>
class MeshVecAttribEditor : public MeshAttribEditor<RealType>
{
public:
	explicit MeshVecAttribEditor(MeshAttributeStorage* Storage) : MeshAttribEditor<RealType>(Storage) {
		if (Storage)
			gs_debug_assert(Storage->CheckType(NumChannels, sizeof(RealType)));
	}
};


class GRADIENTSPACECORE_API MeshNormal3fEditor : public MeshVecAttribEditor<float, 3>
{
public:
	explicit MeshNormal3fEditor(MeshAttributeStorage* Storage) : MeshVecAttribEditor(Storage) {}

	inline Vector3f GetNormal(int Index) {
		return Vector3f(MeshAttribEditor<float>::GetElementPtr(Index));
	}
	inline void SetNormal(int Index, Vector3f NewNormal) {
		MeshAttribEditor<float>::SetElement(Index, NewNormal.AsPointer());
	}
};

class GRADIENTSPACECORE_API MeshUV2fEditor : public MeshVecAttribEditor<float, 2>
{
public:
	explicit MeshUV2fEditor(MeshAttributeStorage* Storage) : MeshVecAttribEditor(Storage) {}

	inline Vector2f GetUV(int Index) {
		return Vector2f(MeshAttribEditor<float>::GetElementPtr(Index));
	}
	inline void SetUV(int Index, Vector2f NewUV) {
		MeshAttribEditor<float>::SetElement(Index, NewUV.AsPointer());
	}
};

class GRADIENTSPACECORE_API MeshColor4fEditor : public MeshVecAttribEditor<float, 4>
{
public:
	explicit MeshColor4fEditor(MeshAttributeStorage* Storage) : MeshVecAttribEditor(Storage) {}

	inline Vector4f GetColor(int Index) {
		return Vector4f(MeshAttribEditor<float>::GetElementPtr(Index));
	}
	inline void SetColor(int Index, Vector4f NewColor) {
		MeshAttribEditor<float>::SetElement(Index, &NewColor.X);
	}
};

class GRADIENTSPACECORE_API MeshColor4bEditor : public MeshVecAttribEditor<uint8_t, 4>
{
public:
	explicit MeshColor4bEditor(MeshAttributeStorage* Storage) : MeshVecAttribEditor(Storage) {}

	inline Color4b GetColor(int Index) {
		return Color4b(MeshAttribEditor<uint8_t>::GetElementPtr(Index), 4);
	}
	inline void SetColor(int Index, Color4b NewColor) {
		MeshAttribEditor<uint8_t>::SetElement(Index, NewColor.AsPointer());
	}
};


template<typename ScalarType>
class MeshScalarAttribEditor : public MeshAttribEditor<ScalarType>
{
public:
	explicit MeshScalarAttribEditor(MeshAttributeStorage* Storage) : MeshAttribEditor<ScalarType>(Storage) {
		if (Storage)
			gs_debug_assert(Storage->CheckType(1, sizeof(ScalarType)));
	}

	ScalarType GetElement(int Index) const {
		return *(MeshAttribEditor<ScalarType>::GetElementPtr(Index));
	}
	void SetElement(int Index, ScalarType NewValue) {
		MeshAttribEditor<ScalarType>::SetElement(Index, &NewValue);
	}
};




class GRADIENTSPACECORE_API MeshTriGroupEditor : public MeshScalarAttribEditor<int32_t>
{
public:
	explicit MeshTriGroupEditor(MeshAttributeStorage* Storage) : MeshScalarAttribEditor<int32_t>(Storage) { }

	inline int32_t GetGroup(int TriangleID) const {
		return *(MeshAttribEditor<int32_t>::GetElementPtr(TriangleID));
	}
	inline void SetMaterial(int TriangleID, int32_t NewGroup) {
		MeshAttribEditor<int32_t>::SetElement(TriangleID, &NewGroup);
	}
};

class GRADIENTSPACECORE_API MeshTriMaterialEditor : public MeshScalarAttribEditor<uint8_t>
{
public:
	explicit MeshTriMaterialEditor(MeshAttributeStorage* Storage) : MeshScalarAttribEditor<uint8_t>(Storage) { }

	uint8_t GetMaterial(int TriangleID) {
		return *(MeshAttribEditor<uint8_t>::GetElementPtr(TriangleID));
	}
	inline void SetMaterial(int TriangleID, uint8_t NewMaterial) {
		MeshAttribEditor<uint8_t>::SetElement(TriangleID, &NewMaterial);
	}
};











struct GRADIENTSPACECORE_API GenericIndexedMeshAttribute
{

};


}