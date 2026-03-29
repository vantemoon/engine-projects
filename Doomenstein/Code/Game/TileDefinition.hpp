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
	IntVec2 m_floorSpriteCoords;
	IntVec2 m_ceilingSpriteCoords;

	static std::map<std::string, TileDefinition*> s_definitions;

public:
	static void InitializeDefinitions();
};