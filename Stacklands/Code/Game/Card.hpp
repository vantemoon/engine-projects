#pragma once
#include "Game/CardDefinition.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <string>


//-----------------------------------------------------------------------------------------------
class Card
{
public:
	Card( Vec2 const& position, CardDefinition const* definition );

	void Update( float deltaSeconds );
	void Render() const;

	Vec2 GetPosition() const;
	void SetPosition( Vec2 const& position );

	AABB2 GetBounds() const;

private:
	CardDefinition const* m_definition = nullptr;

	Vec2 m_position = Vec2::ZERO;
	Vec2 m_size = Vec2( 12.f, 14.5f );
};