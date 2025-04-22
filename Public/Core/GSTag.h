// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"



namespace GS
{

struct GRADIENTSPACECORE_API GSTag
{
	uint32_t Type = 0;
	uint32_t Value = 0;

	bool operator==(const GSTag& OtherTag) const
	{
		return Type == OtherTag.Type && Value == OtherTag.Value;
	}
	static GSTag EmptyTag() { return GSTag(); }
};


struct GRADIENTSPACECORE_API GSTagSet
{
	union {
		struct {
			GSTag A;
			GSTag B;
			GSTag C;
			GSTag D;
		};
		GSTag Tags[4] = { GSTag(), GSTag(), GSTag(), GSTag() };
	};

	int NumTags = 0;

	bool AddTag(GSTag Tag, bool bUniqueType = false)
	{
		if (NumTags >= 4) return false;
		if (bUniqueType && ContainsTagOfType(Tag)) return false;
		Tags[NumTags] = Tag;
		NumTags++;
		return true;
	}


	bool ContainsTagOfType(GSTag Tag) const
	{
		for (int j = 0; j < NumTags; ++j)
			if (Tags[j].Type == Tag.Type) 
				return true;
		return false;
	}

	const GSTag* FindTagOfType(uint32_t Type) const
	{
		for (int j = 0; j < NumTags; ++j)
			if (Tags[j].Type == Type)
				return &Tags[j];
		return nullptr;
	}


};





}
