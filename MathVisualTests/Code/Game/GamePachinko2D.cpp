#include "Game/GamePachinko2D.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <math.h>


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
	m_worldCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( PACHINKO_WORLD_SIZE_X, PACHINKO_WORLD_SIZE_Y ) );
	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	UpdateFromKeyboard();

	float physicsDeltaSeconds = deltaSeconds;
	if ( m_isSlowMo )
	{
		physicsDeltaSeconds *= 0.05f;
	}

	if ( m_usingFixedTimestep )
	{
		m_physicsTimeOwed += physicsDeltaSeconds;
		while ( m_physicsTimeOwed >= m_physicsTimestep )
		{
			UpdatePhysics( m_physicsTimestep );
			m_physicsTimeOwed -= m_physicsTimestep;
		}
	}
	else
	{
		UpdatePhysics( physicsDeltaSeconds );
	}

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

	// Use [ and ] keys to decrease/increase physics timestep
	if ( g_engine->m_inputSystem->WasKeyJustPressed( '[' ) )
	{
		m_physicsTimestep /= 1.1f;
	}
	if ( g_engine->m_inputSystem->WasKeyJustPressed( ']' ) )
	{
		m_physicsTimestep *= 1.1f;
	}

	// Randomize all shape positions with F8
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F8 ) )
	{
		Reset();
	}

	// Spawn new ball with spacebar or N key
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SPACE ) )
	{
		SpawnBall();
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'N' ) )
	{
		SpawnBall();
	}

	// Decrease ball elasticity with G key, increase with H key
	if ( g_engine->m_inputSystem->WasKeyJustPressed( 'Z' ) )
	{
		m_ballElasticity -= 0.05f;
		m_ballElasticity = GetClamped( m_ballElasticity, 0.f, 1.f );
	}
	if ( g_engine->m_inputSystem->WasKeyJustPressed( 'X' ) )
	{
		m_ballElasticity += 0.05f;
		m_ballElasticity = GetClamped( m_ballElasticity, 0.f, 1.f );
	}

	// Toggle gravity with G key
	if ( g_engine->m_inputSystem->WasKeyJustPressed( 'G' ) )
	{
		m_isGravityOn = !m_isGravityOn;
	}

	// Toggle floor with B key
	if ( g_engine->m_inputSystem->WasKeyJustPressed( 'B' ) )
	{
		m_isFloorOn = !m_isFloorOn;
	}

	// Toggle fixed timestep with P key
	if ( g_engine->m_inputSystem->WasKeyJustPressed( 'P' ) )
	{
		m_usingFixedTimestep = !m_usingFixedTimestep;
	}
}


//-----------------------------------------------------------------------------------------------
void GamePachinko2D::UpdatePhysics( float timestep )
{
	for ( int ballIndex = 0; ballIndex < m_balls.size(); ++ballIndex )
	{
		TestShapeDisc* ball = m_balls[ballIndex];
		if ( m_isGravityOn )
		{
			Vec2 gravityAcceleration = Vec2( 0.f, -m_gravityStrength );
			ball->m_velocity += gravityAcceleration * timestep;
		}
		ball->Update( timestep );
	}

	CollideBallsWithBalls();
	CollideBallsWithBumpers();
	CollideBallsWithWalls();
}


//-----------------------------------------------------------------------------------------------
void GamePachinko2D::CollideBallsWithBalls()
{
	for ( int ballIndexA = 0; ballIndexA < m_balls.size(); ++ballIndexA )
	{
		TestShapeDisc* ballA = m_balls[ballIndexA];
		for ( int ballIndexB = ballIndexA + 1; ballIndexB < m_balls.size(); ++ballIndexB )
		{
			TestShapeDisc* ballB = m_balls[ballIndexB];

			bool didOverlap = PushDiscsOutOfEachOther2D( 
				ballA->m_center, ballA->m_radius, 
				ballB->m_center, ballB->m_radius );
			if ( !didOverlap ) continue;

			Vec2 displacement = ballB->m_center - ballA->m_center;
			Vec2 collisionNormal = displacement.GetNormalized();

			Vec2 relativeVelocity = ballB->m_velocity - ballA->m_velocity;
			float dot = DotProduct2D( relativeVelocity, collisionNormal );
			if ( dot >= 0.f ) continue;

			float elasticity = ballA->m_elasticity * ballB->m_elasticity;

			Vec2 aVN = collisionNormal * DotProduct2D( ballA->m_velocity, collisionNormal );
			Vec2 bVN = collisionNormal * DotProduct2D( ballB->m_velocity, collisionNormal );

			Vec2 aVT = ballA->m_velocity - aVN;
			Vec2 bVT = ballB->m_velocity - bVN;

			aVN = bVN * elasticity;
			bVN = aVN * elasticity;

			ballA->m_velocity = aVT + aVN;
			ballB->m_velocity = bVT + bVN;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void GamePachinko2D::CollideBallsWithBumpers()
{
	for ( int ballIndex = 0; ballIndex < m_balls.size(); ++ballIndex )
	{
		TestShapeDisc* ball = m_balls[ballIndex];
		for ( int bumperIndex = 0; bumperIndex < m_capsuleBumpers.size(); ++bumperIndex )
		{
			TestShapeCapsule* capsule = m_capsuleBumpers[bumperIndex];

			float distanceSquared = GetDistanceSquared2D( ball->m_center, capsule->m_boundingDiscCenter );
			float radiusSum = ball->m_radius + capsule->m_boundingDiscRadius;
			if ( distanceSquared > radiusSum * radiusSum ) continue;

			bool didOverlap = PushDiscOutOfFixedCapsule2D(
				ball->m_center, ball->m_radius, 
				capsule->m_boneStart, capsule->m_boneEnd, capsule->m_radius );
			if ( !didOverlap ) continue;

			Vec2 nearestPointOnCapsule = GetNearestPointOnCapsule2D( ball->m_center, capsule->m_boneStart, capsule->m_boneEnd, capsule->m_radius );
			Vec2 displacement = ball->m_center - nearestPointOnCapsule;
			Vec2 collisionNormal = displacement.GetNormalized();

			float dot = DotProduct2D( ball->m_velocity, collisionNormal );
			if ( dot >= 0.f ) continue;

			float elasticity = ball->m_elasticity * capsule->m_elasticity;

			Vec2 vN = collisionNormal * DotProduct2D( ball->m_velocity, collisionNormal );
			Vec2 vT = ball->m_velocity - vN;

			vN = -vN * elasticity;
			ball->m_velocity = vT + vN;
		}
		for ( int bumperIndex = 0; bumperIndex < m_discBumpers.size(); ++bumperIndex )
		{
			TestShapeDisc* disc = m_discBumpers[bumperIndex];

			float distanceSquared = GetDistanceSquared2D( ball->m_center, disc->m_center );
			float radiusSum = ball->m_radius + disc->m_radius;
			if ( distanceSquared > radiusSum * radiusSum ) continue;
			
			bool didOverlap = PushDiscOutOfFixedDisc2D(
				ball->m_center, ball->m_radius, 
				disc->m_center, disc->m_radius );
			if ( !didOverlap ) continue;

			Vec2 displacement = ball->m_center - disc->m_center;
			Vec2 collisionNormal = displacement.GetNormalized();

			float dot = DotProduct2D( ball->m_velocity, collisionNormal );
			if ( dot >= 0.f ) continue;

			float elasticity = ball->m_elasticity * disc->m_elasticity;

			Vec2 vN = collisionNormal * DotProduct2D( ball->m_velocity, collisionNormal );
			Vec2 vT = ball->m_velocity - vN;

			vN = -vN * elasticity;
			ball->m_velocity = vT + vN;
		}
		for ( int bumperIndex = 0; bumperIndex < m_obbBumpers.size(); ++bumperIndex )
		{
			TestShapeOBB* obb = m_obbBumpers[bumperIndex];

			float distanceSquared = GetDistanceSquared2D( ball->m_center, obb->m_boundingDiscCenter );
			float radiusSum = ball->m_radius + obb->m_boundingDiscRadius;
			if ( distanceSquared > radiusSum * radiusSum ) continue;
			
			bool didOverlap = PushDiscOutOfFixedOBB2D(
				ball->m_center, ball->m_radius, 
				obb->m_orientedBox );
			if ( !didOverlap ) continue;

			Vec2 nearestPointOnOBB = GetNearestPointOnOBB2D( ball->m_center, obb->m_orientedBox );
			Vec2 displacement = ball->m_center - nearestPointOnOBB;
			Vec2 collisionNormal = displacement.GetNormalized();

			float dot = DotProduct2D( ball->m_velocity, collisionNormal );
			if ( dot >= 0.f ) continue;

			float elasticity = ball->m_elasticity * obb->m_elasticity;

			Vec2 vN = collisionNormal * DotProduct2D( ball->m_velocity, collisionNormal );
			Vec2 vT = ball->m_velocity - vN;

			vN = -vN * elasticity;
			ball->m_velocity = vT + vN;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void GamePachinko2D::CollideBallsWithWalls()
{
	float pachinkoWallElasticity = g_gameConfigBlackboard.GetValue( "pachinkoWallElasticity", 0.9f );
	for ( int ballIndex = 0; ballIndex < m_balls.size(); ++ballIndex )
	{
		TestShapeDisc* ball = m_balls[ballIndex];
		float elasticity = ball->m_elasticity * pachinkoWallElasticity;

		// Left wall
		if ( ball->m_center.x - ball->m_radius < 0.f )
		{
			ball->m_center.x = ball->m_radius;
			ball->m_velocity.x = -ball->m_velocity.x * elasticity;
		}

		// Right wall
		if ( ball->m_center.x + ball->m_radius > PACHINKO_WORLD_SIZE_X )
		{
			ball->m_center.x = PACHINKO_WORLD_SIZE_X - ball->m_radius;
			ball->m_velocity.x = -ball->m_velocity.x * elasticity;
		}

		// Floor
		if ( m_isFloorOn && ball->m_center.y - ball->m_radius < 0.f )
		{
			ball->m_center.y = ball->m_radius;
			ball->m_velocity.y = -ball->m_velocity.y * elasticity;
		}
		else if ( !m_isFloorOn && ball->m_center.y + ball->m_radius < 0.f )
		{
			ball->m_center.y = PACHINKO_WORLD_SIZE_Y * 1.1f + ball->m_radius;
		}
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

	// Balls
	for ( int ballIndex = 0; ballIndex < m_balls.size(); ++ballIndex )
	{
		TestShapeDisc* ball = m_balls[ballIndex];
		AddVertsForDisc2D( verts, ball->m_center, ball->m_radius, ball->m_color, 32 );
	}
	
	// Ray and circles
	AddVertsForArrow2D(verts, m_rayStartPos, m_rayEndPos, 3.f, 10.f, Rgba8::GREEN );
	float pachinkoMinBallRadius = ( float ) g_gameConfigBlackboard.GetValue( "pachinkoMinBallRadius", 5 );
	float pachinkoMaxBallRadius = ( float ) g_gameConfigBlackboard.GetValue( "pachinkoMaxBallRadius", 25 );
	AddVertsForRing2D( verts, m_rayStartPos, pachinkoMinBallRadius, 2.f, Rgba8( 50, 80, 150, 255 ), 32 );
	AddVertsForRing2D( verts, m_rayStartPos, pachinkoMaxBallRadius, 2.f, Rgba8( 50, 80, 150, 255 ), 32 );

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
void GamePachinko2D::Reset()
{
	for ( int bumperIndex = 0; bumperIndex < m_capsuleBumpers.size(); ++bumperIndex )
	{
		TestShapeCapsule* capsuleBumper = m_capsuleBumpers[bumperIndex];
		delete capsuleBumper;
	}
	m_capsuleBumpers.clear();

	for ( int bumperIndex = 0; bumperIndex < m_discBumpers.size(); ++bumperIndex )
	{
		TestShapeDisc* discBumper = m_discBumpers[bumperIndex];
		delete discBumper;
	}
	m_discBumpers.clear();

	for ( int bumperIndex = 0; bumperIndex < m_obbBumpers.size(); ++bumperIndex )
	{
		TestShapeOBB* obbBumper = m_obbBumpers[bumperIndex];
		delete obbBumper;
	}
	m_obbBumpers.clear();

	for ( int ballIndex = 0; ballIndex < m_balls.size(); ++ballIndex )
	{
		TestShapeDisc* ball = m_balls[ballIndex];
		delete ball;
	}
	m_balls.clear();

	GenerateRandomBumpers();
}


//-----------------------------------------------------------------------------------------------
void GamePachinko2D::SpawnBall()
{
	RandomNumberGenerator rng;
	Vec2 velocity = 3.f * ( m_rayEndPos - m_rayStartPos );
	float pachinkoMinBallRadius =  ( float ) g_gameConfigBlackboard.GetValue( "pachinkoMinBallRadius", 5 );
	float pachinkoMaxBallRadius = ( float ) g_gameConfigBlackboard.GetValue( "pachinkoMaxBallRadius", 25 );
	float radius = rng.RollRandomFloatInRange( pachinkoMinBallRadius, pachinkoMaxBallRadius );

	TestShapeDisc* ball = new TestShapeDisc( m_rayStartPos, radius, 32, m_ballElasticity, velocity );
	ball->m_color = GetRandomBallColor();
	m_balls.push_back( ball );
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


//-----------------------------------------------------------------------------------------------
Rgba8 GamePachinko2D::GetRandomBallColor() const
{
	RandomNumberGenerator rng;
	float t = rng.RollRandomFloatInRange( 0.2f, 1.f );
	Rgba8 color = Rgba8().Interpolate( Rgba8::WHITE, Rgba8( 50, 80, 150, 255 ), t );
	return color;
}