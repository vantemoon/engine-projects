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
enum GameMode
{
	INVALID_MODE = -1,
	GAMEMODE_NEAREST_POINT,
	GAMEMODE_RAYCAST_VS_DISCS,
	NUM_GAME_MODES
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

void DebugDrawRing( Vec2 center, float radius, float thickness, const Rgba8& color );
void DebugDrawLine( Vec2 start, Vec2 end, float thickness, const Rgba8& startColor, const Rgba8& endColor );