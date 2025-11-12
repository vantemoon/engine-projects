#pragma once
#include "Engine/Renderer/Texture.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class SpriteDefinition;
class Texture;
struct AABB2;
struct IntVec2;
struct Vec2;

//-----------------------------------------------------------------------------------------------
class SpriteSheet
{
public:
	explicit SpriteSheet( Texture& texture, IntVec2 const& simpleGridLayout );

	Texture&						GetTexture()																const;
	int								GetNumSprites()																const;
	SpriteDefinition const&			GetSpriteDefinition( int spriteIndex )										const;
	void							GetSpriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex )		const;
	AABB2							GetSpriteUVs( int spriteIndex )												const;

protected:
	Texture&						m_texture; // Reference member must be set in constructor's initializer list
	std::vector<SpriteDefinition>	m_spriteDefinitions;
};