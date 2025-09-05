#include <stdlib.h>
#include "Engine/Math/RandomNumberGenerator.hpp"


//-----------------------------------------------------------------------------------------------
int RandomNumberGenerator::RollRandomIntLessThan( int maxNotInclusive )
{
	int randomInt = rand() % maxNotInclusive;
	return randomInt;
}


//-----------------------------------------------------------------------------------------------
int RandomNumberGenerator::RollRandomIntInRange( int minInclusive, int maxInclusive )
{
	int range = maxInclusive - minInclusive + 1;
	int randomInt = RollRandomIntLessThan( range ) + minInclusive;
	return randomInt;
}


//-----------------------------------------------------------------------------------------------
float RandomNumberGenerator::RollRandomFloatZeroToOne()
{
	float randomFloat = rand() / (float)RAND_MAX;
	return randomFloat;
}


//-----------------------------------------------------------------------------------------------
float RandomNumberGenerator::RollRandomFloatInRange( float minInclusive, float maxInclusive )
{
	float range = maxInclusive - minInclusive;
	float randomFloat = RollRandomFloatZeroToOne() * range + minInclusive;
	return randomFloat;
}