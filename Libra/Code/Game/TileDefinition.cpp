#include "Game/TileDefinition.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Tile.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"


//-----------------------------------------------------------------------------------------------
std::vector<TileDefinition> TileDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void TileDefinition::InitializeTileDefinitions()
{
	Vec2 uvMins;
	Vec2 uvMaxs;

	// Grass 1
	const SpriteDefinition& grass1SpriteDef = g_game->m_tileSpriteSheet->GetSpriteDefinition( TILE_TYPE_GRASS_1 );
	grass1SpriteDef.GetUVs( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_GRASS_1].m_isSolid = false;
	s_definitions[TILE_TYPE_GRASS_1].m_UVs = AABB2( uvMins, uvMaxs);
	s_definitions[TILE_TYPE_GRASS_1].m_tint = GRASS_TINT;

	// Grass 3
	const SpriteDefinition& grass3SpriteDef = g_game->m_tileSpriteSheet->GetSpriteDefinition( TILE_TYPE_GRASS_3 );
	grass3SpriteDef.GetUVs( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_GRASS_3].m_isSolid = false;
	s_definitions[TILE_TYPE_GRASS_3].m_UVs = AABB2( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_GRASS_3].m_tint = GRASS_TINT;

	// Grass 4
	const SpriteDefinition& grass4SpriteDef = g_game->m_tileSpriteSheet->GetSpriteDefinition( TILE_TYPE_GRASS_4 );
	grass4SpriteDef.GetUVs( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_GRASS_4].m_isSolid = false;
	s_definitions[TILE_TYPE_GRASS_4].m_UVs = AABB2( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_GRASS_4].m_tint = GRASS_TINT;

	// Stone 2
	const SpriteDefinition& stone2SpriteDef = g_game->m_tileSpriteSheet->GetSpriteDefinition( TILE_TYPE_STONE_2 );
	stone2SpriteDef.GetUVs( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_STONE_2].m_isSolid = true;
	s_definitions[TILE_TYPE_STONE_2].m_UVs = AABB2( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_STONE_2].m_tint = LIGHT_STONE_TINT;

	// Stone Brick 4
	const SpriteDefinition& stoneBrick4SpriteDef = g_game->m_tileSpriteSheet->GetSpriteDefinition( TILE_TYPE_STONE_BRICK_4 );
	stoneBrick4SpriteDef.GetUVs( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_STONE_BRICK_4].m_isSolid = true;
	s_definitions[TILE_TYPE_STONE_BRICK_4].m_UVs = AABB2( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_STONE_BRICK_4].m_tint = LIGHT_STONE_TINT;

	// Metal Wall
	const SpriteDefinition& metalWallSpriteDef = g_game->m_tileSpriteSheet->GetSpriteDefinition( TILE_TYPE_METAL_WALL );
	metalWallSpriteDef.GetUVs( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_METAL_WALL].m_isSolid = true;
	s_definitions[TILE_TYPE_METAL_WALL].m_UVs = AABB2( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_METAL_WALL].m_tint = METAL_TINT;

	// Stone Tile
	const SpriteDefinition& stoneTileSpriteDef = g_game->m_tileSpriteSheet->GetSpriteDefinition( TILE_TYPE_STONE_TILE );
	stoneTileSpriteDef.GetUVs( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_STONE_TILE].m_isSolid = false;
	s_definitions[TILE_TYPE_STONE_TILE].m_UVs = AABB2( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_STONE_TILE].m_tint = DARK_STONE_TINT;

	// Mud 2
	const SpriteDefinition& mud2SpriteDef = g_game->m_tileSpriteSheet->GetSpriteDefinition( TILE_TYPE_MUD_2 );
	mud2SpriteDef.GetUVs( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_MUD_2].m_isSolid = false;
	s_definitions[TILE_TYPE_MUD_2].m_UVs = AABB2( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_MUD_2].m_tint = MUD_TINT;

	// Entrance
	const SpriteDefinition& entranceSpriteDef = g_game->m_tileSpriteSheet->GetSpriteDefinition( TILE_TYPE_ENTRANCE );
	entranceSpriteDef.GetUVs( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_ENTRANCE].m_isSolid = false;
	s_definitions[TILE_TYPE_ENTRANCE].m_UVs = AABB2( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_ENTRANCE].m_tint = Rgba8::WHITE;

	// Exit
	const SpriteDefinition& exitSpriteDef = g_game->m_tileSpriteSheet->GetSpriteDefinition( TILE_TYPE_EXIT );
	exitSpriteDef.GetUVs( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_EXIT].m_isSolid = false;
	s_definitions[TILE_TYPE_EXIT].m_UVs = AABB2( uvMins, uvMaxs );
	s_definitions[TILE_TYPE_EXIT].m_tint = Rgba8::WHITE;
}