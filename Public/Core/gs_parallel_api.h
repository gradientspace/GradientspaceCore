// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include"GradientspacePlatform.h"
#include "Core/FunctionRef.h"
#include "Core/UniquePointer.h"

#include <functional>

namespace GS
{

struct GRADIENTSPACECORE_API ParallelForFlags
{
	bool bForceSingleThread = false;
	bool bUnbalanced = false;

	// if context key is the default value, then Parallel::GetDefaultContextKey() will be used
	uint32_t ContextKey = 0xFFFFFFFF;
};

struct GRADIENTSPACECORE_API TaskFlags
{
	// if context key is the default value, then Parallel::GetDefaultContextKey() will be used
	uint32_t ContextKey = 0xFFFFFFFF;
};


class GRADIENTSPACECORE_API IExternalTaskWrapper
{
public:
	IExternalTaskWrapper() {}

	// make this type noncopyable
	IExternalTaskWrapper(IExternalTaskWrapper&&) = delete; 
	IExternalTaskWrapper(const IExternalTaskWrapper&) = delete; 
	IExternalTaskWrapper& operator=(const IExternalTaskWrapper&) = delete; 
	IExternalTaskWrapper& operator=(IExternalTaskWrapper&&) = delete;
};

struct GRADIENTSPACECORE_API TaskContainer
{
	std::shared_ptr<IExternalTaskWrapper> ExternalTask;
	uint32_t ContextKey = 0xFFFFFFFF;
};

namespace Parallel
{
	/** 
	 * Gradientspace library hosts should implement this API to provide parallel functions.
	 *
	 * The implementation(s) should be registered using GS::Parallel::RegisterAPI.
	 * 
	 * Library code will use the wrapper functions in headers like ParallelFor.h to 
	 * interact with the registered implementation(s).
	 */
	class GRADIENTSPACECORE_API gs_parallel_api
	{
	public:
		virtual ~gs_parallel_api() {}

		virtual void parallel_for_jobcount(
			uint32_t NumJobs,
			FunctionRef<void(uint32_t JobIndex)> JobFunction,
			ParallelForFlags Flags
		) = 0;

		virtual TaskContainer launch_task(
			const char* Identifier,
			std::function<void()> task,
			TaskFlags Flags
		) = 0;

		virtual void wait_for_task(
			TaskContainer& task
		) = 0;
	};




	// currently always 0
	GRADIENTSPACECORE_API
	uint32_t GetDefaultContextKey();

	GRADIENTSPACECORE_API
	void RegisterAPI(
		UniquePtr<gs_parallel_api>&& Implementation,
		uint32_t ContextKey = GetDefaultContextKey());

	GRADIENTSPACECORE_API
	bool UseParallelAPI(
		FunctionRef<void(gs_parallel_api&)> WorkFunc,
		uint32_t ContextKey = GetDefaultContextKey());

};





}

