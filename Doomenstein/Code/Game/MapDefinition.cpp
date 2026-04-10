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
	XmlResult loadResult = mapDefDoc.LoadFile( "Data/Definitions/MapDefinitions.xml" );
	if ( loadResult != tinyxml2::XML_SUCCESS )
	{
		return;
	}

	tinyxml2::XMLElement* rootElement = mapDefDoc.RootElement();
	if ( rootElement == nullptr )
	{
		return;
	}

	tinyxml2::XMLElement* mapDefElement = rootElement->FirstChildElement( "MapDefinition" );
	while ( mapDefElement != nullptr )
	{
		MapDefinition newMapDef;

		newMapDef.m_name = ParseXmlAttribute( *mapDefElement, "name", newMapDef.m_name );

		std::string imagePath = ParseXmlAttribute( *mapDefElement, "image", "" );
		if ( !imagePath.empty() )
		{
			newMapDef.m_image = Image( imagePath.c_str() );
		}

		std::string shaderPath = ParseXmlAttribute( *mapDefElement, "shader", "" );
		if ( !shaderPath.empty() && g_engine != nullptr && g_engine->m_renderer != nullptr )
		{
			newMapDef.m_shader = g_engine->m_renderer->CreateOrGetShader( shaderPath.c_str(), VertexType::VERTEX_PCUTBN );
		}

		std::string spriteSheetTexturePath = ParseXmlAttribute( *mapDefElement, "spriteSheetTexture", "" );
		if ( !spriteSheetTexturePath.empty() && g_engine != nullptr && g_engine->m_renderer != nullptr )
		{
			newMapDef.m_spriteSheetTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( spriteSheetTexturePath.c_str() );
		}

		newMapDef.m_spriteSheetCellCount = ParseXmlAttribute( *mapDefElement, "spriteSheetCellCount", newMapDef.m_spriteSheetCellCount );

		tinyxml2::XMLElement* spawnInfosElement = mapDefElement->FirstChildElement( "SpawnInfos" );
		if ( spawnInfosElement != nullptr )
		{
			tinyxml2::XMLElement* spawnInfoElement = spawnInfosElement->FirstChildElement( "SpawnInfo" );
			while ( spawnInfoElement != nullptr )
			{
				SpawnInfo spawnInfo;

				spawnInfo.m_actor = ParseXmlAttribute( *spawnInfoElement, "actor", spawnInfo.m_actor );

				std::string positionText = ParseXmlAttribute( *spawnInfoElement, "position", "" );
				if ( !positionText.empty() )
				{
					spawnInfo.m_position.SetFromText( positionText.c_str() );
				}

				std::string orientationText = ParseXmlAttribute( *spawnInfoElement, "orientation", "" );
				if ( !orientationText.empty() )
				{
					spawnInfo.m_orientation.SetFromText( orientationText.c_str() );
				}

				std::string velocityText = ParseXmlAttribute( *spawnInfoElement, "velocity", "" );
				if ( !velocityText.empty() )
				{
					spawnInfo.m_velocity.SetFromText( velocityText.c_str() );
				}

				newMapDef.m_spawnInfos.push_back( spawnInfo );
				spawnInfoElement = spawnInfoElement->NextSiblingElement( "SpawnInfo" );
			}
		}

		if ( !newMapDef.m_name.empty() )
		{
			auto existingDefIter = s_definitions.find( newMapDef.m_name );
			if ( existingDefIter != s_definitions.end() )
			{
				delete existingDefIter->second;
				existingDefIter->second = nullptr;
			}
			s_definitions[newMapDef.m_name] = new MapDefinition( newMapDef );
		}

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