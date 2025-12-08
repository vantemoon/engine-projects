#include "Game/TileDefinition.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Tile.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"


//-----------------------------------------------------------------------------------------------
std::map<std::string, TileDefinition*> TileDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void TileDefinition::InitializeTileDefinitions()
{
	XmlDocument tileDefDoc;
	tileDefDoc.LoadFile( "Data/Definitions/TileDefinitions.xml" );

	tinyxml2::XMLElement* rootElement = tileDefDoc.RootElement();
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
		const char* isWaterText = tileDefElement->Attribute( "isWater" );
		if ( isWaterText != nullptr )
		{
			newTileDef.m_isWater = ParseXmlAttribute( *tileDefElement, "isWater", false );
		}
		const char* isDestructibleText = tileDefElement->Attribute( "isDestructible" );
		if ( isDestructibleText != nullptr )
		{
			newTileDef.m_isDestructible = ParseXmlAttribute( *tileDefElement, "isDestructible", false );
		}
		const char* altTileTypeText = tileDefElement->Attribute( "altTileType" );
		if ( altTileTypeText != nullptr )
		{
			newTileDef.m_altTileType = altTileTypeText;
		}
		const char* spriteCoordText = tileDefElement->Attribute( "spriteCoords" );
		if ( spriteCoordText != nullptr )
		{
			IntVec2 spriteCoords = ParseXmlAttribute( *tileDefElement, "spriteCoords", IntVec2::ZERO );
			int spriteIndex = spriteCoords.x * 8 + spriteCoords.y;
			SpriteDefinition const& spriteDef = g_game->m_tileSpriteSheet->GetSpriteDefinition( spriteIndex );
			newTileDef.m_UVs = spriteDef.GetUVs();
		}
		const char* tintText = tileDefElement->Attribute( "tint" );
		if ( tintText != nullptr )
		{
			newTileDef.m_tint.SetFromText( tintText );
		}
		const char* healthText = tileDefElement->Attribute( "maxHealth" );
		if ( healthText != nullptr )
		{
			newTileDef.m_maxHealth = ParseXmlAttribute( *tileDefElement, "maxHealth", 0 );
		}
		s_definitions[newTileDef.m_name] = new TileDefinition( newTileDef );
		tileDefElement = tileDefElement->NextSiblingElement( "TileDefinition" );
	}
}