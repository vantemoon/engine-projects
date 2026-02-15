#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Rgba8.hpp"


//-----------------------------------------------------------------------------------------------
class ImpactWave : public Entity
{
public:
	static constexpr int   NUM_SEGMENTS = 48;
	float m_ageSeconds = 0.0f;
	float m_lifeSeconds;

	float m_startRadius;
	float m_endRadius;
	float m_startThick;
	float m_endThick;

	Rgba8 m_startColour = Rgba8( 255, 255, 255, 220 );
	Rgba8 m_endColour = Rgba8( 255, 255, 255, 0 );

public:
	ImpactWave( Game* game, Vec2 const& center, float lifeSeconds, float startRadius, float endRadius, float startThickness, float endThickness, 
		        Rgba8 const& startColour, Rgba8 const& endColour );
	~ImpactWave() override = default;
	void Update() override;
	void Render() const override;
};