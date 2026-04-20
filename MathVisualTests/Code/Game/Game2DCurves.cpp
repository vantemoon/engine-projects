#include "Game/Game2DCurves.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Easing.hpp"
#include "Engine/Math/MathUtils.hpp"
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
}


//-----------------------------------------------------------------------------------------------
Game2DCurves::~Game2DCurves()
{
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::Update( float deltaSeconds )
{
	UpdateFromKeyboard( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Game2DCurves::UpdateFromKeyboard( [[maybe_unused]] float deltaSeconds )
{
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_F1 ) )
	{
		m_isDebugDraw = !m_isDebugDraw;
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
	Rgba8 const graphBackgroundColor( 0, 0, 100 );
	Rgba8 const graphLineColor( 0, 255, 0 );
	AddVertsForAABB2D( verts, m_easingFunctionGraphBounds, graphBackgroundColor, Vec2::ZERO, Vec2::ONE );

	std::string easingFunctionLabel = GetEasingFunctionLabel();
	RenderEasingFunctionGraph( verts );
	
	if ( !easingFunctionLabel.empty() )
	{
		float const labelCellHeight = 3.5f;
		float const labelWidth = GetSimpleTriangleStringWidth( easingFunctionLabel, labelCellHeight );
		Vec2 const labelStartMins(
			m_easingFunctionGraphBounds.GetCenter().x - ( labelWidth * 0.5f ),
			m_easingFunctionGraphBounds.m_mins.y - labelCellHeight - 1.5f );

		AddVertsForTextTriangles2D( verts, easingFunctionLabel, labelStartMins, labelCellHeight, graphLineColor );
	}

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
void Game2DCurves::RenderEasingFunctionGraph( std::vector<Vertex>& verts ) const
{
	Rgba8 const curveColor( 0, 255, 0 );
	float const lineThickness = 0.2f;

	for ( int segmentIndex = 0; segmentIndex < m_numCurveSegments; ++ segmentIndex )
	{
		float t0 = static_cast<float>( segmentIndex ) / static_cast<float>( m_numCurveSegments );
		float t1 = static_cast<float>( segmentIndex + 1 ) / static_cast<float>( m_numCurveSegments );

		EasingFunction easingFunction = GetEasingFunction();
		float y0 = easingFunction( t0 );
		float y1 = easingFunction( t1 );

		Vec2 const p0(
			RangeMap( t0, 0.f, 1.f, m_easingFunctionGraphBounds.m_mins.x, m_easingFunctionGraphBounds.m_maxs.x ),
			RangeMap( y0, 0.f, 1.f, m_easingFunctionGraphBounds.m_mins.y, m_easingFunctionGraphBounds.m_maxs.y ) );

		Vec2 const p1(
			RangeMap( t1, 0.f, 1.f, m_easingFunctionGraphBounds.m_mins.x, m_easingFunctionGraphBounds.m_maxs.x ),
			RangeMap( y1, 0.f, 1.f, m_easingFunctionGraphBounds.m_mins.y, m_easingFunctionGraphBounds.m_maxs.y ) );

		AddVertsForLineSegment2D( verts, p0, p1, lineThickness, curveColor );
	}
}


//-----------------------------------------------------------------------------------------------
float Game2DCurves::CustomFunkyEasingFunction( float t )
{
	return t;
}