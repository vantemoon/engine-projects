#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
struct TileDefinition
{
public:
	// std::string	m_name;
	bool		m_isSolid;
	bool        m_isWater;
	AABB2		m_UVs = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
	Rgba8		m_tint = Rgba8( 255, 255, 255 );

	static std::vector<TileDefinition> s_definitions;

public:
	static void InitializeTileDefinitions();
};