#pragma once
#include "Game/Entity.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"


//-----------------------------------------------------------------------------------------------
class Explosion : public Entity
{
public:
	float m_duration = 0.5f;
	float m_age = 0.f;
	float m_size = 1.f;

	SpriteSheet*           m_explosionSpriteSheet = nullptr;
	SpriteAnimDefinition*  m_explosionAnimDef = nullptr;

public:
	Explosion( Vec2 const& position, float orientationDegrees, float lifetime, float size );
	~Explosion();
	void Update( float deltaSeconds )								  override;
	void Render()												const override;

protected:
	void InitializeVertexArray()									  override;
};