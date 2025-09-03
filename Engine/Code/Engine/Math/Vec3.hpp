#pragma once


//-----------------------------------------------------------------------------------------------
struct Vec3
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

public:
	// Construction/Destruction
	~Vec3() {}															// destructor (do nothing)
	Vec3() {}															// default constructor (do nothing)
	Vec3(Vec3 const& copyFrom);							                // copy constructor (from another vec3)
	explicit Vec3(float initialX, float initialY, float initialZ);		// explicit constructor (from x, y, z)
};