// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/GSTag.h"
#include "Core/SharedPointer.h"
#include "Core/dynamic_buffer.h"
#include "Core/unsafe_vector.h"

#include <string>

namespace GS
{

enum class EGSErrorLevel
{
	Information = 0,
	Warning = 1,
	Error = 2,
	FatalError = 3
};


struct GRADIENTSPACECORE_API GSError
{
	uint32_t ErrorCode = 0;
	EGSErrorLevel ErrorLevel = EGSErrorLevel::Error;

	std::string Message;

	GSTagSet Tags;
	SharedPtr<dynamic_buffer<uint8_t>> CustomData;
};



/**
 * WARNING: do not ever assume integer values of these errors will be stable!!
 */
enum class EGSStandardErrors : uint32_t
{

	InvalidTopology_MultipleGroupBoundaries,
	InvalidTopology_DegenerateFace

};




class GRADIENTSPACECORE_API GSErrorSet
{
public:
	unsafe_vector<GSError> Errors;		// any reason to not use std::vector here??

	void AppendError(GSError&& Error)
	{
		Errors.add_move(std::move(Error));
	}

	void AppendError(uint32_t ErrorCode, EGSErrorLevel ErrorLevel,
		const char* Message = nullptr,
		GSTag Tag = GSTag::EmptyTag());

	void AppendError(const char* Message, GSTag Tag = GSTag::EmptyTag());
	void AppendWarning(const char* Message, GSTag Tag = GSTag::EmptyTag());
	void AppendInformation(const char* Message, GSTag Tag = GSTag::EmptyTag());

	bool ContainsErrorOfType(uint32_t ErrorCode) const
	{
		int N = (int)Errors.size();
		for (int j = 0; j < N; ++j)
			if (Errors[j].ErrorCode == ErrorCode)
				return true;
		return false;
	}
	bool ContainsErrorOfType(EGSStandardErrors ErrorType) const
	{
		return ContainsErrorOfType((uint32_t)ErrorType);
	}

	GS::GSErrorSet& SortByLevel();
};





GRADIENTSPACECORE_API
void AppendError(
	GSErrorSet* ErrorSetOrNull,
	uint32_t ErrorCode, EGSErrorLevel ErrorLevel,
	const char* Message = nullptr,
	GSTag Tag = GSTag::EmptyTag());

GRADIENTSPACECORE_API
void AppendError(
	GSErrorSet* ErrorSetOrNull,
	EGSStandardErrors ErrorType,
	EGSErrorLevel ErrorLevel,
	const char* Message = nullptr,
	GSTag Tag = GSTag::EmptyTag());

GRADIENTSPACECORE_API
void AppendError(
	GSErrorSet* ErrorSetOrNull,
	EGSStandardErrors ErrorType,
	const char* Message = nullptr,
	GSTag Tag = GSTag::EmptyTag());

GRADIENTSPACECORE_API
void AppendWarning(
	GSErrorSet* ErrorSetOrNull,
	EGSStandardErrors ErrorType,
	const char* Message = nullptr,
	GSTag Tag = GSTag::EmptyTag());

GRADIENTSPACECORE_API
void AppendInformation(
	GSErrorSet* ErrorSetOrNull,
	EGSStandardErrors ErrorType,
	const char* Message = nullptr,
	GSTag Tag = GSTag::EmptyTag());


GRADIENTSPACECORE_API
void AppendErrorUnique(
	GSErrorSet* ErrorSetOrNull,
	EGSStandardErrors ErrorType,
	const char* Message = nullptr,
	GSTag Tag = GSTag::EmptyTag());

GRADIENTSPACECORE_API
void AppendWarningUnique(
	GSErrorSet* ErrorSetOrNull,
	EGSStandardErrors ErrorType,
	const char* Message = nullptr,
	GSTag Tag = GSTag::EmptyTag());

GRADIENTSPACECORE_API
void AppendInformationUnique(
	GSErrorSet* ErrorSetOrNull,
	EGSStandardErrors ErrorType,
	const char* Message = nullptr,
	GSTag Tag = GSTag::EmptyTag());

}
