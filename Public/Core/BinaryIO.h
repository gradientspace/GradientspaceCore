// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"

#include <istream>
#include <ostream>
#include <fstream>

namespace GS
{

class GRADIENTSPACECORE_API IBinaryReader
{
public:
	virtual ~IBinaryReader() {}
	virtual bool IsEndOfFile() const = 0;
	virtual bool ReadBytes(void* ToBuffer, int ByteCount) = 0;
	virtual void SetPosition(size_t offset) = 0;
};


class GRADIENTSPACECORE_API StreamBinaryReader : public IBinaryReader
{
public:
	StreamBinaryReader(std::istream& stream);

	virtual bool IsEndOfFile() const override;
	virtual void SetPosition(size_t offset) override;
	virtual bool ReadBytes(void* ToBuffer, int ByteCount) override;

protected:
	std::istream& in_stream;
};



class GRADIENTSPACECORE_API FileBinaryReader : public IBinaryReader
{
public:
	~FileBinaryReader();
	FileBinaryReader& operator=(FileBinaryReader&& copy) = default;
	FileBinaryReader(FileBinaryReader&& moved) = default;

	static FileBinaryReader OpenFile(const std::string& FilePath);
	virtual bool IsEndOfFile() const override;
	virtual void SetPosition(size_t offset) override;

	bool operator!() const;
	bool IsOpen() const;
	void CloseFile();

	virtual bool ReadBytes(void* ToBuffer, int ByteCount) override;

protected:
	FileBinaryReader();		// prevent external construction, only allow opening via static functions
	std::ifstream in_stream;
};




class GRADIENTSPACECORE_API IBinaryWriter
{
public:
	virtual ~IBinaryWriter() {}
	virtual bool WriteBytes(const void* WriteBuffer, int ByteCount) = 0;
};


class GRADIENTSPACECORE_API StreamBinaryWriter : public IBinaryWriter
{
public:
	StreamBinaryWriter(std::ostream& stream);

	virtual bool WriteBytes(const void* WriteBuffer, int ByteCount) override;

protected:
	std::ostream& out_stream;
};

class GRADIENTSPACECORE_API FileBinaryWriter : public IBinaryWriter
{
public:
	~FileBinaryWriter();
	FileBinaryWriter& operator=(FileBinaryWriter&& copy) = default;
	FileBinaryWriter(FileBinaryWriter&& moved) = default;

	static FileBinaryWriter OpenFile(const std::string& FilePath);

	bool operator!() const;
	bool IsOpen() const;
	void CloseFile();

	virtual bool WriteBytes(const void* WriteBuffer, int ByteCount) override;

protected:
	FileBinaryWriter();		// prevent external construction, only allow opening via static functions
	std::ofstream out_stream;
};



}