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

constexpr float TILE_SIZE = 12.5f;
constexpr int   NUM_TILES_VISIBLE_VERTICALLY = 8;
constexpr int   NUM_TILES_VISIBLE_HORIZONTALLY = 16;
constexpr float INNER_STONE_TILE_PROBABILITY = 0.1f;
const     Rgba8 GRASS_COLOR = Rgba8( 0, 100, 0 );
const     Rgba8 STONE_COLOR = Rgba8( 128, 128, 128 );

constexpr float PLAYER_TANK_PHYSICS_RADIUS = 4.f;
constexpr float PLAYER_TANK_COSMETIC_RADIUS = 6.f;
constexpr float PLAYER_TANK_MOVE_SPEED_TILES_PER_SECOND = 10.f;
constexpr float PLAYER_TANK_TURN_SPEED_DEGREES_PER_SECOND = 180.f;
constexpr float PLAYER_TANK_TURRET_TURN_SPEED_DEGREES_PER_SECOND = 360.f;
constexpr float PLAYER_TANK_FIRE_COOLDOWN_SECONDS = 0.1f;
constexpr int   PLAYER_TANK_HEALTH = 10;

constexpr float SCORPIO_PHYSICS_RADIUS = 4.f;
constexpr float SCORPIO_COSMETIC_RADIUS = 6.f;
constexpr float SCORPIO_TURN_SPEED_DEGREES_PER_SECOND = 60.f;
constexpr float SCORPIO_FIRE_COOLDOWN_SECONDS = 0.3f;
constexpr int   SCORPIO_HEALTH = 10;
constexpr float SCORPIO_MAX_RANGE = 100.f;

constexpr float LEO_PHYSICS_RADIUS = 4.f;
constexpr float LEO_COSMETIC_RADIUS = 6.f;
constexpr float LEO_MOVE_SPEED_TILES_PER_SECOND = 6.f;
constexpr float LEO_TURN_SPEED_DEGREES_PER_SECOND = 60.f;
constexpr float LEO_FIRE_COOLDOWN_SECONDS = 1.2f;
constexpr float LEO_TURN_COOLDOWN_SECONDS = 2.f;
constexpr int   LEO_HEALTH = 10;

constexpr float BULLET_WIDTH = 1.f;
constexpr float BULLET_LENGTH = 2.f;
constexpr float BULLET_PHYSICS_RADIUS = 0.5f;
constexpr float BULLET_COSMETIC_RADIUS = 1.f;
constexpr float BOLT_WIDTH = 1.5f;
constexpr float BOLT_LENGTH = 3.f;
constexpr float BOLT_PHYSICS_RADIUS = 1.f;
constexpr float BOLT_COSMETIC_RADIUS = 3.f;
constexpr float GOOD_BOLT_SPEED_TILES_PER_SECOND = 80.f;
constexpr int   GOOD_BOLT_HEALTH = 3;
constexpr float EVIL_BULLET_SPEED_TILES_PER_SECOND = 60.f;
constexpr int   EVIL_BULLET_HEALTH = 1;
constexpr float EVIL_BOLT_SPEED_TILES_PER_SECOND = 60.f;
constexpr int   EVIL_BOLT_HEALTH = 1;

void DebugDrawRing( Vec2 center, float radius, float thickness, const Rgba8& color );
void DebugDrawLine( Vec2 start, Vec2 end, float thickness, const Rgba8& startColor, const Rgba8& endColor );