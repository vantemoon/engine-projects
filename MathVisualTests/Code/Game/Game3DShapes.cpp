#include "Game/Game3DShapes.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Game3DShapes::Game3DShapes()
{
}


//-----------------------------------------------------------------------------------------------
Game3DShapes::~Game3DShapes()
{
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	
	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	Render();
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::Render() const
{
	
}