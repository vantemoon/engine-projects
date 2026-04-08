#include "Game/MapDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/XmlUtils.hpp"


//-----------------------------------------------------------------------------------------------
std::map<std::string, MapDefinition*> MapDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void MapDefinition::InitializeDefinitions()
{
	XmlDocument mapDefDoc;
	mapDefDoc.LoadFile( "Data/Definitions/MapDefinitions.xml" );

	tinyxml2::XMLElement* rootElement = mapDefDoc.RootElement();
	if ( rootElement == nullptr )
	{
		return;
	}

	tinyxml2::XMLElement* mapDefElement = rootElement->FirstChildElement( "MapDefinition" );
	while ( mapDefElement != nullptr )
	{
		MapDefinition newMapDef;

		char const* nameText = mapDefElement->Attribute( "name" );
		if ( nameText != nullptr )
		{
			newMapDef.m_name = nameText;
		}

		char const* imageText = mapDefElement->Attribute( "image" );
		if ( imageText != nullptr )
		{
			newMapDef.m_image = Image( imageText );
		}

		char const* shaderText = mapDefElement->Attribute( "shader" );
		if ( shaderText != nullptr )
		{
			newMapDef.m_shader = g_engine->m_renderer->CreateOrGetShader( shaderText, VertexType::VERTEX_PCUTBN );
		}

		char const* spriteSheetTextureText = mapDefElement->Attribute( "spriteSheetTexture" );
		if ( spriteSheetTextureText != nullptr )
		{
			newMapDef.m_spriteSheetTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( spriteSheetTextureText );
		}

		char const* spriteSheetCellCountText = mapDefElement->Attribute( "spriteSheetCellCount" );
		if ( spriteSheetCellCountText != nullptr )
		{
			newMapDef.m_spriteSheetCellCount.SetFromText( spriteSheetCellCountText );
		}

		tinyxml2::XMLElement* spawnInfosElement = mapDefElement->FirstChildElement( "SpawnInfos" );
		if ( spawnInfosElement != nullptr )
		{
			tinyxml2::XMLElement* spawnInfoElement = spawnInfosElement->FirstChildElement( "SpawnInfo" );
			while ( spawnInfoElement != nullptr )
			{
				SpawnInfo spawnInfo;

				char const* actorText = spawnInfoElement->Attribute( "actor" );
				if ( actorText != nullptr )
				{
					spawnInfo.m_actor = actorText;
				}

				/*char const* factionText = spawnInfoElement->Attribute( "faction" );
				if ( factionText != nullptr )
				{
					spawnInfo.m_faction = factionText;
				}*/

				char const* positionText = spawnInfoElement->Attribute( "position" );
				if ( positionText != nullptr )
				{
					spawnInfo.m_position.SetFromText( positionText );
				}

				char const* orientationText = spawnInfoElement->Attribute( "orientation" );
				if ( orientationText != nullptr )
				{
					spawnInfo.m_orientation.SetFromText( orientationText );
				}

				char const* velocityText = spawnInfoElement->Attribute( "velocity" );
				if ( velocityText != nullptr )
				{
					spawnInfo.m_velocity.SetFromText( velocityText );
				}

				newMapDef.m_spawnInfos.push_back( spawnInfo );
				spawnInfoElement = spawnInfoElement->NextSiblingElement( "SpawnInfo" );
			}
		}

		s_definitions[newMapDef.m_name] = new MapDefinition( newMapDef );
		mapDefElement = mapDefElement->NextSiblingElement( "MapDefinition" );
	}
}


//-----------------------------------------------------------------------------------------------
void MapDefinition::ClearDefinitions()
{
	for ( auto& defPair : s_definitions )
	{
		delete defPair.second;
	}
	s_definitions.clear();
}


//-----------------------------------------------------------------------------------------------
MapDefinition const* MapDefinition::GetMapDefinitionByName( std::string const& name )
{
	auto defIter = s_definitions.find( name );
	if ( defIter != s_definitions.end() )
	{
		return defIter->second;
	}
	return nullptr;
}


//-----------------------------------------------------------------------------------------------
MapDefinition const* MapDefinition::GetMapDefinitionByIndex( int index )
{
	if ( index < 0 || index >= ( int ) s_definitions.size() )
	{
		return nullptr;
	}
	auto defIter = s_definitions.begin();
	std::advance( defIter, index );
	return defIter->second;
}