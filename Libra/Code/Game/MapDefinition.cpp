#include "Game/MapDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/XmlUtils.hpp"


//-----------------------------------------------------------------------------------------------
std::map<std::string, MapDefinition*> MapDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void MapDefinition::InitializeMapDefinitions()
{
	if ( TileDefinition::s_definitions.empty() )
	{
		TileDefinition::InitializeTileDefinitions();
	}

	float tileSize = g_gameConfigBlackboard.GetValue( "tileSize", 12.5f );

	XmlDocument mapDefDoc;
	mapDefDoc.LoadFile( "Data/Definitions/MapDefinitions.xml" );

	tinyxml2::XMLElement* rootElement = mapDefDoc.RootElement();
	tinyxml2::XMLElement* mapDefElement = rootElement->FirstChildElement( "MapDefinition" );

	while ( mapDefElement != nullptr )
	{
		MapDefinition newMapDef;
		const char* nameText = mapDefElement->Attribute( "name" );
		if ( nameText != nullptr )
		{
			newMapDef.m_name = nameText;
		}
		const char* dimensionsText = mapDefElement->Attribute( "dimensions" );
		if ( dimensionsText != nullptr )
		{
			newMapDef.m_dimensions = ParseXmlAttribute( *mapDefElement, "dimensions", IntVec2(10, 10) );
			newMapDef.m_exitPosition = Vec2( ( newMapDef.m_dimensions.x - 2 ) * tileSize + tileSize * 0.5f,
											 ( newMapDef.m_dimensions.y - 2 ) * tileSize + tileSize * 0.5f );
		}
		const char* aspectRatioText = mapDefElement->Attribute( "aspectRatio" );
		if ( aspectRatioText != nullptr )
		{
			newMapDef.m_aspectRatio = ParseXmlAttribute( *mapDefElement, "aspectRatio", 1.f );
		}
		const char* numOfLeosText = mapDefElement->Attribute( "numOfLeos" );
		if ( numOfLeosText != nullptr )
		{
			newMapDef.m_numOfLeos = ParseXmlAttribute( *mapDefElement, "numOfLeos", 0 );
		}
		const char* numOfScorpiosText = mapDefElement->Attribute( "numOfScorpios" );
		if ( numOfScorpiosText != nullptr )
		{
			newMapDef.m_numOfScorpios = ParseXmlAttribute( *mapDefElement, "numOfScorpios", 0 );
		}
		const char* numOfAriesText = mapDefElement->Attribute( "numOfAries" );
		if ( numOfAriesText != nullptr )
		{
			newMapDef.m_numOfAries = ParseXmlAttribute( *mapDefElement, "numOfAries", 0 );
		}
		const char* fillTileTypeText = mapDefElement->Attribute( "fillTileType" );
		if ( fillTileTypeText != nullptr )
		{
			newMapDef.m_fillTileType = TileDefinition::s_definitions[fillTileTypeText];
		}
		const char* mapImageNameText = mapDefElement->Attribute( "mapImageName" );
		if ( mapImageNameText != nullptr )
		{
			newMapDef.m_mapImageName = mapImageNameText;
		}
		const char* borderTileTypeText = mapDefElement->Attribute( "borderTileType" );
		if ( borderTileTypeText != nullptr )
		{
			newMapDef.m_borderTileType = TileDefinition::s_definitions[borderTileTypeText];
		}
		const char* bunkerFloorTileTypeText = mapDefElement->Attribute( "bunkerFloorTileType" );
		if ( bunkerFloorTileTypeText != nullptr )
		{
			newMapDef.m_bunkerFloorTileType = TileDefinition::s_definitions[bunkerFloorTileTypeText];
		}
		const char* startbunkerWallTileTypeText = mapDefElement->Attribute( "startBunkerWallTileType" );
		if ( startbunkerWallTileTypeText != nullptr )
		{
			newMapDef.m_startbunkerWallTileType = TileDefinition::s_definitions[startbunkerWallTileTypeText];
		}
		const char* exitbunkerWallTileTypeText = mapDefElement->Attribute( "exitBunkerWallTileType" );
		if ( exitbunkerWallTileTypeText != nullptr )
		{
			newMapDef.m_exitbunkerWallTileType = TileDefinition::s_definitions[exitbunkerWallTileTypeText];
		}
		const char* worm1TileTypeText = mapDefElement->Attribute( "worm1TileType" );
		if ( worm1TileTypeText != nullptr )
		{
			newMapDef.m_worm1TileType = TileDefinition::s_definitions[worm1TileTypeText];
		}
		const char* worm2TileTypeText = mapDefElement->Attribute( "worm2TileType" );
		if ( worm2TileTypeText != nullptr )
		{
			newMapDef.m_worm2TileType = TileDefinition::s_definitions[worm2TileTypeText];
		}
		const char* numOfWorms1Text = mapDefElement->Attribute( "numOfWorms1" );
		if ( numOfWorms1Text != nullptr )
		{
			newMapDef.m_numOfWorms1 = ParseXmlAttribute( *mapDefElement, "numOfWorms1", 0 );
		}
		const char* numOfWorms2Text = mapDefElement->Attribute( "numOfWorms2" );
		if ( numOfWorms2Text != nullptr )
		{
			newMapDef.m_numOfWorms2 = ParseXmlAttribute( *mapDefElement, "numOfWorms2", 0 );
		}
		const char* wormLength1Text = mapDefElement->Attribute( "wormLength1" );
		if ( wormLength1Text != nullptr )
		{
			newMapDef.m_wormLength1 = ParseXmlAttribute( *mapDefElement, "wormLength1", 0 );
		}
		const char* wormLength2Text = mapDefElement->Attribute( "wormLength2" );
		if ( wormLength2Text != nullptr )
		{
			newMapDef.m_wormLength2 = ParseXmlAttribute( *mapDefElement, "wormLength2", 0 );
		}

		s_definitions[newMapDef.m_name] = new MapDefinition( newMapDef );
		mapDefElement = mapDefElement->NextSiblingElement( "MapDefinition" );
	}
}