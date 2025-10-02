#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
struct Vertex
{
public:
	Vec3  m_position;
	Rgba8 m_color;
	Vec2  m_uvTexCoords;

public:
	// Construction/Destruction
	Vertex();																				    // default constructor (do nothing)
	explicit Vertex( Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords );		// explicit constructor (from position, color, uvTexCoords)
};