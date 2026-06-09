#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <string>


//-----------------------------------------------------------------------------------------------
class Card
{
public:
	Card( Vec2 const& position, std::string const& name );

	void Update( float deltaSeconds );
	void Render() const;

	Vec2 GetPosition() const;
	void SetPosition( Vec2 const& position );

	AABB2 GetBounds() const;

private:
	Vec2 m_position = Vec2::ZERO;
	Vec2 m_size = Vec2( 10.f, 14.f );

	std::string m_name = "Card";

	Rgba8 m_bodyTint = Rgba8( 245, 235, 210, 255 );
	Rgba8 m_borderTint = Rgba8( 40, 35, 30, 255 );
};