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
									std::string const& text, Rgba8 const& tint, float cellAspectScale /*= 1.f */ )
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
void BitmapFont::AddVertsForTextInBox2D( std::vector<Vertex>& verts, std::string const& text, AABB2 const& box, float cellHeight,
										 Rgba8 tint /*= Rgba8::WHITE*/, float cellAspectScale /*= 1.f */, Vec2 alignment /*= Vec2( .5f, .5f ) */,
										 TextBoxMode mode /*= TextBoxMode::SHRINK_TO_FIT*/, int maxGlyphsToDraw /*= 99999999 */ )
{
	int numOfGlyphsOnCurrentLine = 0;
	int maxNumOfGlyphsOnOneLine = 0;
	int numOfLines = 1;

	std::vector<int> glyphsPerLine;

	for ( char const& glyphChar : text )
	{	
		if ( glyphChar == '\n' )
		{
			if ( numOfGlyphsOnCurrentLine > maxNumOfGlyphsOnOneLine )
			{
				maxNumOfGlyphsOnOneLine = numOfGlyphsOnCurrentLine;
			}
			glyphsPerLine.push_back( numOfGlyphsOnCurrentLine );
			numOfGlyphsOnCurrentLine = 0;
			numOfLines++;
			continue;
		}
		numOfGlyphsOnCurrentLine++;
	}
	glyphsPerLine.push_back( numOfGlyphsOnCurrentLine );
	if ( numOfGlyphsOnCurrentLine > maxNumOfGlyphsOnOneLine )
	{
		maxNumOfGlyphsOnOneLine = numOfGlyphsOnCurrentLine;
	}

	Vec2 textMins;
	float scale = 1.f;
	float adjustedCellHeight = cellHeight;
	float adjustedTextHeight = 0.f;
	float adjustedTextWidth = 0.f;
	if ( mode == SHRINK_TO_FIT )
	{
		float widthScale = 1.f;
		float heightScale = 1.f;

		float totalTextWidth = GetTextWidth( cellHeight, std::string( maxNumOfGlyphsOnOneLine, 'A' ), cellAspectScale );
		if ( totalTextWidth > box.GetDimensions().x )
		{
			widthScale = box.GetDimensions().x / totalTextWidth;
		}
		float totalTextHeight = cellHeight * numOfLines;
		if ( totalTextHeight > box.GetDimensions().y )
		{
			heightScale = box.GetDimensions().y / totalTextHeight;
		}
		if ( widthScale != 1.f || heightScale != 1.f ) scale = std::min( widthScale, heightScale );

		adjustedCellHeight = cellHeight * scale;
		adjustedTextHeight = adjustedCellHeight * static_cast< float >( numOfLines );
		adjustedTextWidth = GetTextWidth( cellHeight, std::string( maxNumOfGlyphsOnOneLine, 'A' ), cellAspectScale ) * scale;

		textMins.x = box.m_mins.x + ( box.GetDimensions().x - adjustedTextWidth ) * alignment.x;
		textMins.y = box.m_mins.y + ( box.GetDimensions().y - adjustedTextHeight ) * alignment.y + adjustedCellHeight * ( numOfLines - 1 );
	}
	else // OVERRUN
	{
		adjustedCellHeight = cellHeight;
		adjustedTextHeight = adjustedCellHeight * static_cast< float >( numOfLines );
		adjustedTextWidth = GetTextWidth( cellHeight, std::string( maxNumOfGlyphsOnOneLine, 'A' ), cellAspectScale );
		textMins.x = box.m_mins.x - ( adjustedTextWidth - box.GetDimensions().x ) * alignment.x;
		textMins.y = box.m_mins.y - ( adjustedTextHeight - box.GetDimensions().y ) * alignment.y + adjustedCellHeight * ( numOfLines - 1 );
	}

	Vec2 penPosition = textMins;
	int glyphsDrawn = 0;
	int currentLineIndex = 0;
	bool isAtLineStart = true;
	for ( char const& glyphChar : text )
	{
		if ( glyphsDrawn >= maxGlyphsToDraw )
		{
			break;
		}
		int glyphUnicode = static_cast<unsigned char>( glyphChar );

		if ( glyphUnicode == '\n' )
		{
			penPosition.x = textMins.x;
			penPosition.y -= adjustedCellHeight;
			currentLineIndex++;
			isAtLineStart = true;
			continue;
		}

		float glyphAspect = GetGlyphAspect( glyphUnicode );
		float glyphWidth = adjustedCellHeight * glyphAspect * cellAspectScale;
		if ( isAtLineStart )
		{
			float spaceOnTheLeft = ( maxNumOfGlyphsOnOneLine - glyphsPerLine[currentLineIndex] ) * glyphWidth * alignment.x;
			penPosition.x += spaceOnTheLeft;
			isAtLineStart = false;
		}

		SpriteDefinition const& glyphSpriteDef = m_fontGlyphsSpriteSheet.GetSpriteDefinition( glyphUnicode );
		AABB2 glyphUVs = glyphSpriteDef.GetUVs();
		Vec2 glyphMins = penPosition;
		Vec2 glyphMaxs = penPosition + Vec2( glyphWidth, adjustedCellHeight );

		// Triangle A
		verts.push_back( Vertex( Vec3( glyphMins.x, glyphMins.y, 0.f ), tint, Vec2( glyphUVs.m_mins.x, glyphUVs.m_mins.y ) ) );
		verts.push_back( Vertex( Vec3( glyphMaxs.x, glyphMins.y, 0.f ), tint, Vec2( glyphUVs.m_maxs.x, glyphUVs.m_mins.y ) ) );
		verts.push_back( Vertex( Vec3( glyphMaxs.x, glyphMaxs.y, 0.f ), tint, Vec2( glyphUVs.m_maxs.x, glyphUVs.m_maxs.y ) ) );

		// Triangle B
		verts.push_back( Vertex( Vec3( glyphMaxs.x, glyphMaxs.y, 0.f ), tint, Vec2( glyphUVs.m_maxs.x, glyphUVs.m_maxs.y ) ) );
		verts.push_back( Vertex( Vec3( glyphMins.x, glyphMaxs.y, 0.f ), tint, Vec2( glyphUVs.m_mins.x, glyphUVs.m_maxs.y ) ) );
		verts.push_back( Vertex( Vec3( glyphMins.x, glyphMins.y, 0.f ), tint, Vec2( glyphUVs.m_mins.x, glyphUVs.m_mins.y ) ) );

		penPosition.x += glyphWidth;
		glyphsDrawn++;
	}
}


//------------------------------------------------------------------------------------------------
float BitmapFont::GetGlyphAspect( [[maybe_unused]] int glyphUnicode ) const
{
	return m_fontDefaultAspect;
}


//------------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForText3DAtOriginXForward( std::vector<Vertex>& verts, float cellHeight, std::string const& text,
													Rgba8 const& tint, float cellAspectScale, Vec2 alignment, int maxGlyphsToDraw )
{
	if ( cellHeight <= 0.f || text.empty() || maxGlyphsToDraw <= 0 )
	{
		return;
	}

	std::string textToDraw;
	int glyphsDrawn = 0;
	for ( char const& glyphChar : text )
	{
		if ( glyphsDrawn >= maxGlyphsToDraw )
		{
			break;
		}

		textToDraw.push_back( glyphChar );
		glyphsDrawn++;
	}

	float textWidth = GetTextWidth( cellHeight, textToDraw, cellAspectScale );
	Vec2 textMins( -textWidth * alignment.x, -cellHeight * alignment.y );

	std::vector<Vertex> textVerts2D;
	AddVertsForText2D( textVerts2D, textMins, cellHeight, textToDraw, tint, cellAspectScale );

	for ( Vertex const& sourceVert : textVerts2D )
	{
		Vec3 const& p = sourceVert.m_position;
		verts.push_back( Vertex( Vec3( 0.f, p.x, p.y ), sourceVert.m_color, sourceVert.m_uvTexCoords ) );
	}
}