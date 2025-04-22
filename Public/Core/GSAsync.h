// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/gs_parallel_api.h"
#include "Core/gs_debug.h"

namespace GS::Parallel
{

inline TaskContainer StartTask(
	std::function<void()> JobFunction,
	const char* Identifier = nullptr,
	TaskFlags Flags = TaskFlags())
{
	TaskContainer task;
	bool bOK = GS::Parallel::UseParallelAPI([&](GS::Parallel::gs_parallel_api& ParallelAPI)
	{
		task = ParallelAPI.launch_task(Identifier, JobFunction, Flags);
		task.ContextKey = Flags.ContextKey;

	}, Flags.ContextKey);

	gs_debug_assert(bOK);		
	return task;
}


inline void WaitForTask(
	TaskContainer& Container)
{
	bool bOK = GS::Parallel::UseParallelAPI([&](GS::Parallel::gs_parallel_api& ParallelAPI)
	{
		ParallelAPI.wait_for_task(Container);

	}, Container.ContextKey);
}

template<typename EnumerableType>
inline void WaitForAllTasks(
	EnumerableType Tasks
)
{
	for (TaskContainer& task : Tasks)
		WaitForTask(task);
}


}
