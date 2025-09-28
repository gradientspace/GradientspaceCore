// Copyright Gradientspace Corp. All Rights Reserved.
#include "Core/BinaryIO.h"

using namespace GS;



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
