#include "Game/TileDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"


//-----------------------------------------------------------------------------------------------
std::map<std::string, TileDefinition*> TileDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void TileDefinition::InitializeDefinitions()
{
	XmlDocument tileDefDoc;
	tileDefDoc.LoadFile( "Data/Definitions/TileDefinitions.xml" );

	tinyxml2::XMLElement* rootElement = tileDefDoc.RootElement();
	if ( rootElement == nullptr )
	{
		return;
	}

	tinyxml2::XMLElement* tileDefElement = rootElement->FirstChildElement( "TileDefinition" );
	while ( tileDefElement != nullptr )
	{
		TileDefinition newTileDef;
		const char* nameText = tileDefElement->Attribute( "name" );
		if ( nameText != nullptr )
		{
			newTileDef.m_name = nameText;
		}
		const char* isSolidText = tileDefElement->Attribute( "isSolid" );
		if ( isSolidText != nullptr )
		{
			newTileDef.m_isSolid = ParseXmlAttribute( *tileDefElement, "isSolid", false );
		}
		const char* mapImagePixelColorText = tileDefElement->Attribute( "mapImagePixelColor" );
		if ( mapImagePixelColorText != nullptr )
		{
			newTileDef.m_mapImagePixelColor.SetFromText( mapImagePixelColorText );
		}
		const char* floorSpriteCoordsText = tileDefElement->Attribute( "floorSpriteCoords" );
		if ( floorSpriteCoordsText != nullptr )
		{
			newTileDef.m_floorSpriteCoords.SetFromText( floorSpriteCoordsText );
		}
		const char* wallSpriteCoordsText = tileDefElement->Attribute( "wallSpriteCoords" );
		if ( wallSpriteCoordsText != nullptr )
		{
			newTileDef.m_wallSpriteCoords.SetFromText( wallSpriteCoordsText );
		}
		const char* ceilingSpriteCoordsText = tileDefElement->Attribute( "ceilingSpriteCoords" );
		if ( ceilingSpriteCoordsText != nullptr )
		{
			newTileDef.m_ceilingSpriteCoords.SetFromText( ceilingSpriteCoordsText );
		}

		s_definitions[newTileDef.m_name] = new TileDefinition( newTileDef );
		tileDefElement = tileDefElement->NextSiblingElement( "TileDefinition" );
	}
}


//-----------------------------------------------------------------------------------------------
TileDefinition const* GetTileDefinitionFromColor( Rgba8 const& color )
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