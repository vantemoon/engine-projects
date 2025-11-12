#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"


//-----------------------------------------------------------------------------------------------
SpriteSheet::SpriteSheet( Texture& texture, IntVec2 const& simpleGridLayout )
	: m_texture( texture )
{
	int numSpritesWide = simpleGridLayout.x;
	int numSpritesHigh = simpleGridLayout.y;
	int totalSprites = numSpritesWide * numSpritesHigh;

	for ( int spriteIndex = 0; spriteIndex < totalSprites; ++spriteIndex )
	{
		int spriteX = spriteIndex % numSpritesWide;
		int spriteY = spriteIndex / numSpritesWide;

		float uvMinX = static_cast< float >( spriteX ) / static_cast< float >( numSpritesWide );
		float uvMinY = static_cast< float >( ( numSpritesHigh - 1 ) - spriteY ) / static_cast< float >( numSpritesHigh );
		float uvMaxX = static_cast< float >( spriteX + 1 ) / static_cast< float >( numSpritesWide );
		float uvMaxY = static_cast< float >( ( numSpritesHigh - 1 ) - spriteY + 1 ) / static_cast< float >( numSpritesHigh );

		Vec2 uvAtMins( uvMinX, uvMinY );
		Vec2 uvAtMaxs( uvMaxX, uvMaxY );

		SpriteDefinition spriteDef( *this, spriteIndex, uvAtMins, uvAtMaxs );
		m_spriteDefinitions.push_back( spriteDef );
	}
}


//-----------------------------------------------------------------------------------------------
Texture& SpriteSheet::GetTexture() const
{
	return m_texture;
}


//-----------------------------------------------------------------------------------------------
int SpriteSheet::GetNumSprites() const
{
	return static_cast<int>( m_spriteDefinitions.size() );
}


//-----------------------------------------------------------------------------------------------
SpriteDefinition const& SpriteSheet::GetSpriteDefinition( int spriteIndex ) const
{
	return m_spriteDefinitions[spriteIndex];
}


//-----------------------------------------------------------------------------------------------
void SpriteSheet::GetSpriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex ) const
{
	m_spriteDefinitions[spriteIndex].GetUVs( out_uvAtMins, out_uvAtMaxs );
}


//-----------------------------------------------------------------------------------------------
AABB2 SpriteSheet::GetSpriteUVs( int spriteIndex ) const
{
	return m_spriteDefinitions[spriteIndex].GetUVs();
}