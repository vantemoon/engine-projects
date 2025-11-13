#include "Game/GameRaycastVsDiscs.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/RaycastUtils.cpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
GameRaycastVsDiscs::GameRaycastVsDiscs()
{
	GenerateRandomDiscs();
	m_closestImpactResult = RaycastResult2D();
	m_closestImpactResult.m_impactDist = FLT_MAX;
}


//-----------------------------------------------------------------------------------------------
GameRaycastVsDiscs::~GameRaycastVsDiscs()
{
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if ( m_isSlowMo )
	{
		deltaSeconds *= 0.1f;
	}

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	RaycastVsDiscs();
	UpdateFromKeyboard();
	Render();
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::UpdateFromKeyboard()
{
	float moveSpeed = 0.5f;
	if ( m_isSlowMo )
	{
		moveSpeed *= 0.1f;
	}

	// Move ray start and end positions with WASD and IJKL
	if ( g_engine->m_inputSystem->IsKeyDown( 'W' ) )
	{
		m_rayStartPos.y += moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'S' ) )
	{
		m_rayStartPos.y -= moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'A' ) )
	{
		m_rayStartPos.x -= moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'D' ) )
	{
		m_rayStartPos.x += moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'I' ) )
	{
		m_rayEndPos.y += moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'K' ) )
	{
		m_rayEndPos.y -= moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'J' ) )
	{
		m_rayEndPos.x -= moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'L' ) )
	{
		m_rayEndPos.x += moveSpeed;
	}

	// Use arrow keys to move both ray start and end positions together
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_UPARROW ) )
	{
		m_rayStartPos.y += moveSpeed;
		m_rayEndPos.y += moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_DOWNARROW ) )
	{
		m_rayStartPos.y -= moveSpeed;
		m_rayEndPos.y -= moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_LEFTARROW ) )
	{
		m_rayStartPos.x -= moveSpeed;
		m_rayEndPos.x -= moveSpeed;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_RIGHTARROW ) )
	{
		m_rayStartPos.x += moveSpeed;
		m_rayEndPos.x += moveSpeed;
	}

	// Use mouse to set ray start position with left mouse button
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_LBUTTON ) )
	{
		m_rayStartPos = GetMouseWorldPos();
	}

	// Use mouse to set ray end position with right mouse button
	if ( g_engine->m_inputSystem->IsKeyDown( KEYCODE_RBUTTON ) )
	{
		m_rayEndPos = GetMouseWorldPos();
	}

	// Hold down T to enable slow motion
	if ( g_engine->m_inputSystem->IsKeyDown( 'T' ) )
	{
		m_isSlowMo = true;
	}
	else
	{
		m_isSlowMo = false;
	}

	// Randomize all shape positions with F8
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F8 ) )
	{
		GenerateRandomDiscs();
	}
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::Render() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	Rgba8 darkBlue = Rgba8( 50, 80, 150, 255 );
	Rgba8 lightBlue = Rgba8( 100, 150, 255, 255 );

	std::vector<Vertex> verts;

	for ( int discIndex = 0; discIndex < MAX_DISCS; ++discIndex )
	{
		if ( discIndex == m_closestImpactDiscIndex )
		{
			continue;
		}
		TestShapeDisc* disc = m_testDiscs[discIndex];
		Rgba8 discColor = darkBlue;
		AddVertsForDisc2D( verts, disc->m_center, disc->m_radius, discColor, disc->m_numSides );
	}

	if ( m_closestImpactDiscIndex != -1 )
	{
		TestShapeDisc* closestDisc = m_testDiscs[m_closestImpactDiscIndex];
		Rgba8 highlightColor = lightBlue;
		AddVertsForDisc2D( verts, closestDisc->m_center, closestDisc->m_radius, highlightColor, closestDisc->m_numSides );
	}
	
	if ( m_closestImpactDiscIndex != -1 )
	{
		AddVertsForArrow2D( verts, m_rayStartPos, m_rayEndPos, 0.3f, 1.f, Rgba8( 190, 190, 190 ) );
		AddVertsForArrow2D( verts, m_rayStartPos, m_closestImpactResult.m_impactPos, 0.3f, 1.f, Rgba8::RED );
		AddVertsForArrow2D( verts, m_closestImpactResult.m_impactPos, m_closestImpactResult.m_impactPos + m_closestImpactResult.m_impactNormal * 10.f, 0.3f, 1.f, Rgba8::YELLOW );
		AddVertsForDisc2D( verts, m_closestImpactResult.m_impactPos, 0.5f, Rgba8::WHITE, 16 );
	}
	else
	{
		AddVertsForArrow2D( verts, m_rayStartPos, m_rayEndPos, 0.3f, 1.f, Rgba8::GREEN );
	}

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::GenerateRandomDiscs()
{
	RandomNumberGenerator rng;

	for ( int discIndex = 0; discIndex < MAX_DISCS; ++discIndex )
	{
		Vec2 discCenter = Vec2( rng.RollRandomFloatInRange( 15.f, 185.f ), rng.RollRandomFloatInRange( 15.f, 85.f ) );
		float discRadius = rng.RollRandomFloatInRange( 5.f, 15.f );

		TestShapeDisc* disc = new TestShapeDisc( discCenter, discRadius, 32 );
		m_testDiscs[discIndex] = disc;
	}
}


//-----------------------------------------------------------------------------------------------
void GameRaycastVsDiscs::RaycastVsDiscs()
{
	m_closestImpactDiscIndex = -1;
	m_closestImpactResult.m_impactDist = FLT_MAX;

	for ( int discIndex = 0; discIndex < MAX_DISCS; ++discIndex )
	{
		TestShapeDisc* disc = m_testDiscs[discIndex];
		Vec2 rayDirection = ( m_rayEndPos - m_rayStartPos ).GetNormalized();
		float rayLength = ( m_rayEndPos - m_rayStartPos ).GetLength();
		RaycastResult2D result = RaycastVsDisc2D( m_rayStartPos, rayDirection, rayLength, disc->m_center, disc->m_radius );
		if ( result.m_didImpact )
		{
			if ( result.m_impactDist < m_closestImpactResult.m_impactDist )
			{
				m_closestImpactDiscIndex = discIndex;
				m_closestImpactResult = result;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
Vec2 GameRaycastVsDiscs::GetMouseWorldPos() const
{
	Vec2 mouseUV = g_engine->m_window->GetNormalizedMouseUV();
	Vec2 cameraBottomLeft = m_worldCamera->GetOrthoBottomLeft();
	Vec2 cameraTopRight = m_worldCamera->GetOrthoTopRight();
	AABB2 cameraBounds = AABB2( cameraBottomLeft, cameraTopRight );
	Vec2 cursorWorldPos = cameraBounds.GetPointAtUV( mouseUV );
	return cursorWorldPos;
}