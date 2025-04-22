// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include <vector>

namespace GS
{


// TODO replace bools w/ some kind of struct that can contain a message...


struct SerializationVersion
{
	union {
		struct {
			uint32_t Version : 32;
			uint32_t Flags : 32;
		};
		size_t Packed;
	};

	SerializationVersion()
	{
		Packed = 0;
	}

	SerializationVersion(uint32_t VersionNumber)
	{
		Version = VersionNumber;
		Flags = 0;
	}
};


class GRADIENTSPACECORE_API ISerializer
{
public:
	virtual ~ISerializer() {}

public:
	virtual size_t GetNumBytesWritten() const = 0;

	virtual bool WriteData(const char* key, const void* buffer, size_t num_bytes) = 0;
	virtual bool ReadData(const char* key, size_t num_bytes, void* buffer) = 0;

	virtual bool WriteBoolean(const char* key, bool bValue);
	virtual bool ReadBoolean(const char* key, bool& bValue);

	virtual bool WriteVersion(const char* key, const SerializationVersion& Version);
	virtual bool ReadVersion(const char* key, SerializationVersion& Version);

	template<typename ValueType>
	bool WriteValue(const char* key, const ValueType& Value)
	{
		return WriteData(key, &Value, sizeof(ValueType));
	}

	template<typename ValueType>
	bool ReadValue(const char* key, ValueType& Value)
	{
		return ReadData(key, sizeof(ValueType), &Value);
	}
};




class GRADIENTSPACECORE_API MemorySerializer : public ISerializer
{
public:
	// ISerializer interface
	virtual size_t GetNumBytesWritten() const override { return (is_reading) ? 0 : data.size(); }

	virtual bool WriteData(const char* key, const void* buffer, size_t num_bytes) override;
	virtual bool ReadData(const char* Key, size_t num_bytes, void* buffer) override;

	void BeginWrite();
	void BeginRead();

	size_t NumBytes() const { return data.size(); }
	const uint8_t* GetBuffer(size_t& NumBytesOut) const;

	void InitializeMemory(size_t NumBytes);
	void InitializeMemory(size_t NumBytes, const uint8_t* FromBuffer);
	//  todo InitializeMemory that takes ownership of buffer...
	uint8_t* GetWritableBuffer(size_t& NumBytesOut);

	// todo some way to take ownership of buffer memory? can't really do with std::vector...

protected:
	// use dynamic_buffer or unsafe_vector here?
	std::vector<uint8_t> data;

	bool validate_keys = true;
	bool is_reading = false;

	size_t read_index;
};



}



