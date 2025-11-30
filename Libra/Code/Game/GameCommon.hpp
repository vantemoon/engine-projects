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
void DebugDrawRing( Vec2 center, float radius, float thickness, const Rgba8& color );
void DebugDrawLine( Vec2 start, Vec2 end, float thickness, const Rgba8& startColor, const Rgba8& endColor );