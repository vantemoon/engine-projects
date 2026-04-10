#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Image.hpp"
#include <map>
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
class Shader;
class Texture;


//-----------------------------------------------------------------------------------------------
struct SpawnInfo
{
	std::string m_actor = "";
	// std::string m_faction;
	Vec3 m_position = Vec3::ZERO;
	Vec3 m_orientation = Vec3::ZERO;
	Vec3 m_velocity = Vec3::ZERO;
};


//-----------------------------------------------------------------------------------------------
class MapDefinition
{
public:
	std::string m_name;
	Image m_image;
	Shader* m_shader = nullptr;
	Texture* m_spriteSheetTexture = nullptr;
	IntVec2 m_spriteSheetCellCount;
	std::vector<SpawnInfo> m_spawnInfos;

	static std::map<std::string, MapDefinition*> s_definitions;

public:
	static void InitializeDefinitions();
	static void ClearDefinitions();
	static MapDefinition const* GetMapDefinitionByName( std::string const& name );
	static MapDefinition const* GetMapDefinitionByIndex( int index );
};