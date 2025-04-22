// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include <random>

namespace GS
{


/**
 * Generates sequences of random numbers. Wrapper around std:: random functionality.
 * Internal generator currently uses 32-bit unsigned integers.
 */
struct RandomStream
{
	std::mt19937 random_generator;

	RandomStream() {
		std::random_device rd;
		random_generator = std::mt19937(rd());
		//std::uniform_real_distribution<> dis(0.0, 1.0);
	}

	RandomStream(unsigned int Seed) {
		Initialize(Seed);
	}

	RandomStream(const RandomStream& other) {
		random_generator = other.random_generator;
	}
	RandomStream& operator=(const RandomStream& other) {
		random_generator = other.random_generator;
		return *this;
	}

	void Initialize(unsigned int Seed) {
		random_generator = std::mt19937(Seed);
	}

	//! return a random unsigned integer in range [0, MaxUnsignedInt()]
	uint32_t UnsignedInt()
	{
		return random_generator();
	}

	//! return maximum unsigned int produced by generator (minimum is 0)
	static constexpr uint32_t MaxUnsignedInt()
	{
		return std::mt19937::max();
	}

	//! return a random real number in range [0,1]
	double UnitReal()
	{
		unsigned int r = random_generator();
		return (double)r / (double)random_generator.max();
	}

	//! return a random real number in range [MinValue, MaxValue]
	double RealInRange(double MinValue, double MaxValue)
	{
		double r = UnitReal();
		return MinValue + r * (MaxValue - MinValue);
	}

};




}
