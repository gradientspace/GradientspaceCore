// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/gs_serializer.h"
#include "Core/buffer_view.h"

namespace GS::SerializeUtils
{

struct StoreBufferRLECompressed
{
	static constexpr const char* SerializeVersionString() { return "store_buffer_rle_Version"; }
	
	constexpr static uint32_t Version1 = 1;
	constexpr static uint32_t CurrentVersion = Version1;
};

// ElementType must support operator==
template<typename ElementType>
static bool store_buffer_rle_compressed(const_buffer_view<ElementType> buffer, GS::ISerializer& Serializer, const char* custom_key = nullptr);

// BufferType must support resize() and operator[]
template<typename ElementType, typename BufferType>
static bool restore_buffer_rle_compressed(BufferType& buffer, GS::ISerializer& Serializer, const char* custom_key = nullptr);



// definitions

template<typename ElementType>
bool GS::SerializeUtils::store_buffer_rle_compressed(const_buffer_view<ElementType> buffer, GS::ISerializer& Serializer, const char* custom_key)
{
	GS::SerializationVersion CurrentVersion(StoreBufferRLECompressed::CurrentVersion);
	bool bOK = Serializer.WriteVersion(StoreBufferRLECompressed::SerializeVersionString(), CurrentVersion);

	size_t size = buffer.size();
	size_t stored_size = sizeof(ElementType) * size;

	// rle compression is stored as separate count & value lists
	unsafe_vector<uint16_t> Counts;
	unsafe_vector<ElementType> Items;
	size_t cur_index = 0;
	size_t accum_stored_size = 0;

	bool bAborted = false;
	if (size == 0) {
		bAborted = true;
		goto empty_buffer;
	}

	Counts.reserve(size / 8);
	Items.reserve(size / 8);

	Counts.add(1);
	Items.add(buffer[0]);
	accum_stored_size += sizeof(uint16_t) + sizeof(ElementType);
	for (size_t i = 1; i < size && bAborted == false; ++i)
	{
		if (buffer[i] == Items[cur_index] && Counts[cur_index] < 0xFFFF )
		{
			Counts[cur_index]++;
		}
		else
		{
			Counts.add(1);
			Items.add(buffer[i]);
			cur_index++;
			accum_stored_size += sizeof(uint16_t) + sizeof(ElementType);
			if (accum_stored_size >= stored_size) 
				bAborted = true;		// RLE encoding is larger, don't bother
		}
	}

empty_buffer:
	if (bAborted)
	{
		bOK = bOK && Serializer.WriteValue("Encoding", (uint32_t)0);
		bOK = bOK && Serializer.WriteValue("Length", size);
		if (size > 0) {
			const char* use_key = (custom_key != nullptr) ? custom_key : "Data";
			bOK = bOK && Serializer.WriteData(use_key, (const void*)&buffer[0], sizeof(ElementType)*size);
		}
	}
	else
	{
		bOK = bOK && Serializer.WriteValue("Encoding", (uint32_t)1);
		bOK = bOK && Serializer.WriteValue("Length", size);
		bOK = bOK && Serializer.WriteValue("NumItems", Counts.size());
		bOK = bOK && Serializer.WriteData("ItemCounts", Counts.raw_pointer(), sizeof(uint16_t)*Counts.size());
		const char* use_key = (custom_key != nullptr) ? custom_key : "Items";
		bOK = bOK && Serializer.WriteData(use_key, Items.raw_pointer(), sizeof(ElementType)*Counts.size());
	}
	return bOK;
}




template<typename ElementType, typename BufferType>
bool GS::SerializeUtils::restore_buffer_rle_compressed(BufferType& buffer, GS::ISerializer& Serializer, const char* custom_key)
{
	GS::SerializationVersion Version(0);
	bool bOK = Serializer.ReadVersion(StoreBufferRLECompressed::SerializeVersionString(), Version);
	gs_debug_assert(Version.Version == StoreBufferRLECompressed::CurrentVersion);		// todo handle other versions

	uint32_t Encoding = 0;
	bOK = bOK && Serializer.ReadValue("Encoding", Encoding);
	size_t Length = 0;
	bOK = bOK && Serializer.ReadValue("Length", Length);
	if (Encoding == 0)
	{
		buffer.resize(Length);
		if ( Length > 0 ) {
			const char* use_key = (custom_key != nullptr) ? custom_key : "Data";
			bOK = bOK && Serializer.ReadData(use_key, sizeof(ElementType)*Length, (void*)&buffer[0]);
		}
	}
	else
	{
		size_t NumItems = 0;
		bOK = bOK && Serializer.ReadValue("NumItems", NumItems);
		unsafe_vector<uint16_t> Counts;
		Counts.resize(NumItems);
		bOK = bOK && Serializer.ReadData("ItemCounts", sizeof(uint16_t)*NumItems, (void*)&Counts[0]);
		unsafe_vector<ElementType> Items;
		Items.resize(NumItems);
		const char* use_key = (custom_key != nullptr) ? custom_key : "Items";
		bOK = bOK && Serializer.ReadData(use_key, sizeof(ElementType)*NumItems, (void*)&Items[0]);

		buffer.resize(Length);
		size_t index = 0;
		for (size_t i = 0; i < NumItems; ++i)
		{
			ElementType elem = Items[(int)i];
			uint16_t count = Counts[(int)i];
			for (int j = 0; j < count; ++j)
				buffer[index++] = elem;
		}
	}
	return bOK;
}




} // end namespace GS
