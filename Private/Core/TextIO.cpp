// Copyright Gradientspace Corp. All Rights Reserved.
#include "Core/TextIO.h"

using namespace GS;

StreamTextWriter::StreamTextWriter(std::ostream& stream)
	: out_stream(stream)
{
}

void StreamTextWriter::WriteLine(const char* LineString)
{
	out_stream << LineString << std::endl;
}
void StreamTextWriter::WriteToken(const char* TokenString)
{
	out_stream << TokenString;
}
void StreamTextWriter::WriteEndOfLine()
{
	out_stream << std::endl;
}



FileTextWriter::~FileTextWriter()
{
	CloseFile();
}

FileTextWriter FileTextWriter::OpenFile(const std::string& FilePath)
{
	FileTextWriter Result;
	Result.out_stream.open(FilePath, std::ofstream::out | std::ofstream::trunc);
	return Result;
}

FileTextWriter::FileTextWriter()
{
}

bool FileTextWriter::operator!() const
{
	return !out_stream;
}

bool FileTextWriter::IsOpen() const
{
	return !!out_stream;
}

void FileTextWriter::CloseFile()
{
	out_stream.close();
}

void FileTextWriter::WriteLine(const char* LineString)
{
	out_stream << LineString << std::endl;
}
void FileTextWriter::WriteToken(const char* TokenString)
{
	out_stream << TokenString;
}
void FileTextWriter::WriteEndOfLine()
{
	out_stream << std::endl;
}
