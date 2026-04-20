#include "Engine/Math/Easing.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
float SmoothStart2( float t )
{
	return t * t;
}


//-----------------------------------------------------------------------------------------------
float SmoothStart3( float t )
{
	return t * t * t;
}


//-----------------------------------------------------------------------------------------------
float SmoothStart4( float t )
{
	return t * t * t * t;
}


//-----------------------------------------------------------------------------------------------
float SmoothStart5( float t )
{
	return t * t * t * t * t;
}


//-----------------------------------------------------------------------------------------------
float SmoothStart6( float t )
{
	return t * t * t * t * t * t;
}


//-----------------------------------------------------------------------------------------------
float SmoothStop2( float t )
{
	float oneMinusT = 1.f - t;
	return 1.f - ( oneMinusT * oneMinusT );
}


//-----------------------------------------------------------------------------------------------
float SmoothStop3( float t )
{
	float oneMinusT = 1.f - t;
	return 1.f - ( oneMinusT * oneMinusT * oneMinusT );
}


//-----------------------------------------------------------------------------------------------
float SmoothStop4( float t )
{
	float oneMinusT = 1.f - t;
	return 1.f - ( oneMinusT * oneMinusT * oneMinusT * oneMinusT );
}


//-----------------------------------------------------------------------------------------------
float SmoothStop5( float t )
{
	float oneMinusT = 1.f - t;
	return 1.f - ( oneMinusT * oneMinusT * oneMinusT * oneMinusT * oneMinusT );
}


//-----------------------------------------------------------------------------------------------
float SmoothStop6( float t )
{
	float oneMinusT = 1.f - t;
	return 1.f - ( oneMinusT * oneMinusT * oneMinusT * oneMinusT * oneMinusT * oneMinusT );
}


//-----------------------------------------------------------------------------------------------
float SmoothStep3( float t )
{
	return 3.f * t * t - 2.f * t * t * t;
}


//-----------------------------------------------------------------------------------------------
float SmoothStep5( float t )
{
	return 10.f * t * t * t - 15.f * t * t * t * t + 6.f * t * t * t * t * t;
}


//-----------------------------------------------------------------------------------------------
float Hesitate3( float t )
{
	return 3.f * t - 6.f * t * t + 4.f * t * t * t;
}


//-----------------------------------------------------------------------------------------------
float Hesitate5( float t )
{
	return 5.f * t - 20.f * t * t + 40.f * t * t * t - 40.f * t * t * t * t + 16.f * t * t * t * t * t;
}