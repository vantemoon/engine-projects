#pragma once
#include "Game/Game.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/CubicBezierCurve2D.hpp"
#include "Engine/Core/Vertex.hpp"
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
typedef float ( *EasingFunction )( float t );


//-----------------------------------------------------------------------------------------------
enum class EasingFunctionLabel
{
	IDENTITY,
	SMOOTH_START_2,
	SMOOTH_START_3,
	SMOOTH_START_4,
	SMOOTH_START_5,
	SMOOTH_START_6,
	SMOOTH_STOP_2,
	SMOOTH_STOP_3,
	SMOOTH_STOP_4,
	SMOOTH_STOP_5,
	SMOOTH_STOP_6,
	SMOOTH_STEP_3,
	SMOOTH_STEP_5,
	HESITATE_3,
	HESITATE_5,
	CUSTOM,
	NUM_EASING_FUNCTIONS
};


//-----------------------------------------------------------------------------------------------
class Game2DCurves : public Game
{
public:
	Game2DCurves();
	~Game2DCurves();

	void Update( float deltaSeconds) override;
	void UpdateFromKeyboard( float deltaSeconds );
	void Render() const override;

private:
	std::string GetEasingFunctionLabel() const;
	EasingFunction GetEasingFunction() const;
	void RenderEasingFunctionGraph( std::vector<Vertex>& verts ) const;
	void RenderEasingFunctionPoint( std::vector<Vertex>& verts ) const;
	static float CustomFunkyEasingFunction( float t );

private:
	int m_numSubdivisions = 64;
	bool m_isDebugDraw = false;
	float m_parametricT = 0.f;

	AABB2 m_splinePanel = AABB2::ZERO_TO_ONE;
	AABB2 m_bezierPanel = AABB2::ZERO_TO_ONE;
	AABB2 m_easingPanel = AABB2::ZERO_TO_ONE;

	// Easing function
	AABB2 m_easingFunctionGraphBounds = AABB2::ZERO_TO_ONE;
	EasingFunctionLabel m_easingFunction = EasingFunctionLabel::SMOOTH_STOP_2;
	Vec2 m_easingFunctionPoint = Vec2::ZERO;

	// Cubic Bezier curve
	CubicBezierCurve2D m_bezierCurve = CubicBezierCurve2D( Vec2::ZERO, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO );
};