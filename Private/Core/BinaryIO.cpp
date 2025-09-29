// Copyright Gradientspace Corp. All Rights Reserved.
#include "Core/BinaryIO.h"

using namespace GS;



StreamBinaryReader::StreamBinaryReader(std::istream& stream)
	: in_stream(stream)
{
}

bool StreamBinaryReader::IsEndOfFile() const
{
	return in_stream.eof();
}

void StreamBinaryReader::SetPosition(size_t offset)
{
	in_stream.clear(); // clear eof flag
	in_stream.seekg(offset, std::ios::beg);
}

bool StreamBinaryReader::ReadBytes(void* ToBuffer, int ByteCount)
{
	in_stream.read((char*)ToBuffer, ByteCount);
	return in_stream.good();
}





FileBinaryReader::~FileBinaryReader()
{
	CloseFile();
}

FileBinaryReader FileBinaryReader::OpenFile(const std::string& FilePath)
{
	FileBinaryReader Result;
	Result.in_stream.open(FilePath, std::ios::binary);
	return Result;
}

FileBinaryReader::FileBinaryReader()
{
}

bool FileBinaryReader::operator!() const
{
	return !in_stream;
}

bool FileBinaryReader::IsOpen() const
{
	return !!in_stream;
}

void FileBinaryReader::CloseFile()
{
	in_stream.close();
}

bool FileBinaryReader::IsEndOfFile() const
{
	return in_stream.eof();
}

void FileBinaryReader::SetPosition(size_t offset)
{
	in_stream.clear(); // clear eof flag
	in_stream.seekg(offset, std::ios::beg);
}

bool FileBinaryReader::ReadBytes(void* ToBuffer, int ByteCount)
{
	in_stream.read((char*)ToBuffer, ByteCount);
	return in_stream.good();
}






StreamBinaryWriter::StreamBinaryWriter(std::ostream& stream)
	: out_stream(stream)
{
}

bool StreamBinaryWriter::WriteBytes(const void* WriteBuffer, int ByteCount)
{
	out_stream.write((const char*)WriteBuffer, ByteCount);
	return (out_stream.good());
}




FileBinaryWriter::~FileBinaryWriter()
{
	CloseFile();
}

FileBinaryWriter FileBinaryWriter::OpenFile(const std::string& FilePath)
{
	FileBinaryWriter Result;
	Result.out_stream.open(FilePath, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
	return Result;
}

FileBinaryWriter::FileBinaryWriter()
{
}

bool FileBinaryWriter::operator!() const
{
	return !out_stream;
}

bool FileBinaryWriter::IsOpen() const
{
	return !!out_stream;
}

void FileBinaryWriter::CloseFile()
{
	out_stream.close();
}

bool FileBinaryWriter::WriteBytes(const void* WriteBuffer, int ByteCount)
{
	out_stream.write((const char*)WriteBuffer, ByteCount);
	return (out_stream.good());
}
