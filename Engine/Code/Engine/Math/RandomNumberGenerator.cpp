#include "Engine/Math/RandomNumberGenerator.hpp"
#include <stdlib.h>
#include <time.h>


//-----------------------------------------------------------------------------------------------
RandomNumberGenerator::RandomNumberGenerator( int seed )
{
	if ( seed == 0 )
		seed = ( unsigned int ) time( nullptr );

	m_seed = seed;
	srand( seed );
}


//-----------------------------------------------------------------------------------------------
void RandomNumberGenerator::SetSeed( int seed )
{
	m_seed = seed;
	srand( seed );
}

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
	float randomFloat = rand() / (float) RAND_MAX;
	return randomFloat;
}


//-----------------------------------------------------------------------------------------------
float RandomNumberGenerator::RollRandomFloatInRange( float minInclusive, float maxInclusive )
{
	float range = maxInclusive - minInclusive;
	float randomFloat = RollRandomFloatZeroToOne() * range + minInclusive;
	return randomFloat;
}