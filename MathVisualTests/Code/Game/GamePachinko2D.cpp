#include "Game/GamePachinko2D.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
GamePachinko2D::GamePachinko2D()
{
	LoadGameConfigFromFile( "Data/GameConfig.xml" );
	GenerateRandomBumpers();
}


//-----------------------------------------------------------------------------------------------
GamePachinko2D::~GamePachinko2D()
{
}


//-----------------------------------------------------------------------------------------------
void GamePachinko2D::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( PACHINKO_WORLD_SIZE_X, PACHINKO_WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	UpdateFromKeyboard();
	Render();
}


//-----------------------------------------------------------------------------------------------
void GamePachinko2D::UpdateFromKeyboard()
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
		GenerateRandomBumpers();
	}
}


//-----------------------------------------------------------------------------------------------
void GamePachinko2D::Render() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	std::vector<Vertex> verts;

	// Bumpers
	for ( int bumperIndex = 0; bumperIndex < m_capsuleBumpers.size(); ++bumperIndex )
	{
		TestShapeCapsule* capsuleBumper = m_capsuleBumpers[bumperIndex];
		Rgba8 bumperColor = GetBumperColorFromElasticity( capsuleBumper->m_elasticity );
		AddVertsForCapsule2D( verts, capsuleBumper->m_boneStart, capsuleBumper->m_boneEnd, capsuleBumper->m_radius, bumperColor, 32 );
	}
	for ( int bumperIndex = 0; bumperIndex < m_discBumpers.size(); ++bumperIndex )
	{
		TestShapeDisc* discBumper = m_discBumpers[bumperIndex];
		Rgba8 bumperColor = GetBumperColorFromElasticity( discBumper->m_elasticity );
		AddVertsForDisc2D( verts, discBumper->m_center, discBumper->m_radius, bumperColor, 32 );
	}
	for ( int bumperIndex = 0; bumperIndex < m_obbBumpers.size(); ++bumperIndex )
	{
		TestShapeOBB* obbBumper = m_obbBumpers[bumperIndex];
		Rgba8 bumperColor = GetBumperColorFromElasticity( obbBumper->m_elasticity );
		AddVertsForOBB2D( verts, obbBumper->m_orientedBox, bumperColor );
	}
	
	// Ray
	AddVertsForArrow2D(verts, m_rayStartPos, m_rayEndPos, 3.f, 10.f, Rgba8::GREEN );

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
void GamePachinko2D::LoadGameConfigFromFile( char const* filepath )
{
	tinyxml2::XMLDocument xmlDocument;
	XmlResult loadResult = xmlDocument.LoadFile( filepath );

	if ( loadResult != tinyxml2::XML_SUCCESS )
	{
		GUARANTEE_OR_DIE( false, Stringf( "Failed to load game config XML file '%s'", filepath ) );
		return;
	}

	XmlElement* rootElement = xmlDocument.RootElement();
	if ( rootElement == nullptr )
	{
		GUARANTEE_OR_DIE( false, Stringf( "Failed to find root element in game config XML file '%s'", filepath ) );
		return;
	}

	g_gameConfigBlackboard.PopulateFromXmlElementAttributes( *rootElement );
}


//-----------------------------------------------------------------------------------------------
Vec2 GamePachinko2D::GetMouseWorldPos() const
{
	Vec2 mouseUV = g_engine->m_window->GetNormalizedMouseUV();
	Vec2 cameraBottomLeft = m_worldCamera->GetOrthoBottomLeft();
	Vec2 cameraTopRight = m_worldCamera->GetOrthoTopRight();
	AABB2 cameraBounds = AABB2( cameraBottomLeft, cameraTopRight );
	Vec2 cursorWorldPos = cameraBounds.GetPointAtUV( mouseUV );
	return cursorWorldPos;
}


//-----------------------------------------------------------------------------------------------
void GamePachinko2D::GenerateRandomBumpers()
{
	RandomNumberGenerator rng;
	float pachinkoMinBumperElasticity = g_gameConfigBlackboard.GetValue( "pachinkoMinBumperElasticity", 0.01f );
	float pachinkoMaxBumperElasticity = g_gameConfigBlackboard.GetValue( "pachinkoMaxBumperElasticity", 0.99f );

	int pachinkoNumCapsuleBumpers = g_gameConfigBlackboard.GetValue( "pachinkoNumCapsuleBumpers", 10 );
	float pachinkoMinCapsuleBumperLength = ( float ) g_gameConfigBlackboard.GetValue( "pachinkoMinCapsuleBumperLength", 1 );
	float pachinkoMaxCapsuleBumperLength = ( float ) g_gameConfigBlackboard.GetValue( "pachinkoMaxCapsuleBumperLength", 150 );
	float pachinkoMinCapsuleBumperRadius = ( float ) g_gameConfigBlackboard.GetValue( "pachinkoMinCapsuleBumperRadius", 5 );
	float pachinkoMaxCapsuleBumperRadius = ( float ) g_gameConfigBlackboard.GetValue( "pachinkoMaxCapsuleBumperRadius", 50 );
	for ( int bumperIndex = 0; bumperIndex < pachinkoNumCapsuleBumpers; ++bumperIndex )
	{
		Vec2 start = Vec2( 
			rng.RollRandomFloatInRange( PACHINKO_WORLD_SIZE_X * 0.1f, PACHINKO_WORLD_SIZE_X * 0.9f ), 
			rng.RollRandomFloatInRange( PACHINKO_WORLD_SIZE_Y * 0.1f, PACHINKO_WORLD_SIZE_Y * 0.9f ) );
		float length = rng.RollRandomFloatInRange( pachinkoMinCapsuleBumperLength, pachinkoMaxCapsuleBumperLength );
		Vec2 direction = Vec2::MakeFromPolarDegrees( rng.RollRandomFloatInRange( 0.f, 360.f ) );
		Vec2 end = start + direction * length;
		float radius = rng.RollRandomFloatInRange( pachinkoMinCapsuleBumperRadius, pachinkoMaxCapsuleBumperRadius );
		float elasticity = rng.RollRandomFloatInRange( pachinkoMinBumperElasticity, pachinkoMaxBumperElasticity );

		TestShapeCapsule* capsuleBumper = new TestShapeCapsule( start, end, radius, 32, elasticity );
		m_capsuleBumpers.push_back( capsuleBumper );
	}

	int pachinkoNumDiscBumpers = g_gameConfigBlackboard.GetValue( "pachinkoNumDiscBumpers", 10 );
	float pachinkoMinDiscBumperRadius = ( float ) g_gameConfigBlackboard.GetValue( "pachinkoMinDiscBumperRadius", 5 );
	float pachinkoMaxDiscBumperRadius = ( float ) g_gameConfigBlackboard.GetValue( "pachinkoMaxDiscBumperRadius", 50 );
	for ( int bumperIndex = 0; bumperIndex < pachinkoNumDiscBumpers; ++bumperIndex )
	{
		Vec2 center = Vec2( 
			rng.RollRandomFloatInRange( PACHINKO_WORLD_SIZE_X * 0.1f, PACHINKO_WORLD_SIZE_X * 0.9f ),
			rng.RollRandomFloatInRange( PACHINKO_WORLD_SIZE_Y * 0.1f, PACHINKO_WORLD_SIZE_Y * 0.9f ) );
		float radius = rng.RollRandomFloatInRange( pachinkoMinDiscBumperRadius, pachinkoMaxDiscBumperRadius );
		float elasticity = rng.RollRandomFloatInRange( pachinkoMinBumperElasticity, pachinkoMaxBumperElasticity );

		TestShapeDisc* discBumper = new TestShapeDisc( center, radius, 32, elasticity );
		m_discBumpers.push_back( discBumper );
	}

	int pachinkoNumObbBumpers = g_gameConfigBlackboard.GetValue( "pachinkoNumObbBumpers", 10 );
	float pachinkoMinObbBumperWidth = ( float ) g_gameConfigBlackboard.GetValue( "pachinkoMinObbBumperWidth", 5 );
	float pachinkoMaxObbBumperWidth = ( float ) g_gameConfigBlackboard.GetValue( "pachinkoMaxObbBumperWidth", 80 );
	for ( int bumperIndex = 0; bumperIndex < pachinkoNumObbBumpers; ++bumperIndex )
	{
		Vec2 center = Vec2( 
			rng.RollRandomFloatInRange( PACHINKO_WORLD_SIZE_X * 0.1f, PACHINKO_WORLD_SIZE_X * 0.9f ), 
			rng.RollRandomFloatInRange( PACHINKO_WORLD_SIZE_Y * 0.1f, PACHINKO_WORLD_SIZE_Y * 0.9f ) );
		Vec2 halfDimensions = Vec2( 
			rng.RollRandomFloatInRange( pachinkoMinObbBumperWidth, pachinkoMaxObbBumperWidth ), 
			rng.RollRandomFloatInRange( pachinkoMinObbBumperWidth, pachinkoMaxObbBumperWidth ) );
		Vec2 iBasisNormal = Vec2::MakeFromPolarDegrees( rng.RollRandomFloatInRange( 0.f, 360.f ) );
		float elasticity = rng.RollRandomFloatInRange( pachinkoMinBumperElasticity, pachinkoMaxBumperElasticity );

		TestShapeOBB* obbBumper = new TestShapeOBB( center, iBasisNormal, halfDimensions, elasticity );
		m_obbBumpers.push_back( obbBumper );
	}
}


//-----------------------------------------------------------------------------------------------
Rgba8 GamePachinko2D::GetBumperColorFromElasticity( float elasticity ) const
{
	float pachinkoMinBumperElasticity = g_gameConfigBlackboard.GetValue( "pachinkoMinBumperElasticity", 0.01f );
	float pachinkoMaxBumperElasticity = g_gameConfigBlackboard.GetValue( "pachinkoMaxBumperElasticity", 0.99f );
	float normalizedElasticity = RangeMapClamped( elasticity, pachinkoMinBumperElasticity, pachinkoMaxBumperElasticity, 0.f, 1.f );

	Rgba8 color = Rgba8::WHITE;
	color.r = ( unsigned char ) GetClamped( ( 1.f - normalizedElasticity ) * 255.f, 0.f, 255.f );
	color.g = ( unsigned char ) GetClamped( normalizedElasticity * 255.f, 0.f, 255.f );
	color.b = 0;
	return color;
}