#include "Game/Game3DShapes.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/RaycastUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Game3DShapes::Game3DShapes()
{
	m_overlapPulseTimer = new Timer( 0.6 );
	m_overlapPulseTimer->Start();

	GenerateRandomShapes();

	Vec3 worldCameraPos = Vec3( -2.f, 2.f, 1.f );
	EulerAngles worldCameraOrientation = EulerAngles( 0.f, 0.f, 0.f );
	m_worldCamera->SetPositionAndOrientation( worldCameraPos, worldCameraOrientation );

	m_refPoint = worldCameraPos;
	m_refDirection = worldCameraOrientation.GetForwardDir_IFwd_JLeft_KUp();
	if ( m_refDirection.GetLengthSquared() > 0.f )
	{
		m_refDirection = m_refDirection.GetNormalized();
	}
}


//-----------------------------------------------------------------------------------------------
Game3DShapes::~Game3DShapes()
{
	delete m_overlapPulseTimer;
	m_overlapPulseTimer = nullptr;
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
	UpdateGrabbedShapeFromCamera();
	CheckIfShapesOverlap();
	GetNearestPoints();
	RaycastAgainstShapes();
	HandleShapeGrabbing();
	UpdateGrabbedShapeFromCamera();

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

	cameraOrientation.m_yawDegrees += static_cast< float >( mouseDelta.x ) * mouseSensitivity;
	cameraOrientation.m_pitchDegrees -= static_cast< float >( mouseDelta.y ) * mouseSensitivity;

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

	// Move camera
	if ( g_engine->m_inputSystem->IsKeyDown( 'W' ) ) cameraPos += forward * moveDistance;
	if ( g_engine->m_inputSystem->IsKeyDown( 'S' ) ) cameraPos -= forward * moveDistance;
	if ( g_engine->m_inputSystem->IsKeyDown( 'A' ) ) cameraPos += left * moveDistance;
	if ( g_engine->m_inputSystem->IsKeyDown( 'D' ) ) cameraPos -= left * moveDistance;
	if ( g_engine->m_inputSystem->IsKeyDown( 'Q' ) ) cameraPos.z += moveDistance;
	if ( g_engine->m_inputSystem->IsKeyDown( 'E' ) ) cameraPos.z -= moveDistance;

	m_worldCamera->SetPosition( cameraPos );
	m_worldCamera->SetOrientation( cameraOrientation );

	// Lock raycast
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_SPACE ) )
	{
		if ( m_isUsingCameraAsRefPoint )
		{
			m_refPoint = cameraPos;
			m_refDirection = cameraOrientation.GetForwardDir_IFwd_JLeft_KUp();
			if ( m_refDirection.GetLengthSquared() > 0.f )
			{
				m_refDirection = m_refDirection.GetNormalized();
			}
		}
		m_isUsingCameraAsRefPoint = !m_isUsingCameraAsRefPoint;
	}

	if ( m_isUsingCameraAsRefPoint )
	{
		m_refPoint = cameraPos;
		m_refDirection = cameraOrientation.GetForwardDir_IFwd_JLeft_KUp();
		if ( m_refDirection.GetLengthSquared() > 0.f )
		{
			m_refDirection = m_refDirection.GetNormalized();
		}
	}

	// Rotate grabbed OBB
	if ( m_isShapeGrabbed && m_grabbedShapeType == 3 )
	{
		TestShapeOBB3D* shape = m_testOBBs[m_grabbedShapeIndex];
		bool didRotate = false;

		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'I' ) ) {
			shape->m_orientation.m_yawDegrees += 10.f;
			didRotate = true;
		}
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'O' ) ) {
			shape->m_orientation.m_yawDegrees -= 10.f;
			didRotate = true;
		}
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'J' ) ) {
			shape->m_orientation.m_pitchDegrees += 10.f;
			didRotate = true;
		}
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'K' ) ) {
			shape->m_orientation.m_pitchDegrees -= 10.f;
			didRotate = true;
		}
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'N' ) ) {
			shape->m_orientation.m_rollDegrees += 10.f;
			didRotate = true;
		}
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'M' ) ) {
			shape->m_orientation.m_rollDegrees -= 10.f;
			didRotate = true;
		}
		if ( g_engine->m_inputSystem->WasKeyJustPressed( 'U' ) ) {
			shape->m_orientation = EulerAngles::ZERO;
			didRotate = true;
		}

		if ( didRotate )
		{
			Vec3 iBasis;
			Vec3 jBasis;
			Vec3 kBasis;
			shape->m_orientation.GetAsVectors_IFwd_JLeft_KUp( iBasis, jBasis, kBasis );
			shape->m_orientedBox.m_iBasisNormal = iBasis;
			shape->m_orientedBox.m_jBasisNormal = jBasis;
			shape->m_orientedBox.m_kBasisNormal = kBasis;
		}
	}

	// Randomize all shapes F8
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

	std::vector<Vertex> verts;
	std::vector<Vertex> wireframeVerts;
	std::vector<Vertex> solidVerts;

	float pulse = SinDegrees( static_cast<float>( m_overlapPulseTimer->GetElapsedFraction() * 360.0 ) );
	float pulseBrightness = 0.7f + ( 0.3f * pulse );
	Rgba8 pulsedLightBlue(
		static_cast<unsigned char>( static_cast<float>( m_lightBlue.r ) * pulseBrightness ),
		static_cast<unsigned char>( static_cast<float>( m_lightBlue.g ) * pulseBrightness ),
		static_cast<unsigned char>( static_cast<float>( m_lightBlue.b ) * pulseBrightness ),
		m_lightBlue.a );

	// AABBs
	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		TestShapeAABB3D* shape = m_testAABBs[shapeIndex];
		Rgba8 shapeColor = shape->m_color;
		if ( IsShapeGrabbed( 0, shapeIndex ) )
		{
			shapeColor = Rgba8::RED;
		}
		else if ( m_nearestRaycastResult.m_didImpact && m_impactedShapeType == 0 && m_impactedShapeIndex == shapeIndex )
		{
			shapeColor = pulsedLightBlue;
		}

		if ( shapeIndex % 2 == 0 ) AddVertsForAABBWireframe3D( wireframeVerts, shape->m_alignedBox, shapeColor );
		else AddVertsForAABB3D( solidVerts, shape->m_alignedBox, shapeColor, AABB2::ZERO_TO_ONE );
	}

	// Spheres
	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		TestShapeSphere* shape = m_testSpheres[shapeIndex];
		Rgba8 shapeColor = shape->m_color;
		if ( IsShapeGrabbed( 1, shapeIndex ) )
		{
			shapeColor = Rgba8::RED;
		}
		else if ( m_nearestRaycastResult.m_didImpact && m_impactedShapeType == 1 && m_impactedShapeIndex == shapeIndex )
		{
			shapeColor = pulsedLightBlue;
		}

		if ( shapeIndex % 2 == 0 ) AddVertsForSphereWireframe3D( wireframeVerts, shape->m_center, shape->m_radius, shapeColor, 16, 8 );
		else AddVertsForSphere3D( solidVerts, shape->m_center, shape->m_radius, shapeColor, AABB2::ZERO_TO_ONE, 16, 8 );
	}

	// Cylinders
	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		TestShapeZCylinder* shape = m_testCylinders[shapeIndex];
		Rgba8 shapeColor = shape->m_color;
		if ( IsShapeGrabbed( 2, shapeIndex ) )
		{
			shapeColor = Rgba8::RED;
		}
		else if ( m_nearestRaycastResult.m_didImpact && m_impactedShapeType == 2 && m_impactedShapeIndex == shapeIndex )
		{
			shapeColor = pulsedLightBlue;
		}

		if ( shapeIndex % 2 == 0 ) AddVertsForCylinderZWireframe3D( wireframeVerts, shape->m_start, shape->m_end, shape->m_radius, shapeColor, 16 );
		else AddVertsForCylinderZ3D( solidVerts, shape->m_start, shape->m_end, shape->m_radius, shapeColor, AABB2::ZERO_TO_ONE, 16 );
	}

	// OBBs
	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		TestShapeOBB3D* shape = m_testOBBs[shapeIndex];
		Rgba8 shapeColor = shape->m_color;
		if ( IsShapeGrabbed( 3, shapeIndex ) )
		{
			shapeColor = Rgba8::RED;
		}
		else if ( m_nearestRaycastResult.m_didImpact && m_impactedShapeType == 3 && m_impactedShapeIndex == shapeIndex )
		{
			shapeColor = pulsedLightBlue;
		}
		if ( shapeIndex % 2 == 0 ) AddVertsForOBBWireframe3D(wireframeVerts, shape->m_orientedBox, shapeColor );
		else AddVertsForOBB3D( solidVerts, shape->m_orientedBox, shapeColor );
	}

	// Grid
	for ( int shapeIndex = 0; shapeIndex < NUM_PLANE; ++shapeIndex )
	{
		TestShapePlane3D* shape = m_testPlanes[shapeIndex];
		Rgba8 shapeColor = shape->m_color;
		if ( IsShapeGrabbed( 4, shapeIndex ) )
		{
			shapeColor = Rgba8::RED;
		}
		else if ( m_nearestRaycastResult.m_didImpact && m_impactedShapeType == 4 && m_impactedShapeIndex == shapeIndex )
		{
			shapeColor = pulsedLightBlue;
		}
		AddVertsForPlane3D(wireframeVerts, shape->m_plane, 200.f, shapeColor );
	}

	// Impact debug
	if ( m_nearestRaycastResult.m_didImpact )
	{
		AddVertsForSphere3D( verts, m_nearestRaycastResult.m_impactPos, 0.05f, Rgba8::WHITE );
		AddVertsForArrow3D(
			verts,
			m_nearestRaycastResult.m_impactPos,
			m_nearestRaycastResult.m_impactPos + m_nearestRaycastResult.m_impactNormal,
			0.03f,
			Rgba8::YELLOW,
			16 );
	}

	// Locked ray debug (only when locked)
	if ( !m_isUsingCameraAsRefPoint )
	{
		Vec3 rayStart = m_nearestRaycastResult.m_rayStartPos;
		Vec3 rayMaxEnd = rayStart + ( m_nearestRaycastResult.m_rayFwdNormal * m_nearestRaycastResult.m_rayMaxLength );

		if ( m_nearestRaycastResult.m_didImpact )
		{
			AddVertsForArrow3D( verts, m_nearestRaycastResult.m_impactPos, rayMaxEnd, 0.03f, Rgba8( 190, 190, 190 ) );
			AddVertsForArrow3D( verts, rayStart, m_nearestRaycastResult.m_impactPos, 0.03f, Rgba8::RED );
		}
		else
		{
			AddVertsForArrow3D( verts, rayStart, rayMaxEnd, 0.03f, Rgba8::GREEN );
		}
	}

	// World axes at origin
	float axisRadius = 0.03f;
	int axisSlices = 16;
	Vec3 origin = Vec3::ZERO;
	AddVertsForArrow3D( verts, origin, Vec3( 1.f, 0.f, 0.f ), axisRadius, Rgba8::RED, axisSlices );
	AddVertsForArrow3D( verts, origin, Vec3( 0.f, 1.f, 0.f ), axisRadius, Rgba8::GREEN, axisSlices );
	AddVertsForArrow3D( verts, origin, Vec3( 0.f, 0.f, 1.f ), axisRadius, Rgba8::BLUE, axisSlices );

	// World axes in front of camera
	Vec3 cameraPos = m_worldCamera->GetPosition();
	Vec3 cameraFwd = m_worldCamera->GetOrientation().GetForwardDir_IFwd_JLeft_KUp();
	if ( cameraFwd.GetLengthSquared() > 0.f )
	{
		cameraFwd = cameraFwd.GetNormalized();
	}

	float cameraAxesOffset = 0.2f;
	float cameraAxesLength = 0.01f;
	float cameraAxesRadius = 0.0008f;

	Vec3 cameraAxesOrigin = cameraPos + ( cameraFwd * cameraAxesOffset );
	AddVertsForArrow3D( verts, cameraAxesOrigin, cameraAxesOrigin + Vec3( cameraAxesLength, 0.f, 0.f ), cameraAxesRadius, Rgba8::RED, axisSlices );
	AddVertsForArrow3D( verts, cameraAxesOrigin, cameraAxesOrigin + Vec3( 0.f, cameraAxesLength, 0.f ), cameraAxesRadius, Rgba8::GREEN, axisSlices );
	AddVertsForArrow3D( verts, cameraAxesOrigin, cameraAxesOrigin + Vec3( 0.f, 0.f, cameraAxesLength ), cameraAxesRadius, Rgba8::BLUE, axisSlices );

	// Nearest points
	if ( m_isUsingCameraAsRefPoint )
	{
		for ( int pointIndex = 0; pointIndex < static_cast<int>( m_nearestPointsToCamera.size() ); ++pointIndex )
		{
			Vec3 const& nearestPoint = m_nearestPointsToCamera[pointIndex];
			if ( nearestPoint != m_nearestPoint )
			{
				AddVertsForSphere3D( verts, nearestPoint, 0.2f, Rgba8( 255, 165, 0 ) );
			}
			else
			{
				AddVertsForSphere3D( verts, nearestPoint, 0.2f, Rgba8::GREEN );
			}
		}
	}
	else
	{
		for ( int pointIndex = 0; pointIndex < static_cast<int>( m_nearestPointsToRefPoint.size() ); ++pointIndex )
		{
			Vec3 const& nearestPoint = m_nearestPointsToRefPoint[pointIndex];
			if ( nearestPoint != m_nearestPoint )
			{
				AddVertsForSphere3D( verts, nearestPoint, 0.2f, Rgba8( 255, 165, 0 ) );
			}
			else
			{
				AddVertsForSphere3D( verts, nearestPoint, 0.2f, Rgba8::GREEN );
			}
		}
	}

	// Plane debug
	float planeOriginPointRadius = 0.2f;
	float planeOriginLineThickness = 0.05f;
	Rgba8 planeOriginPointColor( 170, 170, 170, 200 );
	Rgba8 planeOriginLineColor( 170, 170, 170, 120 );

	for ( int planeIndex = 0; planeIndex < NUM_PLANE; ++planeIndex )
	{
		TestShapePlane3D* planeShape = m_testPlanes[planeIndex];
		if ( planeShape == nullptr )
		{
			continue;
		}

		Vec3 nearestPointToOrigin = GetNearestPointOnPlane3D( Vec3::ZERO, planeShape->m_plane );
		AddVertsForSphere3D( verts, nearestPointToOrigin, planeOriginPointRadius, planeOriginPointColor );
		AddVertsForLineSegment3D( verts, Vec3::ZERO, nearestPointToOrigin, planeOriginLineThickness, planeOriginLineColor );

		Vec3 normal = planeShape->m_plane.m_normal;
		if ( DotProduct3D( normal, nearestPointToOrigin ) < 0.f )
		{
			normal = -normal;
		}

		AddVertsForArrow3D( verts, nearestPointToOrigin, nearestPointToOrigin + normal *2.f, planeOriginLineThickness, Rgba8::CYAN );
	}

	// Shapes
	Texture* texture = g_engine->m_renderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	g_engine->m_renderer->BindTexture( texture );
	g_engine->m_renderer->DrawVertexArray( solidVerts );

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( wireframeVerts );

	// Draw debug verts
	g_engine->m_renderer->SetBlendMode( BlendMode::ALPHA );
	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( verts );

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
	GenerateRandomOBBs();
	GenerateRandomPlanes();
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

	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		delete m_testSpheres[shapeIndex];
		m_testSpheres[shapeIndex] = nullptr;
	}

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

	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		delete m_testCylinders[shapeIndex];
		m_testCylinders[shapeIndex] = nullptr;
	}

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


//-----------------------------------------------------------------------------------------------
void Game3DShapes::GenerateRandomOBBs()
{
	RandomNumberGenerator rng;

	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		delete m_testOBBs[shapeIndex];
		m_testOBBs[shapeIndex] = nullptr;
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

		float yawDegrees = 10.f * static_cast<float>( rng.RollRandomIntInRange( 0, 35 ) );
		float pitchDegrees = 10.f * static_cast<float>( rng.RollRandomIntInRange( -18, 18 ) );
		float rollDegrees = 10.f * static_cast<float>( rng.RollRandomIntInRange( -18, 18 ) );

		EulerAngles orientation( yawDegrees, pitchDegrees, rollDegrees );
		Vec3 iBasis;
		Vec3 jBasis;
		Vec3 kBasis;
		orientation.GetAsVectors_IFwd_JLeft_KUp( iBasis, jBasis, kBasis );

		Vec3 center( boxLeft + ( boxWidth * 0.5f ), boxBottom + ( boxDepth * 0.5f ), boxMinZ + ( boxHeight * 0.5f ) );
		OBB3 obb( center, iBasis, jBasis, kBasis, Vec3( boxWidth * 0.5f, boxDepth * 0.5f, boxHeight * 0.5f ) );

		TestShapeOBB3D* shape = new TestShapeOBB3D( center, iBasis, jBasis, kBasis, Vec3( boxWidth * 0.5f, boxDepth * 0.5f, boxHeight * 0.5f ) );
		m_testOBBs[shapeIndex] = shape;
	}
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::GenerateRandomPlanes()
{
	RandomNumberGenerator rng;

	for ( int planeIndex = 0; planeIndex < NUM_PLANE; ++planeIndex )
	{
		delete m_testPlanes[planeIndex];
		m_testPlanes[planeIndex] = nullptr;
	}

	float worldHalfSize = 30.f;
	float worldMinX = -worldHalfSize;
	float worldMaxX = worldHalfSize;
	float worldMinY = -worldHalfSize;
	float worldMaxY = worldHalfSize;
	float worldMinZ = -worldHalfSize;
	float worldMaxZ = worldHalfSize;

	float worldSpan = worldHalfSize * 2.f;

	float minPlaneSize = 10.f;
	float maxPlaneSize = GetClamped( 20.f, minPlaneSize, worldSpan );

	for ( int planeIndex = 0; planeIndex < NUM_PLANE; ++planeIndex )
	{
		float planeSize = rng.RollRandomFloatInRange( minPlaneSize, maxPlaneSize );
		float centerX = rng.RollRandomFloatInRange( worldMinX + ( planeSize * 0.5f ), worldMaxX - ( planeSize * 0.5f ) );
		float centerY = rng.RollRandomFloatInRange( worldMinY + ( planeSize * 0.5f ), worldMaxY - ( planeSize * 0.5f ) );
		float centerZ = rng.RollRandomFloatInRange( worldMinZ + ( planeSize * 0.5f ), worldMaxZ - ( planeSize * 0.5f ) );

		Vec3 normal(
			rng.RollRandomFloatInRange( -1.f, 1.f ),
			rng.RollRandomFloatInRange( -1.f, 1.f ),
			rng.RollRandomFloatInRange( -1.f, 1.f ) );

		if ( normal.GetLengthSquared() == 0.f )
		{
			normal = Vec3( 0.f, 0.f, 1.f );
		}
		else
		{
			normal = normal.GetNormalized();
		}

		Vec3 planePoint( centerX, centerY, centerZ );
		float distanceFromOrigin = DotProduct3D( normal, planePoint );

		Plane3 plane( normal, distanceFromOrigin );
		m_testPlanes[planeIndex] = new TestShapePlane3D( plane );
	}
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::GetNearestPoints()
{
	m_nearestPointsToCamera.clear();
	m_nearestPointsToRefPoint.clear();

	if ( m_isUsingCameraAsRefPoint ) GetNearestPointsToCamera();
	else GetNearestPointsToRefPoint();
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::GetNearestPointsToCamera()
{
	Vec3 cameraPos = m_worldCamera->GetPosition();
	float nearestPointDistanceSquared = std::numeric_limits<float>::max();

	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		TestShapeAABB3D* aabbShape = m_testAABBs[shapeIndex];
		Vec3 nearestPointToCamera = GetNearestPointOnAABB3D( cameraPos, aabbShape->m_alignedBox.m_mins, aabbShape->m_alignedBox.m_maxs );
		float nearestPointDistanceToCameraSquared = GetDistanceSquared3D( cameraPos, nearestPointToCamera );
		if ( nearestPointDistanceToCameraSquared < nearestPointDistanceSquared )
		{
			nearestPointDistanceSquared = nearestPointDistanceToCameraSquared;
			m_nearestPoint = nearestPointToCamera;
		}
		m_nearestPointsToCamera.push_back( nearestPointToCamera );

		TestShapeSphere* sphereShape = m_testSpheres[shapeIndex];
		nearestPointToCamera = GetNearestPointOnSphere3D( cameraPos, sphereShape->m_center, sphereShape->m_radius );
		nearestPointDistanceToCameraSquared = GetDistanceSquared3D( cameraPos, nearestPointToCamera );
		if ( nearestPointDistanceToCameraSquared < nearestPointDistanceSquared )
		{
			nearestPointDistanceSquared = nearestPointDistanceToCameraSquared;
			m_nearestPoint = nearestPointToCamera;
		}
		m_nearestPointsToCamera.push_back( nearestPointToCamera );

		TestShapeZCylinder* cylinderShape = m_testCylinders[shapeIndex];
		Vec2 cylinderBaseCenter( cylinderShape->m_start.x, cylinderShape->m_start.y );
		nearestPointToCamera = GetNearestPointOnCylinderZ3D( cameraPos, cylinderBaseCenter, cylinderShape->m_start.z, cylinderShape->m_end.z, cylinderShape->m_radius );
		nearestPointDistanceToCameraSquared = GetDistanceSquared3D( cameraPos, nearestPointToCamera );
		if ( nearestPointDistanceToCameraSquared < nearestPointDistanceSquared )
		{
			nearestPointDistanceSquared = nearestPointDistanceToCameraSquared;
			m_nearestPoint = nearestPointToCamera;
		}
		m_nearestPointsToCamera.push_back( nearestPointToCamera );

		TestShapeOBB3D* obbShape = m_testOBBs[shapeIndex];
		nearestPointToCamera = GetNearestPointOnOBB3D( cameraPos, obbShape->m_orientedBox );
		nearestPointDistanceToCameraSquared = GetDistanceSquared3D( cameraPos, nearestPointToCamera );
		if ( nearestPointDistanceToCameraSquared < nearestPointDistanceSquared )
		{
			nearestPointDistanceSquared = nearestPointDistanceToCameraSquared;
			m_nearestPoint = nearestPointToCamera;
		}
		m_nearestPointsToCamera.push_back( nearestPointToCamera );
	}

	for ( int planeIndex = 0; planeIndex < NUM_PLANE; ++planeIndex )
	{
		TestShapePlane3D* planeShape = m_testPlanes[planeIndex];
		Vec3 nearestPointToCamera = GetNearestPointOnPlane3D( cameraPos, planeShape->m_plane );
		float nearestPointDistanceToCameraSquared = GetDistanceSquared3D( cameraPos, nearestPointToCamera );
		if ( nearestPointDistanceToCameraSquared < nearestPointDistanceSquared )
		{
			nearestPointDistanceSquared = nearestPointDistanceToCameraSquared;
			m_nearestPoint = nearestPointToCamera;
		}
		m_nearestPointsToCamera.push_back( nearestPointToCamera );
	}
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::GetNearestPointsToRefPoint()
{
	float nearestPointDistanceSquared = std::numeric_limits<float>::max();

	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		TestShapeAABB3D* aabbShape = m_testAABBs[shapeIndex];
		Vec3 nearestPointToRefPoint = GetNearestPointOnAABB3D( m_refPoint, aabbShape->m_alignedBox.m_mins, aabbShape->m_alignedBox.m_maxs );
		float nearestPointDistanceToRefPointSquared = GetDistanceSquared3D( m_refPoint, nearestPointToRefPoint );
		if ( nearestPointDistanceToRefPointSquared < nearestPointDistanceSquared )
		{
			nearestPointDistanceSquared = nearestPointDistanceToRefPointSquared;
			m_nearestPoint = nearestPointToRefPoint;
		}
		m_nearestPointsToRefPoint.push_back( nearestPointToRefPoint );

		TestShapeSphere* sphereShape = m_testSpheres[shapeIndex];
		nearestPointToRefPoint = GetNearestPointOnSphere3D( m_refPoint, sphereShape->m_center, sphereShape->m_radius );
		nearestPointDistanceToRefPointSquared = GetDistanceSquared3D( m_refPoint, nearestPointToRefPoint );
		if ( nearestPointDistanceToRefPointSquared < nearestPointDistanceSquared )
		{
			nearestPointDistanceSquared = nearestPointDistanceToRefPointSquared;
			m_nearestPoint = nearestPointToRefPoint;
		}
		m_nearestPointsToRefPoint.push_back( nearestPointToRefPoint );

		TestShapeZCylinder* cylinderShape = m_testCylinders[shapeIndex];
		Vec2 cylinderBaseCenter( cylinderShape->m_start.x, cylinderShape->m_start.y );
		nearestPointToRefPoint = GetNearestPointOnCylinderZ3D( m_refPoint, cylinderBaseCenter, cylinderShape->m_start.z, cylinderShape->m_end.z, cylinderShape->m_radius );
		nearestPointDistanceToRefPointSquared = GetDistanceSquared3D( m_refPoint, nearestPointToRefPoint );
		if ( nearestPointDistanceToRefPointSquared < nearestPointDistanceSquared )
		{
			nearestPointDistanceSquared = nearestPointDistanceToRefPointSquared;
			m_nearestPoint = nearestPointToRefPoint;
		}
		m_nearestPointsToRefPoint.push_back( nearestPointToRefPoint );

		TestShapeOBB3D* obbShape = m_testOBBs[shapeIndex];
		nearestPointToRefPoint = GetNearestPointOnOBB3D( m_refPoint, obbShape->m_orientedBox );
		nearestPointDistanceToRefPointSquared = GetDistanceSquared3D( m_refPoint, nearestPointToRefPoint );
		if ( nearestPointDistanceToRefPointSquared < nearestPointDistanceSquared )
		{
			nearestPointDistanceSquared = nearestPointDistanceToRefPointSquared;
			m_nearestPoint = nearestPointToRefPoint;
		}
		m_nearestPointsToRefPoint.push_back( nearestPointToRefPoint );
	}

	for ( int planeIndex = 0; planeIndex < NUM_PLANE; ++planeIndex )
	{
		TestShapePlane3D* planeShape = m_testPlanes[planeIndex];
		Vec3 nearestPointToRefPoint = GetNearestPointOnPlane3D( m_refPoint, planeShape->m_plane );
		float nearestPointDistanceToRefPointSquared = GetDistanceSquared3D( m_refPoint, nearestPointToRefPoint );
		if ( nearestPointDistanceToRefPointSquared < nearestPointDistanceSquared )
		{
			nearestPointDistanceSquared = nearestPointDistanceToRefPointSquared;
			m_nearestPoint = nearestPointToRefPoint;
		}
		m_nearestPointsToRefPoint.push_back( nearestPointToRefPoint );
	}
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::CheckIfShapesOverlap()
{
	if ( m_overlapPulseTimer == nullptr )
	{
		return;
	}

	if ( m_overlapPulseTimer->IsStopped() )
	{
		m_overlapPulseTimer->Start();
	}

	m_overlapPulseTimer->DecrementPeriodIfElapsed();

	float pulse = SinDegrees( static_cast<float>( m_overlapPulseTimer->GetElapsedFraction() * 360.0 ) );
	float pulseBrightness = 0.7f + ( 0.3f * pulse );
	

	// AABBs
	for ( int aabbIndex = 0; aabbIndex < NUM_SHAPE_PER_TYPE; ++aabbIndex )
	{
		TestShapeAABB3D* aabbShape = m_testAABBs[aabbIndex];
		bool isOverlapping = false;

		for ( int otherAabbIndex = 0; otherAabbIndex < NUM_SHAPE_PER_TYPE && !isOverlapping; ++otherAabbIndex )
		{
			if ( otherAabbIndex == aabbIndex )
			{
				continue;
			}

			if ( DoAABBsOverlap3D( aabbShape->m_alignedBox, m_testAABBs[otherAabbIndex]->m_alignedBox ) )
			{
				isOverlapping = true;
			}
		}

		for ( int sphereIndex = 0; sphereIndex < NUM_SHAPE_PER_TYPE && !isOverlapping; ++sphereIndex )
		{
			TestShapeSphere* sphere = m_testSpheres[sphereIndex];
			if ( DoSphereAndAABBOverlap3D( sphere->m_center, sphere->m_radius, aabbShape->m_alignedBox ) )
			{
				isOverlapping = true;
			}
		}

		for ( int cylinderIndex = 0; cylinderIndex < NUM_SHAPE_PER_TYPE && !isOverlapping; ++cylinderIndex )
		{
			TestShapeZCylinder* cylinder = m_testCylinders[cylinderIndex];
			Vec2 cylinderCenterXY( cylinder->m_start.x, cylinder->m_start.y );
			float minZ = ( cylinder->m_start.z < cylinder->m_end.z ) ? cylinder->m_start.z : cylinder->m_end.z;
			float maxZ = ( cylinder->m_start.z > cylinder->m_end.z ) ? cylinder->m_start.z : cylinder->m_end.z;
			FloatRange cylinderMinZMaxZ( minZ, maxZ );

			if ( DoZCylinderAndAABBOverlap3D( cylinderCenterXY, cylinder->m_radius, cylinderMinZMaxZ, aabbShape->m_alignedBox ) )
			{
				isOverlapping = true;
			}
		}

		for ( int planeIndex = 0; planeIndex < NUM_PLANE && !isOverlapping; ++planeIndex )
		{
			TestShapePlane3D* plane = m_testPlanes[planeIndex];
			if ( DoPlaneAndAABBOverlap3D( plane->m_plane, aabbShape->m_alignedBox ) )
			{
				isOverlapping = true;
			}
		}

		Rgba8 defaultColor = ( ( aabbIndex % 2 ) == 0 ) ? m_darkBlue : Rgba8::WHITE;
		Rgba8 overlapColor(
			static_cast<unsigned char>( static_cast<float>( defaultColor.r ) * pulseBrightness ),
			static_cast<unsigned char>( static_cast<float>( defaultColor.g ) * pulseBrightness ),
			static_cast<unsigned char>( static_cast<float>( defaultColor.b ) * pulseBrightness ),
			defaultColor.a );
		aabbShape->m_color = isOverlapping ? overlapColor : defaultColor;

	}

	// Spheres
	for ( int sphereIndex = 0; sphereIndex < NUM_SHAPE_PER_TYPE; ++sphereIndex )
	{
		TestShapeSphere* sphere = m_testSpheres[sphereIndex];
		bool isOverlapping = false;

		for ( int otherSphereIndex = 0; otherSphereIndex < NUM_SHAPE_PER_TYPE && !isOverlapping; ++otherSphereIndex )
		{
			if ( otherSphereIndex == sphereIndex )
			{
				continue;
			}

			TestShapeSphere* otherSphere = m_testSpheres[otherSphereIndex];
			if ( DoSpheresOverlap3D( sphere->m_center, sphere->m_radius, otherSphere->m_center, otherSphere->m_radius ) )
			{
				isOverlapping = true;
			}
		}

		for ( int aabbIndex = 0; aabbIndex < NUM_SHAPE_PER_TYPE && !isOverlapping; ++aabbIndex )
		{
			if ( DoSphereAndAABBOverlap3D( sphere->m_center, sphere->m_radius, m_testAABBs[aabbIndex]->m_alignedBox ) )
			{
				isOverlapping = true;
			}
		}

		for ( int cylinderIndex = 0; cylinderIndex < NUM_SHAPE_PER_TYPE && !isOverlapping; ++cylinderIndex )
		{
			TestShapeZCylinder* cylinder = m_testCylinders[cylinderIndex];
			Vec2 cylinderCenterXY( cylinder->m_start.x, cylinder->m_start.y );
			float minZ = ( cylinder->m_start.z < cylinder->m_end.z ) ? cylinder->m_start.z : cylinder->m_end.z;
			float maxZ = ( cylinder->m_start.z > cylinder->m_end.z ) ? cylinder->m_start.z : cylinder->m_end.z;
			FloatRange cylinderMinZMaxZ( minZ, maxZ );

			if ( DoZCylinderAndSphereOverlap3D( cylinderCenterXY, cylinder->m_radius, cylinderMinZMaxZ, sphere->m_center, sphere->m_radius ) )
			{
				isOverlapping = true;
			}
		}

		for ( int obbIndex = 0; obbIndex < NUM_SHAPE_PER_TYPE && !isOverlapping; ++obbIndex )
		{
			TestShapeOBB3D* obbShape = m_testOBBs[obbIndex];
			if ( DoOBBAndSphereOverlap3D( obbShape->m_orientedBox, sphere->m_center, sphere->m_radius ) )
			{
				isOverlapping = true;
			}
		}

		for ( int planeIndex = 0; planeIndex < NUM_PLANE && !isOverlapping; ++planeIndex )
		{
			TestShapePlane3D* plane = m_testPlanes[planeIndex];
			if ( DoPlaneAndSphereOverlap3D( plane->m_plane, sphere->m_center, sphere->m_radius ) )
			{
				isOverlapping = true;
			}
		}

		Rgba8 defaultColor = ( ( sphereIndex % 2 ) == 0 ) ? m_darkBlue : Rgba8::WHITE;
		Rgba8 overlapColor(
			static_cast<unsigned char>( static_cast<float>( defaultColor.r ) * pulseBrightness ),
			static_cast<unsigned char>( static_cast<float>( defaultColor.g ) * pulseBrightness ),
			static_cast<unsigned char>( static_cast<float>( defaultColor.b ) * pulseBrightness ),
			defaultColor.a );
		sphere->m_color = isOverlapping ? overlapColor : defaultColor;

	}

	// Cylinders
	for ( int cylinderIndex = 0; cylinderIndex < NUM_SHAPE_PER_TYPE; ++cylinderIndex )
	{
		TestShapeZCylinder* cylinder = m_testCylinders[cylinderIndex];
		Vec2 cylinderCenterXY( cylinder->m_start.x, cylinder->m_start.y );
		float minZ = ( cylinder->m_start.z < cylinder->m_end.z ) ? cylinder->m_start.z : cylinder->m_end.z;
		float maxZ = ( cylinder->m_start.z > cylinder->m_end.z ) ? cylinder->m_start.z : cylinder->m_end.z;
		FloatRange cylinderMinZMaxZ( minZ, maxZ );

		bool isOverlapping = false;

		for ( int otherCylinderIndex = 0; otherCylinderIndex < NUM_SHAPE_PER_TYPE && !isOverlapping; ++otherCylinderIndex )
		{
			if ( otherCylinderIndex == cylinderIndex )
			{
				continue;
			}

			TestShapeZCylinder* otherCylinder = m_testCylinders[otherCylinderIndex];
			Vec2 otherCenterXY( otherCylinder->m_start.x, otherCylinder->m_start.y );
			float otherMinZ = ( otherCylinder->m_start.z < otherCylinder->m_end.z ) ? otherCylinder->m_start.z : otherCylinder->m_end.z;
			float otherMaxZ = ( otherCylinder->m_start.z > otherCylinder->m_end.z ) ? otherCylinder->m_start.z : otherCylinder->m_end.z;
			FloatRange otherMinZMaxZ( otherMinZ, otherMaxZ );

			if ( DoZCylindersOverlap3D( cylinderCenterXY, cylinder->m_radius, cylinderMinZMaxZ, otherCenterXY, otherCylinder->m_radius, otherMinZMaxZ ) )
			{
				isOverlapping = true;
			}
		}

		for ( int aabbIndex = 0; aabbIndex < NUM_SHAPE_PER_TYPE && !isOverlapping; ++aabbIndex )
		{
			if ( DoZCylinderAndAABBOverlap3D( cylinderCenterXY, cylinder->m_radius, cylinderMinZMaxZ, m_testAABBs[aabbIndex]->m_alignedBox ) )
			{
				isOverlapping = true;
			}
		}

		for ( int sphereIndex = 0; sphereIndex < NUM_SHAPE_PER_TYPE && !isOverlapping; ++sphereIndex )
		{
			TestShapeSphere* sphere = m_testSpheres[sphereIndex];
			if ( DoZCylinderAndSphereOverlap3D( cylinderCenterXY, cylinder->m_radius, cylinderMinZMaxZ, sphere->m_center, sphere->m_radius ) )
			{
				isOverlapping = true;
			}
		}

		Rgba8 defaultColor = ( ( cylinderIndex % 2 ) == 0 ) ? m_darkBlue : Rgba8::WHITE;
		Rgba8 overlapColor(
			static_cast<unsigned char>( static_cast<float>( defaultColor.r ) * pulseBrightness ),
			static_cast<unsigned char>( static_cast<float>( defaultColor.g ) * pulseBrightness ),
			static_cast<unsigned char>( static_cast<float>( defaultColor.b ) * pulseBrightness ),
			defaultColor.a );
		cylinder->m_color = isOverlapping ? overlapColor : defaultColor;
	}

	// OBBs
	for ( int obbIndex = 0; obbIndex < NUM_SHAPE_PER_TYPE; ++obbIndex )
	{
		TestShapeOBB3D* obbShape = m_testOBBs[obbIndex];
		bool isOverlapping = false;
		
		for ( int sphereIndex = 0; sphereIndex < NUM_SHAPE_PER_TYPE && !isOverlapping; ++sphereIndex )
		{
			TestShapeSphere* sphere = m_testSpheres[sphereIndex];
			if ( DoOBBAndSphereOverlap3D( obbShape->m_orientedBox, sphere->m_center, sphere->m_radius ) )
			{
				isOverlapping = true;
			}
		}

		for ( int planeIndex = 0; planeIndex < NUM_PLANE && !isOverlapping; ++planeIndex )
		{
			TestShapePlane3D* plane = m_testPlanes[planeIndex];
			if ( DoOBBAndPlaneOverlap3D( obbShape->m_orientedBox, plane->m_plane ) )
			{
				isOverlapping = true;
			}
		}

		Rgba8 defaultColor = ( ( obbIndex % 2 ) == 0 ) ? m_darkBlue : Rgba8::WHITE;
		Rgba8 overlapColor(
			static_cast<unsigned char>( static_cast<float>( defaultColor.r ) * pulseBrightness ),
			static_cast<unsigned char>( static_cast<float>( defaultColor.g ) * pulseBrightness ),
			static_cast<unsigned char>( static_cast<float>( defaultColor.b ) * pulseBrightness ),
			defaultColor.a );
		obbShape->m_color = isOverlapping ? overlapColor : defaultColor;
	}

	// Planes
	for ( int planeIndex = 0; planeIndex < NUM_PLANE; ++planeIndex )
	{
		TestShapePlane3D* plane = m_testPlanes[planeIndex];
		bool isOverlapping = false;

		for ( int aabbIndex = 0; aabbIndex < NUM_SHAPE_PER_TYPE && !isOverlapping; ++aabbIndex )
		{
			if ( DoPlaneAndAABBOverlap3D( plane->m_plane, m_testAABBs[aabbIndex]->m_alignedBox ) )
			{
				isOverlapping = true;
			}
		}

		for ( int sphereIndex = 0; sphereIndex < NUM_SHAPE_PER_TYPE && !isOverlapping; ++sphereIndex )
		{
			TestShapeSphere* sphere = m_testSpheres[sphereIndex];
			if ( DoPlaneAndSphereOverlap3D( plane->m_plane, sphere->m_center, sphere->m_radius ) )
			{
				isOverlapping = true;
			}
		}

		for ( int obbIndex = 0; obbIndex < NUM_SHAPE_PER_TYPE && !isOverlapping; ++obbIndex )
		{
			TestShapeOBB3D* obbShape = m_testOBBs[obbIndex];
			if ( DoOBBAndPlaneOverlap3D( obbShape->m_orientedBox, plane->m_plane ) )
			{
				isOverlapping = true;
			}
		}

		Rgba8 defaultColor = ( ( planeIndex % 2 ) == 0 ) ? m_darkBlue : Rgba8::WHITE;
		Rgba8 overlapColor(
			static_cast< unsigned char >( static_cast< float >( defaultColor.r )* pulseBrightness ),
			static_cast< unsigned char >( static_cast< float >( defaultColor.g )* pulseBrightness ),
			static_cast< unsigned char >( static_cast< float >( defaultColor.b )* pulseBrightness ),
			defaultColor.a );
		plane->m_color = isOverlapping ? overlapColor : defaultColor;
	}
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::RaycastAgainstShapes()
{
	Vec3 rayStart;
	Vec3 rayDirection;

	if ( m_isUsingCameraAsRefPoint )
	{
		rayStart = m_worldCamera->GetPosition();
		rayDirection = m_worldCamera->GetOrientation().GetForwardDir_IFwd_JLeft_KUp();
	}
	else
	{
		rayStart = m_refPoint;
		rayDirection = m_refDirection.GetNormalized();
	}

	m_nearestRaycastResult = RaycastResult3D();
	m_nearestRaycastResult.m_rayStartPos = rayStart;
	m_nearestRaycastResult.m_rayFwdNormal = rayDirection;
	m_nearestRaycastResult.m_rayMaxLength = m_raycastMaxLength;
	m_impactedShapeType = -1;
	m_impactedShapeIndex = -1;

	for ( int shapeIndex = 0; shapeIndex < NUM_SHAPE_PER_TYPE; ++shapeIndex )
	{
		RaycastResult3D aabbResult = RaycastVsAABB3D( rayStart, rayDirection, m_raycastMaxLength, m_testAABBs[shapeIndex]->m_alignedBox );
		if ( aabbResult.m_didImpact && ( !m_nearestRaycastResult.m_didImpact || aabbResult.m_impactDist < m_nearestRaycastResult.m_impactDist ) )
		{
			m_nearestRaycastResult = aabbResult;
			m_impactedShapeType = 0;
			m_impactedShapeIndex = shapeIndex;
		}

		RaycastResult3D sphereResult = RaycastVsSphere3D( rayStart, rayDirection, m_raycastMaxLength, m_testSpheres[shapeIndex]->m_center, m_testSpheres[shapeIndex]->m_radius );
		if ( sphereResult.m_didImpact && ( !m_nearestRaycastResult.m_didImpact || sphereResult.m_impactDist < m_nearestRaycastResult.m_impactDist ) )
		{
			m_nearestRaycastResult = sphereResult;
			m_impactedShapeType = 1;
			m_impactedShapeIndex = shapeIndex;
		}

		TestShapeZCylinder* cylinder = m_testCylinders[shapeIndex];
		float minZ = ( cylinder->m_start.z < cylinder->m_end.z ) ? cylinder->m_start.z : cylinder->m_end.z;
		float maxZ = ( cylinder->m_start.z > cylinder->m_end.z ) ? cylinder->m_start.z : cylinder->m_end.z;
		Vec2 centerXY( cylinder->m_start.x, cylinder->m_start.y );
		RaycastResult3D cylinderResult = RaycastVsCylinderZ3D( rayStart, rayDirection, m_raycastMaxLength, centerXY, minZ, maxZ, cylinder->m_radius );
		if ( cylinderResult.m_didImpact && ( !m_nearestRaycastResult.m_didImpact || cylinderResult.m_impactDist < m_nearestRaycastResult.m_impactDist ) )
		{
			m_nearestRaycastResult = cylinderResult;
			m_impactedShapeType = 2;
			m_impactedShapeIndex = shapeIndex;
		}

		RaycastResult3D obbResult = RaycastVsOBB3D( rayStart, rayDirection, m_raycastMaxLength, m_testOBBs[shapeIndex]->m_orientedBox );
		if ( obbResult.m_didImpact && ( !m_nearestRaycastResult.m_didImpact || obbResult.m_impactDist < m_nearestRaycastResult.m_impactDist ) )
		{
			m_nearestRaycastResult = obbResult;
			m_impactedShapeType = 3;
			m_impactedShapeIndex = shapeIndex;
		}
	}

	for ( int planeIndex = 0; planeIndex < NUM_PLANE; ++planeIndex )
	{
		RaycastResult3D planeResult = RaycastVsPlane3D( rayStart, rayDirection, m_raycastMaxLength, m_testPlanes[planeIndex]->m_plane );
		if ( planeResult.m_didImpact && ( !m_nearestRaycastResult.m_didImpact || planeResult.m_impactDist < m_nearestRaycastResult.m_impactDist ) )
		{
			m_nearestRaycastResult = planeResult;
			m_impactedShapeType = 4;
			m_impactedShapeIndex = planeIndex;
		}
	}

	m_nearestRaycastResult.m_rayStartPos = rayStart;
	m_nearestRaycastResult.m_rayFwdNormal = rayDirection;
	m_nearestRaycastResult.m_rayMaxLength = m_raycastMaxLength;
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::HandleShapeGrabbing()
{
	if ( !g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_LBUTTON ) )
	{
		return;
	}

	if ( !m_nearestRaycastResult.m_didImpact )
	{
		return;
	}

	if ( m_isShapeGrabbed && m_grabbedShapeType == m_impactedShapeType && m_grabbedShapeIndex == m_impactedShapeIndex )
	{
		ClearGrabbedShape();
		return;
	}

	m_isShapeGrabbed = true;
	m_grabbedShapeType = m_impactedShapeType;
	m_grabbedShapeIndex = m_impactedShapeIndex;

	Vec3 cameraPos = m_worldCamera->GetPosition();
	Vec3 forward;
	Vec3 left;
	Vec3 up;
	m_worldCamera->GetOrientation().GetAsVectors_IFwd_JLeft_KUp( forward, left, up );

	Vec3 shapeCenter = GetShapeCenterWorldPosition( m_grabbedShapeType, m_grabbedShapeIndex );
	Vec3 cameraToShape = shapeCenter - cameraPos;
	m_grabbedShapeLocalPosition = Vec3(
		DotProduct3D( cameraToShape, forward ),
		DotProduct3D( cameraToShape, left ),
		DotProduct3D( cameraToShape, up ) );
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::UpdateGrabbedShapeFromCamera()
{
	if ( !m_isShapeGrabbed )
	{
		return;
	}

	Vec3 forward;
	Vec3 left;
	Vec3 up;
	m_worldCamera->GetOrientation().GetAsVectors_IFwd_JLeft_KUp( forward, left, up );

	Vec3 cameraPos = m_worldCamera->GetPosition();
	Vec3 targetWorldCenter = cameraPos
		+ ( forward * m_grabbedShapeLocalPosition.x )
		+ ( left * m_grabbedShapeLocalPosition.y )
		+ ( up * m_grabbedShapeLocalPosition.z );

	SetShapeCenterWorldPosition( m_grabbedShapeType, m_grabbedShapeIndex, targetWorldCenter );
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::ClearGrabbedShape()
{
	m_isShapeGrabbed = false;
	m_grabbedShapeType = -1;
	m_grabbedShapeIndex = -1;
	m_grabbedShapeLocalPosition = Vec3::ZERO;
}


//-----------------------------------------------------------------------------------------------
bool Game3DShapes::IsShapeGrabbed( int shapeType, int shapeIndex ) const
{
	return m_isShapeGrabbed && m_grabbedShapeType == shapeType && m_grabbedShapeIndex == shapeIndex;
}


//-----------------------------------------------------------------------------------------------
Vec3 Game3DShapes::GetShapeCenterWorldPosition( int shapeType, int shapeIndex ) const
{
	if ( shapeType == 0 )
	{
		TestShapeAABB3D const* shape = m_testAABBs[shapeIndex];
		return ( shape->m_alignedBox.m_mins + shape->m_alignedBox.m_maxs ) * 0.5f;
	}

	if ( shapeType == 1 )
	{
		return m_testSpheres[shapeIndex]->m_center;
	}

	if ( shapeType == 2 )
	{
		TestShapeZCylinder const* shape = m_testCylinders[shapeIndex];
		return ( shape->m_start + shape->m_end ) * 0.5f;
	}

	if ( shapeType == 3 )
	{
		return m_testOBBs[shapeIndex]->m_orientedBox.m_center;
	}

	return Vec3::ZERO;
}


//-----------------------------------------------------------------------------------------------
void Game3DShapes::SetShapeCenterWorldPosition( int shapeType, int shapeIndex, Vec3 const& worldCenter )
{
	if ( shapeType == 0 )
	{
		TestShapeAABB3D* shape = m_testAABBs[shapeIndex];
		Vec3 currentCenter = ( shape->m_alignedBox.m_mins + shape->m_alignedBox.m_maxs ) * 0.5f;
		Vec3 delta = worldCenter - currentCenter;
		shape->m_alignedBox.m_mins += delta;
		shape->m_alignedBox.m_maxs += delta;
		return;
	}

	if ( shapeType == 1 )
	{
		m_testSpheres[shapeIndex]->m_center = worldCenter;
		return;
	}

	if ( shapeType == 2 )
	{
		TestShapeZCylinder* shape = m_testCylinders[shapeIndex];
		Vec3 currentCenter = ( shape->m_start + shape->m_end ) * 0.5f;
		Vec3 delta = worldCenter - currentCenter;
		shape->m_start += delta;
		shape->m_end += delta;
		return;
	}

	if ( shapeType == 3 )
	{
		TestShapeOBB3D* shape = m_testOBBs[shapeIndex];
		shape->m_orientedBox.m_center = worldCenter;
		return;
	}
}