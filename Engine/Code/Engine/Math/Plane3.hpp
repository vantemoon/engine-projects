#pragma once
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
struct Plane3
{
public:
	Vec3 m_normal;
	float m_distanceAlongNormalFromOrigin;

public:
	Plane3( Vec3 const& normal, float distanceAlongNormalFromOrigin );
	~Plane3();
};