#include "Engine/Math/Vec4.hpp"


//-----------------------------------------------------------------------------------------------
Vec4::Vec4()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
}


//-----------------------------------------------------------------------------------------------
Vec4::Vec4( float initialX, float initialY, float initialZ, float initialW )
{
	x = initialX;
	y = initialY;
	z = initialZ;
	w = initialW;
}