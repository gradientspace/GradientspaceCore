// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/gs_parallel_api.h"
#include "Core/gs_debug.h"



namespace GS
{


inline void ParallelFor(
	uint32_t NumJobs,
	FunctionRef<void(uint32_t JobIndex)> JobFunction,
	ParallelForFlags Flags = ParallelForFlags())
{
	bool bOK = GS::Parallel::UseParallelAPI([&](GS::Parallel::gs_parallel_api& ParallelAPI)
	{
		ParallelAPI.parallel_for_jobcount(NumJobs, JobFunction, Flags);

	}, Flags.ContextKey);

	gs_debug_assert(bOK);		
}



}
