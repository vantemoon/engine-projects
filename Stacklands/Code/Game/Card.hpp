#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include <string>


//-----------------------------------------------------------------------------------------------
class CardDefinition;


//-----------------------------------------------------------------------------------------------
class Card
{
public:
	Card( CardDefinition const* definition, Vec2 const& position );
	~Card();

	void Update();
	void Render() const;

	Vec2 GetPosition() const;
	Vec2 GetNextPosition() const;
	void SetPosition( Vec2 const& position );

	AABB2 GetBounds() const;
	bool IsPointInside( Vec2 const& point ) const;

	void SetIsSelected( bool isSelected );
	bool IsSelected() const;

	CardDefinition const* GetDefinition() const;

private:
	CardDefinition const* m_definition = nullptr;

	Vec2 m_position = Vec2::ZERO;
	Vec2 m_nextPosition = Vec2::ZERO;
	Vec2 m_size = Vec2( 12.f, 14.5f );

	bool m_isSelected = false;

	Rgba8 m_tint = Rgba8::WHITE;
	Rgba8 m_highlightTint = Rgba8( 255, 245, 200, 255 );
};