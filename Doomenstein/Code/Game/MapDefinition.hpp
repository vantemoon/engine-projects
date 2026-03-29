#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include <map>
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
struct MapSpawnInfo
{
	std::string m_actor;
	std::string m_faction;
	Vec3 m_position;
	Vec3 m_orientation;
};


//-----------------------------------------------------------------------------------------------
class MapDefinition
{
public:
	std::string m_name;
	std::string m_imagePath;
	std::string m_shaderPath;
	std::string m_spriteSheetTexturePath;
	IntVec2 m_spriteSheetCellCount;
	std::vector<MapSpawnInfo> m_spawnInfos;

	static std::map<std::string, MapDefinition*> s_definitions;

public:
	static void InitializeDefinitions();
};