#include "Game/TileDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"


//-----------------------------------------------------------------------------------------------
std::map<std::string, TileDefinition*> TileDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void TileDefinition::InitializeDefinitions()
{
	XmlDocument tileDefDoc;
	XmlResult loadResult = tileDefDoc.LoadFile( "Data/Definitions/TileDefinitions.xml" );
	if ( loadResult != tinyxml2::XML_SUCCESS )
	{
		return;
	}

	tinyxml2::XMLElement* rootElement = tileDefDoc.RootElement();
	if ( rootElement == nullptr )
	{
		return;
	}

	tinyxml2::XMLElement* tileDefElement = rootElement->FirstChildElement( "TileDefinition" );
	while ( tileDefElement != nullptr )
	{
		TileDefinition newTileDef;

		newTileDef.m_name = ParseXmlAttribute( *tileDefElement, "name", newTileDef.m_name );
		newTileDef.m_isSolid = ParseXmlAttribute( *tileDefElement, "isSolid", false );
		newTileDef.m_mapImagePixelColor = ParseXmlAttribute( *tileDefElement, "mapImagePixelColor", newTileDef.m_mapImagePixelColor );
		newTileDef.m_floorSpriteCoords = ParseXmlAttribute( *tileDefElement, "floorSpriteCoords", newTileDef.m_floorSpriteCoords );
		newTileDef.m_wallSpriteCoords = ParseXmlAttribute( *tileDefElement, "wallSpriteCoords", newTileDef.m_wallSpriteCoords );
		newTileDef.m_ceilingSpriteCoords = ParseXmlAttribute( *tileDefElement, "ceilingSpriteCoords", newTileDef.m_ceilingSpriteCoords );

		if ( !newTileDef.m_name.empty() )
		{
			s_definitions[newTileDef.m_name] = new TileDefinition( newTileDef );
		}

		tileDefElement = tileDefElement->NextSiblingElement( "TileDefinition" );
	}
}


//-----------------------------------------------------------------------------------------------
void TileDefinition::ClearDefinitions()
{
	for ( auto& defPair : s_definitions )
	{
		delete defPair.second;
	}
	s_definitions.clear();
}


//-----------------------------------------------------------------------------------------------
TileDefinition const* TileDefinition::GetTileDefinitionFromColor( Rgba8 const& color )
{
	for ( auto& defPair : TileDefinition::s_definitions )
	{
		TileDefinition const* tileDef = defPair.second;
		if ( tileDef != nullptr && tileDef->m_mapImagePixelColor == color )
		{
			return tileDef;
		}
	}
	return nullptr;
}