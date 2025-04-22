// Copyright Gradientspace Corp. All Rights Reserved.
#include "Core/gs_parallel_api.h"

#include <vector>

using namespace GS;


namespace GSLocal
{
class GS_Parallel_Registry
{
public:
	struct RegisteredAPI
	{
		uint32_t ContextKey;
		UniquePtr<GS::Parallel::gs_parallel_api> Implementation;
	};

	// could be a map or a small array + map for large keys (should not be very many keys, ideally just one)
	inline static std::vector<RegisteredAPI> RegisteredAPIs;

	inline static size_t NumRegisteredAPIs = 0;

	static void register_api(uint32_t ContextKey, UniquePtr<GS::Parallel::gs_parallel_api>&& Implementation)
	{
		for (size_t k = 0; k < NumRegisteredAPIs; ++k)
		{
			if (RegisteredAPIs[k].ContextKey == ContextKey)
			{
				RegisteredAPIs[k].Implementation = std::move(Implementation);
				return;
			}
		}

		RegisteredAPI NewAPI;
		NewAPI.ContextKey = ContextKey;
		NewAPI.Implementation = std::move(Implementation);
		RegisteredAPIs.push_back(std::move(NewAPI));

		NumRegisteredAPIs = RegisteredAPIs.size();
	}

	static bool use_api(uint32_t ContextKey,
		FunctionRef<void(GS::Parallel::gs_parallel_api&)> WorkFunc)
	{
		for (size_t k = 0; k < NumRegisteredAPIs; ++k)
		{
			if (RegisteredAPIs[k].ContextKey == ContextKey)
			{
				WorkFunc(*RegisteredAPIs[k].Implementation);
				return true;
			}
		}
		return false;
	}

};


}


uint32_t GS::Parallel::GetDefaultContextKey()
{
	// TODO: allow clients to configure this value? eg a global push/pop API...
	return 0;
}



void GS::Parallel::RegisterAPI(
	UniquePtr<gs_parallel_api>&& Implementation,
	uint32_t ContextKey)
{
	GSLocal::GS_Parallel_Registry::register_api(ContextKey, std::move(Implementation));
}

bool GS::Parallel::UseParallelAPI(
	FunctionRef<void(gs_parallel_api&)> WorkFunc,
	uint32_t ContextKey)
{
	uint32_t UseContextKey = (ContextKey == 0xFFFFFFFF) ? GetDefaultContextKey() : ContextKey;

	return GSLocal::GS_Parallel_Registry::use_api(UseContextKey, WorkFunc);
}
