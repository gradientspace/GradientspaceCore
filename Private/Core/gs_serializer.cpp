// Copyright Gradientspace Corp. All Rights Reserved.
#include "Core/gs_serializer.h"
#include "Core/gs_debug.h"

using namespace GS;


bool ISerializer::WriteBoolean(const char* key, bool bValue)
{
	uint8_t value = (bValue) ? 1 : 0;
	return WriteData(key, &value, sizeof(uint8_t));
}

bool ISerializer::ReadBoolean(const char* key, bool& bValue)
{
	uint8_t value = 0;
	bool bOK = ReadData(key, sizeof(uint8_t), &value);
	if (bOK) bValue = (value == 0) ? false : true;
	return bOK;
}

bool ISerializer::WriteVersion(const char* key, const SerializationVersion& Version)
{
	return WriteData(key, &Version.Packed, sizeof(size_t));
}

bool ISerializer::ReadVersion(const char* key, SerializationVersion& Version)
{
	return ReadData(key, sizeof(size_t), &Version.Packed);
}




void MemorySerializer::BeginWrite()
{
	data.resize(0);
	is_reading = false;
}


//GS_DISABLE_OPTIMIZATION
bool MemorySerializer::WriteData(const char* key, const void* buffer, size_t num_bytes)
{
	gs_debug_assert(is_reading == false);

	gs_debug_assert(key != nullptr && buffer != nullptr);
	if (key == nullptr || buffer == nullptr) return false;

	size_t cur_size = data.size();

	size_t key_bytes = strnlen(key, 128);
	gs_debug_assert(key_bytes < 128);		// otherwise key is truncated

	size_t total_bytes = sizeof(size_t) + key_bytes + 1 + sizeof(size_t) + num_bytes;
	data.resize(cur_size + total_bytes);

	uint8_t* cur_ptr = &data[cur_size];
	uint8_t* initial_ptr = cur_ptr;

	// write key length
	memcpy_s(cur_ptr, sizeof(size_t), &key_bytes, sizeof(size_t));
	cur_ptr += sizeof(size_t);

	// write key
	memcpy_s(cur_ptr, key_bytes, key, key_bytes);
	cur_ptr += key_bytes;

	// add null after key
	*cur_ptr = 0;
	cur_ptr++;

	// write bytes length
	memcpy_s(cur_ptr, sizeof(size_t), &num_bytes, sizeof(size_t));
	cur_ptr += sizeof(size_t);

	// write data
	memcpy_s(cur_ptr, num_bytes, buffer, num_bytes);
	cur_ptr += num_bytes;

	size_t delta = cur_ptr - initial_ptr;
	gs_debug_assert(delta == total_bytes);

	return true;
}
//GS_ENABLE_OPTIMIZATION


void MemorySerializer::BeginRead()
{
	read_index = 0;
	is_reading = true;
}

bool MemorySerializer::ReadData(const char* key, size_t num_bytes, void* buffer)
{
	gs_debug_assert(is_reading == true);

	gs_debug_assert(buffer != nullptr);
	if (buffer == nullptr) return false;

	uint8_t* cur_ptr = &data[read_index];

	// TODO: check that we are staying in size of data

	// read key length
	size_t key_len = 0;
	memcpy_s(&key_len, sizeof(size_t), cur_ptr, sizeof(size_t));
	cur_ptr += sizeof(size_t);
	read_index += sizeof(size_t);
	gs_debug_assert(key_len > 0 && key_len < 128);

	// read key
	char key_buffer[128];
	memcpy_s(key_buffer, key_len, cur_ptr, key_len);
	cur_ptr += key_len;
	read_index += key_len;

	size_t key_bytes = strnlen(key, 128);
	if (validate_keys)
	{
		gs_debug_assert(key_bytes == key_len);
		for (int k = 0; k < key_bytes; ++k)
		{
			gs_debug_assert(key_buffer[k] == key[k]);
		}
	}

	// check for null byte
	uint8_t null_byte = *cur_ptr;
	cur_ptr++;
	read_index++;
	gs_debug_assert(null_byte == 0);

	// read num bytes
	size_t bytes_len = 0;
	memcpy_s(&bytes_len, sizeof(size_t), cur_ptr, sizeof(size_t));
	cur_ptr += sizeof(size_t);
	read_index += sizeof(size_t);

	gs_debug_assert(bytes_len == num_bytes);

	// read data
	memcpy_s(buffer, num_bytes, cur_ptr, num_bytes);
	cur_ptr += num_bytes;
	read_index += num_bytes;

	// any final checks?

	return true;
}


const uint8_t* MemorySerializer::GetBuffer(size_t& NumBytesOut) const
{
	NumBytesOut = data.size();
	return (NumBytesOut > 0) ? &data[0] : nullptr;
}


void MemorySerializer::InitializeMemory(size_t NumBytes)
{
	data.resize(NumBytes);
}

void MemorySerializer::InitializeMemory(size_t NumBytes, const uint8_t* FromBuffer)
{
	data.resize(NumBytes);
	memcpy_s(&data[0], NumBytes, FromBuffer, NumBytes);
}

uint8_t* MemorySerializer::GetWritableBuffer(size_t& NumBytesOut)
{
	NumBytesOut = data.size();
	return (NumBytesOut > 0) ? &data[0] : nullptr;
}
