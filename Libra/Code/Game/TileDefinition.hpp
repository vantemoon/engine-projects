#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include <vector>
#include <map>
#include <string>


//-----------------------------------------------------------------------------------------------
struct TileDefinition
{
public:
	std::string		m_name = "";
	bool			m_isSolid = false;
	bool			m_isWater = false;
	bool			m_isDestructible = false;
	std::string		m_altTileType = "";
	AABB2			m_UVs = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
	Rgba8			m_tint = Rgba8( 255, 255, 255 );
	Rgba8			m_mapImageColor = Rgba8( 0, 0, 0, 0 );
	int				m_maxHealth = 999;

	static std::map<std::string, TileDefinition*> s_definitions;

public:
	static void InitializeTileDefinitions();
};