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
	UpdateFromKeyboard( deltaSeconds );

	if ( m_isSlowMo )
	{
		deltaSeconds *= 0.1f;
	}

	m_parametricT += deltaSeconds * 0.5f;
	if ( m_parametricT > 1.f )
	{
		m_parametricT -= 1.f;
	}

	int const numSplineSections = std::max( 1, static_cast<int>( m_hermiteSpline.m_controlPoints.size() ) - 1 );
	float const splineLoopDurationSeconds = 2.f * static_cast<float>( numSplineSections );
	float const splineParametricRate = 1.f / splineLoopDurationSeconds;

	m_splineParametricT += deltaSeconds * splineParametricRate;
	if ( m_splineParametricT > 1.f )
	{
		m_splineParametricT -= 1.f;
	}

	// Easing function point
	float easedT = GetEasingFunction()( m_parametricT );
	m_easingFunctionPoint = m_easingFunctionGraphBounds.m_mins + ( Vec2( m_parametricT, easedT ) * m_easingFunctionGraphBounds.GetDimensions() );
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

	if ( g_engine->m_inputSystem->IsKeyDown( 'T' ) )
	{
		m_isSlowMo = true;
	}
	else
	{
		m_isSlowMo = false;
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

	if ( g_engine->m_inputSystem->WasKeyJustPressed( 'N' ) )
	{
		m_numSubdivisionsPower = std::max( 0, m_numSubdivisionsPower - 1 );
	}
	if ( g_engine->m_inputSystem->WasKeyJustPressed( 'M' ) )
	{
		m_numSubdivisionsPower =  m_numSubdivisionsPower + 1;
	}

	m_numSubdivisions = 1 << m_numSubdivisionsPower;
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
	RenderCubicBezierPoints( verts );

	// Cubic Hermite spline
	RenderCubicHermiteSpline( verts );
	RenderCubicHermiteSplinePoints( verts );

	g_engine->m_renderer->BindTexture( nullptr );
	g_engine->m_renderer->DrawVertexArray( verts );

	g_engine->m_renderer->EndCamera( *m_worldCamera );
}


//-----------------------------------------------------------------------------------------------
int Game2DCurves::GetNumSubdivisions() const
{
	return m_numSubdivisions;
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
	GenerateRandomCubicHermiteSpline();
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
void Game2DCurves::GenerateRandomCubicHermiteSpline()
{
	RandomNumberGenerator rng;
	std::vector<Vec2> controlPoints;

	int const numControlPoints = rng.RollRandomIntInRange( 4, 10 );
	controlPoints.reserve( numControlPoints );

	float const minX = m_splinePanel.m_mins.x;
	float const maxX = m_splinePanel.m_maxs.x;
	float const minY = m_splinePanel.m_mins.y;
	float const maxY = m_splinePanel.m_maxs.y;

	float const panelWidth = maxX - minX;
	float const xStep = ( numControlPoints > 1 ) ? panelWidth / static_cast< float >( numControlPoints - 1 ) : 0.f;

	float const xJitterRange = xStep * 0.6f;

	for ( int controlPointIndex = 0; controlPointIndex < numControlPoints; ++controlPointIndex )
	{
		float const baseX = minX + ( xStep * static_cast< float >( controlPointIndex ) );
		float x = baseX + rng.RollRandomFloatInRange( -xJitterRange, xJitterRange );
		float y = rng.RollRandomFloatInRange( minY, maxY );

		if ( x < minX ) x = minX;
		if ( x > maxX ) x = maxX;

		controlPoints.push_back( Vec2( x, y ) );
	}

	m_hermiteSpline = CubicHermiteSpline2D( controlPoints );
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
	float const pointRadius = 0.6f;
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
void Game2DCurves::RenderCubicBezierPoints( std::vector<Vertex>& verts ) const
{
	float const pointRadius = 0.6f;
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
void Game2DCurves::RenderCubicHermiteSpline( std::vector<Vertex>& verts ) const
{
	Rgba8 const faintLineColor( 50, 50, 100 );
	float const thinLineThickness = 0.2f;
	for ( int segmentIndex = 0; segmentIndex < m_hermiteSpline.m_controlPoints.size() - 1; ++ segmentIndex )
	{
		Vec2 const& p0 = m_hermiteSpline.m_controlPoints[segmentIndex];
		Vec2 const& p1 = m_hermiteSpline.m_controlPoints[segmentIndex + 1];
		AddVertsForLineSegment2D( verts, p0, p1, thinLineThickness, faintLineColor );
	}

	Rgba8 const darkGrey( 50, 50, 50 );
	float const lineThickness = 0.4f;
	int const fixedSegmentCount = 64;
	for ( int segmentIndex = 0; segmentIndex < fixedSegmentCount; ++ segmentIndex )
	{
		float t0 = static_cast< float >( segmentIndex ) / static_cast< float >( fixedSegmentCount );
		float t1 = static_cast< float >( segmentIndex + 1 ) / static_cast< float >( fixedSegmentCount );
		Vec2 const p0 = m_hermiteSpline.EvaluateAtParametric( t0 );
		Vec2 const p1 = m_hermiteSpline.EvaluateAtParametric( t1 );
		AddVertsForLineSegment2D( verts, p0, p1, lineThickness, darkGrey );
	}

	int const numSplineSections = std::max( 1, static_cast< int >( m_hermiteSpline.m_controlPoints.size() ) - 1 );
	int const totalSplineSubdivisions = m_numSubdivisions * numSplineSections;

	for ( int segmentIndex = 0; segmentIndex < totalSplineSubdivisions; ++ segmentIndex )
	{
		float t0 = static_cast< float >( segmentIndex ) / static_cast< float >( totalSplineSubdivisions );
		float t1 = static_cast< float >( segmentIndex + 1 ) / static_cast< float >( totalSplineSubdivisions );
		Vec2 const p0 = m_hermiteSpline.EvaluateAtParametric( t0 );
		Vec2 const p1 = m_hermiteSpline.EvaluateAtParametric( t1 );
		AddVertsForLineSegment2D( verts, p0, p1, lineThickness, Rgba8::GREEN );
	}

	for ( int tangentIndex = 0; tangentIndex < m_hermiteSpline.m_tangents.size(); ++ tangentIndex )
	{
		Vec2 const& controlPoint = m_hermiteSpline.m_controlPoints[tangentIndex];
		Vec2 const& tangent = m_hermiteSpline.m_tangents[tangentIndex];
		if ( tangent.GetLengthSquared() > 0.f )
		{
			Vec2 const tangentEnd = controlPoint + tangent;
			AddVertsForArrow2D( verts, controlPoint, tangentEnd, 0.4f, 0.5f, Rgba8::RED );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::RenderCubicHermiteSplinePoints( std::vector<Vertex>& verts ) const
{
	Rgba8 const lightBlue( 100, 100, 255 );
	float const pointRadius = 0.6f;
	for ( int controlPointIndex = 0; controlPointIndex < m_hermiteSpline.m_controlPoints.size(); ++ controlPointIndex )
	{
		Vec2 const& controlPoint = m_hermiteSpline.m_controlPoints[controlPointIndex];
		AddVertsForDisc2D( verts, controlPoint, pointRadius, lightBlue, 32 );
	}

	float const splineT = GetClampedZeroToOne( m_splineParametricT );
	Vec2 const whitePoint = m_hermiteSpline.EvaluateAtParametric( splineT );
	AddVertsForDisc2D( verts, whitePoint, pointRadius, Rgba8::WHITE, 32 );

	int const numSubdivisions = ( m_numSubdivisions > 0 ) ? m_numSubdivisions : 64;
	float const totalLength = m_hermiteSpline.GetApproximateLength( numSubdivisions );

	Vec2 greenPoint = m_hermiteSpline.m_controlPoints[0];
	if ( totalLength > 0.f )
	{
		float const distanceAlongCurve = splineT * totalLength;
		greenPoint = m_hermiteSpline.EvaluateAtApproximateDistance( distanceAlongCurve, numSubdivisions );
	}

	AddVertsForDisc2D( verts, greenPoint, pointRadius, Rgba8::GREEN, 32 );
}


//-----------------------------------------------------------------------------------------------
float Game2DCurves::CustomFunkyEasingFunction( float t )
{
	float pi = 3.1415926f;
	return t + sinf( 6.f * pi * t ) * t * ( 1.f - t );
}