#pragma once
#include "Engine/Core/Rgba8.hpp"


//-----------------------------------------------------------------------------------------------
struct Vec2;  // Forward declaration


//-----------------------------------------------------------------------------------------------
enum GameState
{
	INVALID_STATE = -1,
	ATTRACT_MODE,
	PLAYING,
	PAUSED,
	GAME_OVER,
	VICTORY,
	NUM_GAME_STATES
};


//-----------------------------------------------------------------------------------------------
constexpr float WORLD_SIZE_X = 200.f;
constexpr float WORLD_SIZE_Y = 100.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;

constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 800.f;
constexpr float SCREEN_CENTER_X = SCREEN_SIZE_X / 2.f;
constexpr float SCREEN_CENTER_Y = SCREEN_SIZE_Y / 2.f;

constexpr float TILE_SIZE = 10.f;
constexpr int   NUM_TILES_VISIBLE_VERTICALLY = 10;
constexpr int   NUM_TILES_VISIBLE_HORIZONTALLY = 20;
constexpr float INNER_STONE_TILE_PROBABILITY = 0.1f;
const     Rgba8 GRASS_COLOR = Rgba8( 0, 100, 0 );
const     Rgba8 STONE_COLOR = Rgba8( 128, 128, 128 );

constexpr float PLAYER_TANK_PHYSICS_RADIUS = 4.f;
constexpr float PLAYER_TANK_COSMETIC_RADIUS = 8.f;
constexpr float PLAYER_TANK_MAX_SPEED_TILES_PER_SECOND = 10.f;
constexpr float PLAYER_TANK_TURN_SPEED_DEGREES_PER_SECOND = 180.f;
constexpr float PLAYER_TANK_TURRET_TURN_SPEED_DEGREES_PER_SECOND = 360.f;

void DebugDrawRing( Vec2 center, float radius, float thickness, const Rgba8& color );
void DebugDrawLine( Vec2 start, Vec2 end, float thickness, const Rgba8& startColor, const Rgba8& endColor );