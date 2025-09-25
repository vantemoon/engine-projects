#pragma once


//-----------------------------------------------------------------------------------------------
struct Rgba8; // Forward declaration
struct Vec2;  // Forward declaration


//-----------------------------------------------------------------------------------------------
constexpr int NUM_PLAYER_LIVES = 4;
constexpr int MAX_ASTEROIDS = 12;
constexpr int MAX_BULLETS = 20;
constexpr int MAX_BEETLES = 2;
constexpr int MAX_WASPS = 3;
constexpr int MAX_DEBRIS = 300;

constexpr float WORLD_SIZE_X = 200.f;
constexpr float WORLD_SIZE_Y = 100.f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;

constexpr float ASTEROID_SPEED = 10.f;
constexpr float ASTEROID_PHYSICS_RADIUS = 1.6f;
constexpr float ASTEROID_COSMETIC_RADIUS = 2.0f;

constexpr float BULLET_LIFETIME_SECONDS = 2.0f;
constexpr float BULLET_SPEED = 50.f;
constexpr float BULLET_PHYSICS_RADIUS = 0.5f;
constexpr float BULLET_COSMETIC_RADIUS = 2.0f;

constexpr float PLAYER_SHIP_ACCELERATION = 30.f;
constexpr float PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float PLAYER_SHIP_PHYSICS_RADIUS = 1.75f;
constexpr float PLAYER_SHIP_COSMETIC_RADIUS = 2.25f;

constexpr float BEETLE_SPEED = 10.f;
constexpr float BEETLE_PHYSICS_RADIUS = 1.5f;
constexpr float BEETLE_COSMETIC_RADIUS = 2.f;

constexpr float WASP_SPEED = 10.f;
constexpr float WASP_MAX_SPEED = 500.f;
constexpr float WASP_ACCELERATION = 10.f;
constexpr float WASP_PHYSICS_RADIUS = 1.25f;
constexpr float WASP_COSMETIC_RADIUS = 1.5f;

constexpr float DEBRIS_LIFETIME_SECONDS = 2.f;

void DebugDrawRing( Vec2 center, float radius, float thickness, const Rgba8& color );
void DebugDrawLine( Vec2 start, Vec2 end, float thickness, const Rgba8& startColor, const Rgba8& endColor );