#pragma once


//-----------------------------------------------------------------------------------------------
struct Vec4
{
	float x;
	float y;
	float z;
	float w;

	Vec4();
	Vec4( float initialX, float initialY, float initialZ, float initialW );

	Vec4 const operator+( Vec4 const& vecToAdd ) const;
	bool operator==( Vec4 const& vecToCompare ) const;
};