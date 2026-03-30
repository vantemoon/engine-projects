#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>
#include <map>


//-----------------------------------------------------------------------------------------------
class TileDefinition
{
public:
	std::string m_name;
	bool m_isSolid;
	Rgba8 m_mapImagePixelColor;
	IntVec2 m_floorSpriteCoords = IntVec2::ZERO;
	IntVec2 m_wallSpriteCoords = IntVec2::ZERO;
	IntVec2 m_ceilingSpriteCoords = IntVec2::ZERO;

	static std::map<std::string, TileDefinition*> s_definitions;

public:
	static void InitializeDefinitions();
	static void ClearDefinitions();
	static TileDefinition const* GetTileDefinitionFromColor( Rgba8 const& color );
};