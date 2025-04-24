// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include <string>		// for std::hash ?

namespace GS
{
	// old boost hash_combine code, see https://stackoverflow.com/questions/4948780/magic-number-in-boosthash-combine
	// note that this is exactly the same as UE's ::HashCombineFast() function
	// see discussion here: https://stackoverflow.com/questions/35985960/c-why-is-boosthash-combine-the-best-way-to-combine-hash-values
	// maybe interesting: https://sair.synerise.com/efficient-integer-pairs-hashing/
	inline uint32_t boost_hash_combine(uint32_t A, uint32_t B)
	{
		return A ^ (B + 0x9e3779b9 + (A << 6) + (A >> 2));
	}

	// These are defined like UE defines them for now - basically just
	// re-use the bits if < 32 bits, or a simple shift+combine for 64-bit.
	// Should look into if there are better options.

	inline uint32_t get_type_hash(int8_t i)
	{
		return (uint32_t)(i);
	}
	inline uint32_t get_type_hash(uint8_t i)
	{
		return (uint32_t)(i);
	}
	inline uint32_t get_type_hash(int16_t i)
	{
		return (uint32_t)(i);
	}
	inline uint32_t get_type_hash(uint16_t i)
	{
		return (uint32_t)(i);
	}
	inline uint32_t get_type_hash(int32_t i)
	{
		return (uint32_t)(i);
	}
	inline uint32_t get_type_hash(uint32_t i)
	{
		return (uint32_t)(i);
	}
	inline uint32_t get_type_hash(int64_t i)
	{
		return ((uint32_t)i) + ((uint32_t)(i >> 32) * 23);
	}
	inline uint32_t get_type_hash(uint64_t i)
	{
		return ((uint32_t)i) + ((uint32_t)(i >> 32) * 23);
	}
	inline uint32_t get_type_hash(float f)
	{
		return *((uint32_t*)&f);
	}
	inline uint32_t get_type_hash(double d)
	{
		return get_type_hash( *((uint64_t*)&d) );
	}


	template<typename ValueType>
	uint32_t HashVector2(ValueType A, ValueType B) {
		return GS::boost_hash_combine(GS::get_type_hash(A), GS::get_type_hash(B));
	}
	template<typename ValueType>
	uint32_t HashVector3(ValueType A, ValueType B, ValueType C) {
		return GS::boost_hash_combine(GS::get_type_hash(A), 
			GS::boost_hash_combine(GS::get_type_hash(B), GS::get_type_hash(C)));
	}
	template<typename ValueType>
	uint32_t HashVector4(ValueType A, ValueType B, ValueType C, ValueType D) {
		return GS::boost_hash_combine(
			GS::boost_hash_combine(GS::get_type_hash(A), GS::get_type_hash(B)),
			GS::boost_hash_combine(GS::get_type_hash(C), GS::get_type_hash(D)));
	}
}

