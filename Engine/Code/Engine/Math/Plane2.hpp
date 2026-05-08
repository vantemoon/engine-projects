#pragma once
#include "Engine/Math/Vec2.hpp"


//-----------------------------------------------------------------------------------------------
struct Plane2
{
public:
	Vec2 m_normal;
	float m_distanceAlongNormalFromOrigin;

public:
	Plane2( Vec2 const& normal, float distanceAlongNormalFromOrigin );
	~Plane2();
};