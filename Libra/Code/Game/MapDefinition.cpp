#include "Game/MapDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
std::vector<MapDefinition> MapDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void MapDefinition::InitializeMapDefinitions()
{
	if ( TileDefinition::s_definitions.empty() )
	{
		TileDefinition::InitializeTileDefinitions();
	}

	int numOfMaps = g_gameConfigBlackboard.GetValue( "numOfMaps", 3 );
	s_definitions.resize( numOfMaps );

	float tileSize = g_gameConfigBlackboard.GetValue( "tileSize", 12.5f );

	// Map 1
	IntVec2 exitTileCoords1 = IntVec2( 46, 22 );
	Vec2 exitPos1 = Vec2( ( exitTileCoords1.x + 0.5f ) * tileSize, ( exitTileCoords1.y + 0.5f ) * tileSize );

	s_definitions[0].m_dimensions = IntVec2( 48, 24 );
	s_definitions[0].m_aspectRatio = 0.5f;
	s_definitions[0].m_exitPosition = exitPos1;

	s_definitions[0].m_numOfLeos = 5;
	s_definitions[0].m_numOfScorpios = 5;
	s_definitions[0].m_numOfAries = 5;

	s_definitions[0].m_fillTileType = TileDefinition::s_definitions["Grass1"];
	s_definitions[0].m_borderTileType = TileDefinition::s_definitions["Stone2"];
	s_definitions[0].m_bunkerFloorTileType = TileDefinition::s_definitions["StoneTile"];
	s_definitions[0].m_startbunkerWallTileType = TileDefinition::s_definitions["MetalWall"];
	s_definitions[0].m_exitbunkerWallTileType = TileDefinition::s_definitions["StoneBrick4"];
	s_definitions[0].m_worm1TileType = TileDefinition::s_definitions["Stone2"];
	s_definitions[0].m_numOfWorms1 = 0;
	s_definitions[0].m_wormLength1 = 5;
	s_definitions[0].m_worm2TileType = TileDefinition::s_definitions["Stone2"];
	s_definitions[0].m_numOfWorms2 = 0;
	s_definitions[0].m_wormLength2 = 0;

	// Map 2
	IntVec2 exitTileCoords2 = IntVec2( 48, 18 );
	Vec2 exitPos2 = Vec2( ( exitTileCoords2.x + 0.5f ) * tileSize, ( exitTileCoords2.y + 0.5f ) * tileSize );

	s_definitions[1].m_dimensions = IntVec2( 50, 20 );
	s_definitions[1].m_aspectRatio = 2.5f;
	s_definitions[1].m_exitPosition = exitPos2;

	s_definitions[1].m_numOfLeos = 5;
	s_definitions[1].m_numOfScorpios = 5;
	s_definitions[1].m_numOfAries = 3;

	s_definitions[1].m_fillTileType = TileDefinition::s_definitions["Grass3"];
	s_definitions[1].m_borderTileType = TileDefinition::s_definitions["StoneBrick4"];
	s_definitions[1].m_bunkerFloorTileType = TileDefinition::s_definitions["StoneTile"];
	s_definitions[1].m_startbunkerWallTileType = TileDefinition::s_definitions["StoneBrick4"];
	s_definitions[1].m_exitbunkerWallTileType = TileDefinition::s_definitions["MetalWall"];
	s_definitions[1].m_worm1TileType = TileDefinition::s_definitions["Stone2"];
	s_definitions[1].m_numOfWorms1 = 0;
	s_definitions[1].m_wormLength1 = 15;
	s_definitions[1].m_worm2TileType = TileDefinition::s_definitions["Stone2"];
	s_definitions[1].m_numOfWorms2 = 0;
	s_definitions[1].m_wormLength2 = 10;

	// Map 3
	IntVec2 exitTileCoords3 = IntVec2( 30, 30 );
	Vec2 exitPos3 = Vec2( ( exitTileCoords3.x + 0.5f ) * tileSize, ( exitTileCoords3.y + 0.5f ) * tileSize );

	s_definitions[2].m_dimensions = IntVec2( 32, 32 );
	s_definitions[2].m_aspectRatio = 1.f;
	s_definitions[2].m_exitPosition = exitPos3;

	s_definitions[2].m_numOfLeos = 5;
	s_definitions[2].m_numOfScorpios = 5;
	s_definitions[2].m_numOfAries = 10;

	s_definitions[2].m_fillTileType = TileDefinition::s_definitions["Mud2"];
	s_definitions[2].m_borderTileType = TileDefinition::s_definitions["MetalWall"];
	s_definitions[2].m_bunkerFloorTileType = TileDefinition::s_definitions["StoneTile"];
	s_definitions[2].m_startbunkerWallTileType = TileDefinition::s_definitions["MetalWall"];
	s_definitions[2].m_exitbunkerWallTileType = TileDefinition::s_definitions["MetalWall"];
	s_definitions[2].m_worm1TileType = TileDefinition::s_definitions["MetalWall"];
	s_definitions[2].m_numOfWorms1 = 0;
	s_definitions[2].m_wormLength1 = 8;
	s_definitions[2].m_worm2TileType = TileDefinition::s_definitions["Grass4"];
	s_definitions[2].m_numOfWorms2 = 0;
	s_definitions[2].m_wormLength2 = 8;
}