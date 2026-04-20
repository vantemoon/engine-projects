#pragma once
#include "Game/Game.hpp"
#include "Engine/Math/AABB2.hpp"


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
	AABB2 m_splinePanel = AABB2::ZERO_TO_ONE;
	AABB2 m_bezierPanel = AABB2::ZERO_TO_ONE;
	AABB2 m_easingPanel = AABB2::ZERO_TO_ONE;
};