#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Math/AABB2.hpp"


//------------------------------------------------------------------------------------------------
BitmapFont::BitmapFont( char const* fontFilePathNameWithNoExtension, Texture& fontTexture )
	: m_fontFilePathNameWithNoExtension( fontFilePathNameWithNoExtension )
	, m_fontGlyphsSpriteSheet( fontTexture, IntVec2( 16, 16 ) ) // Default to 16x16 grid of glyphs
{
	IntVec2 textureDimensions = fontTexture.GetDimensions();
	m_fontDefaultAspect = static_cast<float>( textureDimensions.x ) / static_cast<float>( textureDimensions.y );
}


//------------------------------------------------------------------------------------------------
Texture& BitmapFont::GetTexture()
{
	return m_fontGlyphsSpriteSheet.GetTexture();
}


//------------------------------------------------------------------------------------------------
float BitmapFont::GetTextWidth( float cellHeight, std::string const& text, float cellAspectScale /*= 1.f */ )
{
	float totalWidth = 0.f;
	for ( char const& glyphChar : text )
	{
		int glyphUnicode = static_cast<unsigned char>( glyphChar );
		float glyphAspect = GetGlyphAspect( glyphUnicode );
		float glyphWidth = cellHeight * glyphAspect * cellAspectScale;
		totalWidth += glyphWidth;
	}
	return totalWidth;
}


//------------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForText2D( std::vector<Vertex>& vertexArray, Vec2 textMins, float cellHeight,
									std::string const& text, Rgba8 tint /*= Rgba8::WHITE*/, float cellAspectScale /*= 1.f */ )
{
	Vec2 penPosition = textMins;
	for ( char const& glyphChar : text )
	{
		int glyphUnicode = static_cast<unsigned char>( glyphChar );
		float glyphAspect = GetGlyphAspect( glyphUnicode );
		float glyphWidth = cellHeight * glyphAspect * cellAspectScale;

		SpriteDefinition const& glyphSpriteDef = m_fontGlyphsSpriteSheet.GetSpriteDefinition( glyphUnicode );
		AABB2 glyphUVs = glyphSpriteDef.GetUVs();
		Vec2 glyphMins = penPosition;
		Vec2 glyphMaxs = penPosition + Vec2( glyphWidth, cellHeight );

		// Triangle A
		vertexArray.push_back( Vertex( Vec3( glyphMins.x, glyphMins.y, 0.f ), tint, Vec2( glyphUVs.m_mins.x, glyphUVs.m_mins.y ) ) );
		vertexArray.push_back( Vertex( Vec3( glyphMaxs.x, glyphMins.y, 0.f ), tint, Vec2( glyphUVs.m_maxs.x, glyphUVs.m_mins.y ) ) );
		vertexArray.push_back( Vertex( Vec3( glyphMaxs.x, glyphMaxs.y, 0.f ), tint, Vec2( glyphUVs.m_maxs.x, glyphUVs.m_maxs.y ) ) );
		
		// Triangle B
		vertexArray.push_back( Vertex( Vec3( glyphMaxs.x, glyphMaxs.y, 0.f ), tint, Vec2( glyphUVs.m_maxs.x, glyphUVs.m_maxs.y ) ) );
		vertexArray.push_back( Vertex( Vec3( glyphMins.x, glyphMaxs.y, 0.f ), tint, Vec2( glyphUVs.m_mins.x, glyphUVs.m_maxs.y ) ) );
		vertexArray.push_back( Vertex( Vec3( glyphMins.x, glyphMins.y, 0.f ), tint, Vec2( glyphUVs.m_mins.x, glyphUVs.m_mins.y ) ) );

		penPosition.x += glyphWidth;
	}
}


//------------------------------------------------------------------------------------------------
float BitmapFont::GetGlyphAspect( [[maybe_unused]] int glyphUnicode ) const
{
	return m_fontDefaultAspect;
}