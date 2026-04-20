#include "Game/Game2DCurves.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Easing.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Game2DCurves::Game2DCurves()
{
	float const panelPadding = 2.f;

	AABB2 usableSpace = AABB2( Vec2( 0.f, 0.f ), Vec2( WORLD_SIZE_X, WORLD_SIZE_Y - 8.f ) );
	AABB2 topHalf = usableSpace;

	m_bezierPanel = topHalf.ChopTop( 0.5f );
	m_splinePanel = topHalf;
	m_easingPanel = m_bezierPanel.ChopLeft( 0.5f );

	m_splinePanel.PadOnAllSides( panelPadding );
	m_bezierPanel.PadOnAllSides( panelPadding );
	m_easingPanel.PadOnAllSides( panelPadding );

	float easingFunctionGraphSize = m_easingPanel.GetDimensions().y * 0.85f;
	float halfGraphSize = easingFunctionGraphSize * 0.5f;
	float graphCenterX = m_easingPanel.GetCenter().x;

	m_easingFunctionGraphBounds = AABB2(
		Vec2( graphCenterX - halfGraphSize, m_easingPanel.m_maxs.y - easingFunctionGraphSize ),
		Vec2( graphCenterX + halfGraphSize, m_easingPanel.m_maxs.y ) );

	RandomNumberGenerator rng;

	Randomize();
}


//-----------------------------------------------------------------------------------------------
Game2DCurves::~Game2DCurves()
{
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::Update( float deltaSeconds )
{
	m_parametricT += deltaSeconds * 0.5f;
	if ( m_parametricT > 1.f )
	{
		m_parametricT -= 1.f;
	}

	// Easing function point
	float easedT = GetEasingFunction()( m_parametricT );
	m_easingFunctionPoint = m_easingFunctionGraphBounds.m_mins + ( Vec2( m_parametricT, easedT ) * m_easingFunctionGraphBounds.GetDimensions() );

	UpdateFromKeyboard( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::UpdateFromKeyboard( [[maybe_unused]] float deltaSeconds )
{
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F1 ) )
	{
		m_isDebugDraw = !m_isDebugDraw;
	}

	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F8 ) )
	{
		Randomize();
	}

	if ( g_engine->m_inputSystem->WasKeyJustPressed( 'W' ) )
	{
		m_easingFunction = ( EasingFunctionLabel ) ( ( int ) m_easingFunction - 1 );
		if ( m_easingFunction < EasingFunctionLabel::IDENTITY )
		{
			m_easingFunction = ( EasingFunctionLabel ) ( ( int ) EasingFunctionLabel::NUM_EASING_FUNCTIONS - 1 );
		}
	}
	if ( g_engine->m_inputSystem->WasKeyJustPressed( 'E' ) )
	{
		m_easingFunction = ( EasingFunctionLabel ) ( ( int ) m_easingFunction + 1 );
		if ( m_easingFunction >= EasingFunctionLabel::NUM_EASING_FUNCTIONS )
		{
			m_easingFunction = EasingFunctionLabel::IDENTITY;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::Render() const
{
	g_engine->m_renderer->BeginCamera( *m_worldCamera );

	g_engine->m_renderer->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
	g_engine->m_renderer->SetBlendMode( BlendMode::OPAQUE );

	std::vector<Vertex> verts;

	if ( m_isDebugDraw )
	{
		Rgba8 const panelColor( 100, 0, 0 );
		AddVertsForAABB2D( verts, m_splinePanel, panelColor, Vec2::ZERO, Vec2::ONE );
		AddVertsForAABB2D( verts, m_bezierPanel, panelColor, Vec2::ZERO, Vec2::ONE );
		AddVertsForAABB2D( verts, m_easingPanel, panelColor, Vec2::ZERO, Vec2::ONE );
	}

	// Easing functions
	RenderEasingFunctionGraph( verts );
	RenderEasingFunctionPoint( verts );

	// Cubic Bezier curve
	RenderCubicBezierCurve( verts );
	RenderCubicBezierPoint( verts );

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
std::string Game2DCurves::GetEasingFunctionLabel() const
{
	switch ( m_easingFunction )
	{
		case EasingFunctionLabel::IDENTITY:			return "Identity";
		case EasingFunctionLabel::SMOOTH_START_2:	return "SmoothStart2";
		case EasingFunctionLabel::SMOOTH_START_3:	return "SmoothStart3";
		case EasingFunctionLabel::SMOOTH_START_4:	return "SmoothStart4";
		case EasingFunctionLabel::SMOOTH_START_5:	return "SmoothStart5";
		case EasingFunctionLabel::SMOOTH_START_6:	return "SmoothStart6";
		case EasingFunctionLabel::SMOOTH_STOP_2:	return "SmoothStop2";
		case EasingFunctionLabel::SMOOTH_STOP_3:	return "SmoothStop3";
		case EasingFunctionLabel::SMOOTH_STOP_4:	return "SmoothStop4";
		case EasingFunctionLabel::SMOOTH_STOP_5:	return "SmoothStop5";
		case EasingFunctionLabel::SMOOTH_STOP_6:	return "SmoothStop6";
		case EasingFunctionLabel::SMOOTH_STEP_3:	return "SmoothStep3";
		case EasingFunctionLabel::SMOOTH_STEP_5:	return "SmoothStep5";
		case EasingFunctionLabel::HESITATE_3:		return "Hesitate3";
		case EasingFunctionLabel::HESITATE_5:		return "Hesitate5";
		case EasingFunctionLabel::CUSTOM:			return "CustomFunkyEasingFunction";
		default: 									return "";
	}
}


//-----------------------------------------------------------------------------------------------
EasingFunction Game2DCurves::GetEasingFunction() const
{
	switch ( m_easingFunction )
	{
		case EasingFunctionLabel::IDENTITY:			return Identity;
		case EasingFunctionLabel::SMOOTH_START_2:	return SmoothStart2;
		case EasingFunctionLabel::SMOOTH_START_3:	return SmoothStart3;
		case EasingFunctionLabel::SMOOTH_START_4:	return SmoothStart4;
		case EasingFunctionLabel::SMOOTH_START_5:	return SmoothStart5;
		case EasingFunctionLabel::SMOOTH_START_6:	return SmoothStart6;
		case EasingFunctionLabel::SMOOTH_STOP_2:	return SmoothStop2;
		case EasingFunctionLabel::SMOOTH_STOP_3:	return SmoothStop3;
		case EasingFunctionLabel::SMOOTH_STOP_4:	return SmoothStop4;
		case EasingFunctionLabel::SMOOTH_STOP_5:	return SmoothStop5;
		case EasingFunctionLabel::SMOOTH_STOP_6:	return SmoothStop6;
		case EasingFunctionLabel::SMOOTH_STEP_3:	return SmoothStep3;
		case EasingFunctionLabel::SMOOTH_STEP_5:	return SmoothStep5;
		case EasingFunctionLabel::HESITATE_3:		return Hesitate3;
		case EasingFunctionLabel::HESITATE_5:		return Hesitate5;
		case EasingFunctionLabel::CUSTOM:			return CustomFunkyEasingFunction;
		default: 									return nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::Randomize()
{
	GenerateRandomEasingFunctionLabel();
	GenerateRandomCubicBezierCurve();
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::GenerateRandomEasingFunctionLabel()
{
	RandomNumberGenerator rng;
	int randomIndex = rng.RollRandomIntLessThan( ( int ) EasingFunctionLabel::NUM_EASING_FUNCTIONS );
	m_easingFunction = ( EasingFunctionLabel ) randomIndex;
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::GenerateRandomCubicBezierCurve()
{
	RandomNumberGenerator rng;
	m_bezierCurve.m_startPos = m_bezierPanel.GetPointAtUV( Vec2( rng.RollRandomFloatInRange( 0.f, 1.f ), rng.RollRandomFloatInRange( 0.f, 1.f ) ) );
	m_bezierCurve.m_guidePos1 = m_bezierPanel.GetPointAtUV( Vec2( rng.RollRandomFloatInRange( 0.f, 1.f ), rng.RollRandomFloatInRange( 0.f, 1.f ) ) );
	m_bezierCurve.m_guidePos2 = m_bezierPanel.GetPointAtUV( Vec2( rng.RollRandomFloatInRange( 0.f, 1.f ), rng.RollRandomFloatInRange( 0.f, 1.f ) ) );
	m_bezierCurve.m_endPos = m_bezierPanel.GetPointAtUV( Vec2( rng.RollRandomFloatInRange( 0.f, 1.f ), rng.RollRandomFloatInRange( 0.f, 1.f ) ) );
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::RenderEasingFunctionGraph( std::vector<Vertex>& verts ) const
{
	Rgba8 const graphBackgroundColor( 0, 0, 100 );
	Rgba8 const graphLineColor( 0, 255, 0 );
	AddVertsForAABB2D( verts, m_easingFunctionGraphBounds, graphBackgroundColor, Vec2::ZERO, Vec2::ONE );

	float const lineThickness = 0.4f;
	Rgba8 const lineColor( 50, 50, 100 );

	AddVertsForLineSegment2D( verts, Vec2( m_easingFunctionPoint.x, m_easingFunctionGraphBounds.m_mins.y ), m_easingFunctionPoint, 0.3f, lineColor );
	AddVertsForLineSegment2D( verts, Vec2( m_easingFunctionGraphBounds.m_mins.x, m_easingFunctionPoint.y ), m_easingFunctionPoint, 0.3f, lineColor );

	for ( int segmentIndex = 0; segmentIndex < m_numSubdivisions; ++ segmentIndex )
	{
		float t0 = static_cast<float>( segmentIndex ) / static_cast<float>( m_numSubdivisions );
		float t1 = static_cast<float>( segmentIndex + 1 ) / static_cast<float>( m_numSubdivisions );

		EasingFunction easingFunction = GetEasingFunction();
		float y0 = easingFunction( t0 );
		float y1 = easingFunction( t1 );

		Vec2 const p0(
			RangeMap( t0, 0.f, 1.f, m_easingFunctionGraphBounds.m_mins.x, m_easingFunctionGraphBounds.m_maxs.x ),
			RangeMap( y0, 0.f, 1.f, m_easingFunctionGraphBounds.m_mins.y, m_easingFunctionGraphBounds.m_maxs.y ) );

		Vec2 const p1(
			RangeMap( t1, 0.f, 1.f, m_easingFunctionGraphBounds.m_mins.x, m_easingFunctionGraphBounds.m_maxs.x ),
			RangeMap( y1, 0.f, 1.f, m_easingFunctionGraphBounds.m_mins.y, m_easingFunctionGraphBounds.m_maxs.y ) );

		AddVertsForLineSegment2D( verts, p0, p1, lineThickness, Rgba8::GREEN );
	}

	std::string easingFunctionLabel = GetEasingFunctionLabel();
	if ( !easingFunctionLabel.empty() )
	{
		float const labelCellHeight = 3.5f;
		float const labelWidth = GetSimpleTriangleStringWidth( easingFunctionLabel, labelCellHeight );
		Vec2 const labelStartMins(
			m_easingFunctionGraphBounds.GetCenter().x - ( labelWidth * 0.5f ),
			m_easingFunctionGraphBounds.m_mins.y - labelCellHeight - 1.5f );

		AddVertsForTextTriangles2D( verts, easingFunctionLabel, labelStartMins, labelCellHeight, graphLineColor );
	}
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::RenderEasingFunctionPoint( std::vector<Vertex>& verts ) const
{
	float const pointRadius = 0.5f;
	AddVertsForDisc2D( verts, m_easingFunctionPoint, pointRadius, Rgba8::WHITE, 32 );
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::RenderCubicBezierCurve( std::vector<Vertex>& verts ) const
{
	Rgba8 const faintLineColor( 50, 50, 100 );
	float const thinLineThickness = 0.2f;
	AddVertsForLineSegment2D( verts, m_bezierCurve.m_startPos, m_bezierCurve.m_guidePos1, thinLineThickness, faintLineColor );
	AddVertsForLineSegment2D( verts, m_bezierCurve.m_guidePos1, m_bezierCurve.m_guidePos2, thinLineThickness, faintLineColor );
	AddVertsForLineSegment2D( verts, m_bezierCurve.m_guidePos2, m_bezierCurve.m_endPos, thinLineThickness, faintLineColor );

	Rgba8 const darkGrey( 50, 50, 50 );
	float const lineThickness = 0.4f;
	int const fixedSegmentCount = 64;
	for ( int segmentIndex = 0; segmentIndex < fixedSegmentCount; ++ segmentIndex )
	{
		float t0 = static_cast<float>( segmentIndex ) / static_cast<float>( fixedSegmentCount );
		float t1 = static_cast<float>( segmentIndex + 1 ) / static_cast<float>( fixedSegmentCount );
		Vec2 const p0 = m_bezierCurve.EvaluateAtParametric( t0 );
		Vec2 const p1 = m_bezierCurve.EvaluateAtParametric( t1 );
		AddVertsForLineSegment2D( verts, p0, p1, lineThickness, darkGrey );
	}
	for ( int segmentIndex = 0; segmentIndex < m_numSubdivisions; ++ segmentIndex )
	{
		float t0 = static_cast<float>( segmentIndex ) / static_cast<float>( m_numSubdivisions );
		float t1 = static_cast<float>( segmentIndex + 1 ) / static_cast<float>( m_numSubdivisions );
		Vec2 const p0 = m_bezierCurve.EvaluateAtParametric( t0 );
		Vec2 const p1 = m_bezierCurve.EvaluateAtParametric( t1 );
		AddVertsForLineSegment2D( verts, p0, p1, lineThickness, Rgba8::GREEN );
	}

	Rgba8 const lightBlue( 100, 100, 255 );
	float const pointRadius = 0.5f;
	AddVertsForDisc2D( verts, m_bezierCurve.m_startPos, pointRadius, lightBlue, 32 );
	AddVertsForDisc2D( verts, m_bezierCurve.m_guidePos1, pointRadius, lightBlue, 32 );
	AddVertsForDisc2D( verts, m_bezierCurve.m_guidePos2, pointRadius, lightBlue, 32 );
	AddVertsForDisc2D( verts, m_bezierCurve.m_endPos, pointRadius, lightBlue, 32 );
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::RenderCubicBezierPoint( std::vector<Vertex>& verts ) const
{
	float const pointRadius = 0.5f;
	float const clampedT = GetClampedZeroToOne( m_parametricT );

	Vec2 const whitePoint = m_bezierCurve.EvaluateAtParametric( clampedT );
	AddVertsForDisc2D( verts, whitePoint, pointRadius, Rgba8::WHITE, 32 );

	int const numSubdivisions = ( m_numSubdivisions > 0 ) ? m_numSubdivisions : 64;
	float const totalLength = m_bezierCurve.GetApproximateLength( numSubdivisions );

	Vec2 greenPoint = m_bezierCurve.m_startPos;
	if ( totalLength > 0.f )
	{
		float const distanceAlongCurve = clampedT * totalLength;
		greenPoint = m_bezierCurve.EvaluateAtApproximateDistance( distanceAlongCurve, numSubdivisions );
	}

	AddVertsForDisc2D( verts, greenPoint, pointRadius, Rgba8::GREEN, 32 );
}


//-----------------------------------------------------------------------------------------------
float Game2DCurves::CustomFunkyEasingFunction( float t )
{
	float pi = 3.1415926f;
	return t + sinf( 6.f * pi * t ) * t * ( 1.f - t );
}