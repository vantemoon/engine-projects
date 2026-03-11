#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EventSystem.hpp"
#include <string>


//-----------------------------------------------------------------------------------------------
class Camera;
struct AABB2;
struct Mat44;
struct Vec2;
struct Vec3;


//-----------------------------------------------------------------------------------------------
enum class DebugRenderMode
{
	ALWAYS,
	USE_DEPTH,
	X_RAY,
};


struct DebugRenderConfig
{
	std::string m_fontPath = "Data/Fonts/";
	std::string m_fontName = "SquirrelFixedFont";
};

// Setup
void DebugRenderSystemStartup( DebugRenderConfig const& config );
void DebugRenderSystemShutdown();

// Control
void DebugRenderSetVisible();
void DebugRenderSetHidden();
void DebugRenderClear();

// Output
void DebugRenderBeginFrame();
void DebugRenderWorld( Camera const& camera );
void DebugRenderScreen( Camera const& camera );
void DebugRenderEndFrame();

// Geometry
void DebugAddWorldWireSphere( Vec3 const& center, float radius, float duration,
	Rgba8 const& startColor = Rgba8::WHITE, Rgba8 const& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldCylinder( Vec3 const& start, Vec3 const& end, float radius, float duration,
	Rgba8 const& startColor = Rgba8::WHITE, Rgba8 const& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldArrow( Vec3 const& start, Vec3 const& end, float radius, float duration,
	Rgba8 const& startColor = Rgba8::WHITE, Rgba8 const& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldWireArrow( Vec3 const& start, Vec3 const& end, float radius, float duration,
	Rgba8 const& startColor = Rgba8::WHITE, Rgba8 const& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddBasis( Mat44 const& transform, float duration, float length, float radius, 
	float colorScale = 1.0f, float alphaScale = 1.0f,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldBasis( Mat44 const& transform, float duration,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldText( std::string const& text, Mat44 const& transform, float textHeight, 
	Vec2 const& alignment, float duration,
	Rgba8 const& startColor = Rgba8::WHITE, Rgba8 const& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldBillboardText( std::string const& text, Vec3 const& origin, float textHeight,
	Vec2 const& alignment, float duration,
	Rgba8 const& startColor = Rgba8::WHITE, Rgba8 const& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH );

void DebugAddScreenText( std::string const& text, AABB2 const& box, float cellHeight, 
	Vec2 const& alignment, float duration,
	Rgba8 const& startColor = Rgba8::WHITE, Rgba8 const& endColor = Rgba8::WHITE );
void DebugAddMessage( std::string const& text, float duration,
	Rgba8 const& startColor = Rgba8::WHITE, Rgba8 const& endColor = Rgba8::WHITE );

// Console commands
bool Command_DebugRenderClear( EventArgs& args );
bool Command_DebugRenderToggle( EventArgs& args );