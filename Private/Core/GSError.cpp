// Copyright Gradientspace Corp. All Rights Reserved.
#include "Core/GSError.h"

#include <algorithm>

using namespace GS;



void GSErrorSet::AppendError(uint32_t ErrorCode, EGSErrorLevel ErrorLevel,
	const char* Message,
	GSTag Tag)
{
	GSError Error;
	Error.ErrorCode = ErrorCode;
	Error.ErrorLevel = ErrorLevel;
	if (Tag != GSTag::EmptyTag())
		Error.Tags.AddTag(Tag);
	if (Message != nullptr)
		Error.Message = std::string(Message);
	Errors.add_move(std::move(Error));
}


void GSErrorSet::AppendError(const char* Message, GSTag Tag)
{
	AppendError(0, EGSErrorLevel::Error, Message, Tag);
}
void GSErrorSet::AppendWarning(const char* Message, GSTag Tag)
{
	AppendError(0, EGSErrorLevel::Warning, Message, Tag);
}
void GSErrorSet::AppendInformation(const char* Message, GSTag Tag)
{
	AppendError(0, EGSErrorLevel::Information, Message, Tag);
}


GS::GSErrorSet& GSErrorSet::SortByLevel()
{
	std::sort(Errors.begin(), Errors.end(), [](const GSError &A, const GSError & B) { return (int)A.ErrorLevel > (int)B.ErrorLevel; } );
	return *this;
}

void GS::AppendError(
	GSErrorSet* ErrorSetOrNull,
	uint32_t ErrorCode, EGSErrorLevel ErrorLevel,
	const char* Message,
	GSTag Tag)
{
	if (ErrorSetOrNull != nullptr)
		ErrorSetOrNull->AppendError(ErrorCode, ErrorLevel, Message, Tag);
}

void GS::AppendError(
	GSErrorSet* ErrorSetOrNull,
	EGSStandardErrors ErrorType,
	EGSErrorLevel ErrorLevel,
	const char* Message,
	GSTag Tag)
{
	if (ErrorSetOrNull != nullptr)
		ErrorSetOrNull->AppendError((uint32_t)ErrorType, ErrorLevel, Message, Tag);
}


void GS::AppendError(GSErrorSet* ErrorSetOrNull, EGSStandardErrors ErrorType, const char* Message, GSTag Tag)
{
	if (ErrorSetOrNull != nullptr)
		ErrorSetOrNull->AppendError((uint32_t)ErrorType, EGSErrorLevel::Error, Message, Tag);
}
void GS::AppendWarning(GSErrorSet* ErrorSetOrNull, EGSStandardErrors ErrorType, const char* Message, GSTag Tag)
{
	if (ErrorSetOrNull != nullptr)
		ErrorSetOrNull->AppendError((uint32_t)ErrorType, EGSErrorLevel::Warning, Message, Tag);
}
void GS::AppendInformation( GSErrorSet* ErrorSetOrNull, EGSStandardErrors ErrorType, const char* Message, GSTag Tag)
{
	if (ErrorSetOrNull != nullptr)
		ErrorSetOrNull->AppendError((uint32_t)ErrorType, EGSErrorLevel::Information, Message, Tag);
}

void GS::AppendErrorUnique(GSErrorSet* ErrorSetOrNull, EGSStandardErrors ErrorType, const char* Message, GSTag Tag)
{
	if (ErrorSetOrNull != nullptr && ErrorSetOrNull->ContainsErrorOfType((uint32_t)ErrorType) == false)
		ErrorSetOrNull->AppendError((uint32_t)ErrorType, EGSErrorLevel::Error, Message, Tag);
}
void GS::AppendWarningUnique(GSErrorSet* ErrorSetOrNull, EGSStandardErrors ErrorType, const char* Message, GSTag Tag)
{
	if (ErrorSetOrNull != nullptr && ErrorSetOrNull->ContainsErrorOfType((uint32_t)ErrorType) == false)
		ErrorSetOrNull->AppendError((uint32_t)ErrorType, EGSErrorLevel::Warning, Message, Tag);
}
void GS::AppendInformationUnique(GSErrorSet* ErrorSetOrNull, EGSStandardErrors ErrorType, const char* Message, GSTag Tag)
{
	if (ErrorSetOrNull != nullptr && ErrorSetOrNull->ContainsErrorOfType((uint32_t)ErrorType) == false)
		ErrorSetOrNull->AppendError((uint32_t)ErrorType, EGSErrorLevel::Information, Message, Tag);
}
