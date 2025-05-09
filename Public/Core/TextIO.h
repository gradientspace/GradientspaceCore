// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"

#include <ostream>
#include <fstream>
#include <string>


namespace GS
{


class GRADIENTSPACECORE_API ITextWriter
{
public:
	virtual ~ITextWriter() {}
	virtual void WriteLine(const char* LineString) = 0;
	
	virtual void WriteToken(const char* TokenString) = 0;
	virtual void WriteEndOfLine() = 0;
};



class GRADIENTSPACECORE_API StreamTextWriter : public ITextWriter
{
public:
	StreamTextWriter(std::ostream& stream);

	virtual void WriteLine(const char* LineString) override;
	virtual void WriteToken(const char* TokenString) override;
	virtual void WriteEndOfLine() override;

protected:
	std::ostream& out_stream;
};


class GRADIENTSPACECORE_API FileTextWriter : public ITextWriter
{
public:
	~FileTextWriter();
	FileTextWriter& operator=(FileTextWriter&& copy) = default;
	FileTextWriter(FileTextWriter&& moved) = default;

	static FileTextWriter OpenFile(const std::string& FilePath);

	bool operator!() const;
	bool IsOpen() const;
	void CloseFile();

	virtual void WriteLine(const char* LineString) override;
	virtual void WriteToken(const char* TokenString) override;
	virtual void WriteEndOfLine() override;

protected:
	FileTextWriter();		// prevent external construction, only allow opening via static functions
	std::ofstream out_stream;
};


}
