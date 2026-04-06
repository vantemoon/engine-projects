#include "Game/Game3DShapes.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Game3DShapes::Game3DShapes()
{
	GenerateRandomShapes();

	Vec3 worldCameraPos = Vec3( -2.f, 2.f, 1.f );
	EulerAngles worldCameraOrientation = EulerAngles( 0.f, 0.f, 0.f );
	m_worldCamera->SetPositionAndOrientation( worldCameraPos, worldCameraOrientation );
}


//-----------------------------------------------------------------------------------------------
Game3DShapes::~Game3DShapes()
{
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::Update( float deltaSeconds )
{
	float aspect = SCREEN_SIZE_X / SCREEN_SIZE_Y;
	m_worldCamera->SetPerspectiveView( aspect, 60.f, 0.1f, 1000.f );

	Mat44 cameraToRenderTransform(
		Vec3( 0.f, 0.f, 1.f ),
		Vec3( -1.f, 0.f, 0.f ),
		Vec3( 0.f, 1.f, 0.f ),
		Vec3::ZERO );
	m_worldCamera->SetCameraToRenderTransform( cameraToRenderTransform );

	m_screenCamera->SetOrthoView( Vec2( 0.f, 0.f ), Vec2( SCREEN_SIZE_X, SCREEN_SIZE_Y ) );

	UpdateFromKeyboard( deltaSeconds );
	Render();
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::UpdateFromKeyboard( float deltaSeconds )
{
	float moveSpeed = 40.f;
	if ( g_engine->m_inputSystem->IsKeyDown( 'T' ) )
	{
		moveSpeed *= 0.1f;
	}
	float moveDistance = moveSpeed * deltaSeconds;

	Vec3 cameraPos = m_worldCamera->GetPosition();
	EulerAngles cameraOrientation = m_worldCamera->GetOrientation();

	IntVec2 mouseDelta = g_engine->m_inputSystem->GetCursorClientDelta();
	float mouseSensitivity = 0.1f;

	cameraOrientation.m_yawDegrees += static_cast<float>( mouseDelta.x ) * mouseSensitivity;
	cameraOrientation.m_pitchDegrees -= static_cast<float>( mouseDelta.y ) * mouseSensitivity;

	cameraOrientation.m_pitchDegrees = GetClamped( cameraOrientation.m_pitchDegrees, -89.f, 89.f );

	Vec3 forward = cameraOrientation.GetForwardDir_IFwd_JLeft_KUp();
	forward.z = 0.f;
	if ( forward.GetLengthSquared() > 0.f )
	{
		forward = forward.GetNormalized();
	}

	Vec3 left;
	Vec3 up;
	cameraOrientation.GetAsVectors_IFwd_JLeft_KUp( forward, left, up );
	left.z = 0.f;
	if ( left.GetLengthSquared() > 0.f )
	{
		left = left.GetNormalized();
	}

	// Horizontal-only movement (XY only)
	if ( g_engine->m_inputSystem->IsKeyDown( 'W' ) )
	{
		cameraPos += forward * moveDistance;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'S' ) )
	{
		cameraPos -= forward * moveDistance;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'A' ) )
	{
		cameraPos += left * moveDistance;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'D' ) )
	{
		cameraPos -= left * moveDistance;
	}

	// Vertical-only movement (Z axis)
	if ( g_engine->m_inputSystem->IsKeyDown( 'Q' ) )
	{
		cameraPos.z += moveDistance;
	}
	if ( g_engine->m_inputSystem->IsKeyDown( 'E' ) )
	{
		cameraPos.z -= moveDistance;
	}

	m_worldCamera->SetPosition( cameraPos );
	m_worldCamera->SetOrientation( cameraOrientation );

	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F8 ) )
	{
		GenerateRandomShapes();
	}
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::Render() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	g_engine->m_renderer->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
	g_engine->m_renderer->SetBlendMode( BlendMode::OPAQUE );

	std::vector<Vertex> wireframeVerts;
	std::vector<Vertex> solidVerts;

	// AABBs
	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		if ( shapeIndex % 2 == 0 ) // Wireframe
		{
			TestShapeAABB3D* shape = m_testAABBs[shapeIndex];
			AddVertsForAABBWireframe3D( wireframeVerts, shape->m_alignedBox, Rgba8::WHITE );
		}
		else // Solid
		{
			TestShapeAABB3D* shape = m_testAABBs[shapeIndex];
			AddVertsForAABB3D( solidVerts, shape->m_alignedBox, Rgba8::WHITE, AABB2::ZERO_TO_ONE );
		}
	}

	// Spheres
	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		if ( shapeIndex % 2 == 0 ) // Wireframe
		{
			TestShapeSphere* shape = m_testSpheres[shapeIndex];
			AddVertsForSphereWireframe3D( wireframeVerts, shape->m_center, shape->m_radius, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 16, 8 );
		}
		else // Solid
		{
			TestShapeSphere* shape = m_testSpheres[shapeIndex];
			AddVertsForSphere3D( solidVerts, shape->m_center, shape->m_radius, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 16, 8 );
		}
	}

	// Cylinders
	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		if ( shapeIndex % 2 == 0 ) // Wireframe
		{
			TestShapeZCylinder* shape = m_testCylinders[shapeIndex];
			AddVertsForCylinderZWireframe3D( wireframeVerts, shape->m_start, shape->m_end, shape->m_radius, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 16 );
		}
		else // Solid
		{
			TestShapeZCylinder* shape = m_testCylinders[shapeIndex];
			AddVertsForCylinderZ3D( solidVerts, shape->m_start, shape->m_end, shape->m_radius, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 16 );
		}
	}

	float axisRadius = 0.03f;
	int axisSlices = 16;
	Vec3 origin = Vec3::ZERO;
	AddVertsForArrow3D( wireframeVerts, origin, Vec3( 1.f, 0.f, 0.f ), axisRadius, Rgba8::RED, axisSlices );
	AddVertsForArrow3D( wireframeVerts, origin, Vec3( 0.f, 1.f, 0.f ), axisRadius, Rgba8::GREEN, axisSlices );
	AddVertsForArrow3D( wireframeVerts, origin, Vec3( 0.f, 0.f, 1.f ), axisRadius, Rgba8::BLUE, axisSlices );

	Texture* texture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	g_engine->m_renderer->BindTexture( texture );
	g_engine->m_renderer->DrawVertexArray( solidVerts );

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( wireframeVerts );

	g_engine->m_renderer->SetBlendMode( BlendMode::ALPHA );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );
	g_engine->m_renderer->SetDepthMode( DepthMode::DISABLED );

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::GenerateRandomShapes()
{
	GenerateRandomAABBs();
	GenerateRandomSpheres();
	GenerateRandomCylinders();
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::GenerateRandomAABBs()
{
	RandomNumberGenerator rng;

	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		delete m_testAABBs[shapeIndex];
		m_testAABBs[shapeIndex] = nullptr;
	}

	float worldHalfSize = 30.f;
	float worldMinX = -worldHalfSize;
	float worldMaxX = worldHalfSize;
	float worldMinY = -worldHalfSize;
	float worldMaxY = worldHalfSize;
	float worldMinZ = -worldHalfSize;
	float worldMaxZ = worldHalfSize;

	float worldSpan = worldHalfSize * 2.f;

	float minBoxWidth = 4.f;
	float maxBoxWidth = GetClamped( 14.f, minBoxWidth, worldSpan );
	float minBoxDepth = 4.f;
	float maxBoxDepth = GetClamped( 14.f, minBoxDepth, worldSpan );
	float minBoxHeight = 3.f;
	float maxBoxHeight = GetClamped( 10.f, minBoxHeight, worldSpan );

	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		float boxWidth = rng.RollRandomFloatInRange( minBoxWidth, maxBoxWidth );
		float boxDepth = rng.RollRandomFloatInRange( minBoxDepth, maxBoxDepth );
		float boxHeight = rng.RollRandomFloatInRange( minBoxHeight, maxBoxHeight );

		float boxLeft = rng.RollRandomFloatInRange( worldMinX, worldMaxX - boxWidth );
		float boxBottom = rng.RollRandomFloatInRange( worldMinY, worldMaxY - boxDepth );
		float boxMinZ = rng.RollRandomFloatInRange( worldMinZ, worldMaxZ - boxHeight );

		AABB3 alignedBox( Vec3( boxLeft, boxBottom, boxMinZ ), Vec3( boxLeft + boxWidth, boxBottom + boxDepth, boxMinZ + boxHeight ) );
		m_testAABBs[shapeIndex] = new TestShapeAABB3D( alignedBox );
	}
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::GenerateRandomSpheres()
{
	RandomNumberGenerator rng;

	float worldHalfSize = 30.f;
	float worldMinX = -worldHalfSize;
	float worldMaxX = worldHalfSize;
	float worldMinY = -worldHalfSize;
	float worldMaxY = worldHalfSize;
	float worldMinZ = -worldHalfSize;
	float worldMaxZ = worldHalfSize;

	float worldSpan = worldHalfSize * 2.f;

	float minRadius = 1.5f;
	float maxRadius = GetClamped( 6.f, minRadius, worldSpan * 0.5f );

	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		float radius = rng.RollRandomFloatInRange( minRadius, maxRadius );

		float centerX = rng.RollRandomFloatInRange( worldMinX + radius, worldMaxX - radius );
		float centerY = rng.RollRandomFloatInRange( worldMinY + radius, worldMaxY - radius );
		float centerZ = rng.RollRandomFloatInRange( worldMinZ + radius, worldMaxZ - radius );

		TestShapeSphere* shape = new TestShapeSphere( Vec3( centerX, centerY, centerZ ), radius );
		m_testSpheres[shapeIndex] = shape;
	}
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::GenerateRandomCylinders()
{
	RandomNumberGenerator rng;
	float worldHalfSize = 30.f;
	float worldMinX = -worldHalfSize;
	float worldMaxX = worldHalfSize;
	float worldMinY = -worldHalfSize;
	float worldMaxY = worldHalfSize;
	float worldMinZ = -worldHalfSize;
	float worldMaxZ = worldHalfSize;
	float worldSpan = worldHalfSize * 2.f;
	float minRadius = 1.5f;
	float maxRadius = GetClamped( 6.f, minRadius, worldSpan * 0.5f );
	float minHeight = 3.f;
	float maxHeight = GetClamped( 10.f, minHeight, worldSpan );
	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		float radius = rng.RollRandomFloatInRange( minRadius, maxRadius );
		float height = rng.RollRandomFloatInRange( minHeight, maxHeight );
		float centerX = rng.RollRandomFloatInRange( worldMinX + radius, worldMaxX - radius );
		float centerY = rng.RollRandomFloatInRange( worldMinY + radius, worldMaxY - radius );
		float centerZ = rng.RollRandomFloatInRange( worldMinZ + ( height * 0.5f ), worldMaxZ - ( height * 0.5f ) );
		
		Vec3 start( centerX, centerY, centerZ - ( height * 0.5f ) );
		Vec3 end( centerX, centerY, centerZ + ( height * 0.5f ) );

		TestShapeZCylinder* shape = new TestShapeZCylinder( start, end, radius );
		m_testCylinders[shapeIndex] = shape;
	}
}