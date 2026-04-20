#pragma once
#include "Game/Game.hpp"
#include "Engine/Math/AABB2.hpp"
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
	static float CustomFunkyEasingFunction( float t );

private:
	int m_numCurveSegments = 64;
	bool m_isDebugDraw = false;

	AABB2 m_splinePanel = AABB2::ZERO_TO_ONE;
	AABB2 m_bezierPanel = AABB2::ZERO_TO_ONE;
	AABB2 m_easingPanel = AABB2::ZERO_TO_ONE;

	// Easing function
	AABB2 m_easingFunctionGraphBounds = AABB2::ZERO_TO_ONE;
	EasingFunctionLabel m_easingFunction = EasingFunctionLabel::IDENTITY;
};