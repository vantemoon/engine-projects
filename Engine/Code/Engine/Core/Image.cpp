#define STB_IMAGE_IMPLEMENTATION // Exactly one .CPP (this Image.cpp) should #define this before #including stb_image.h
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/Image.hpp"


//-----------------------------------------------------------------------------------------------
Image::Image( char const* imageFilePath )
	: m_imageFilePath( imageFilePath )
{
	int numComponents = 0;
	unsigned char* texelData = stbi_load( imageFilePath, &m_dimensions.x, &m_dimensions.y, &numComponents, STBI_rgb_alpha );
	if ( texelData == nullptr )
	{
		m_dimensions = IntVec2( 0, 0 );
		m_rgbaTexels = nullptr;
		return;
	}
	int totalTexels = m_dimensions.x * m_dimensions.y;
	m_rgbaTexels = new Rgba8[totalTexels];
	memcpy( m_rgbaTexels, texelData, totalTexels * sizeof( Rgba8 ) );
	stbi_image_free( texelData );
}


//-----------------------------------------------------------------------------------------------
std::string const& Image::GetImageFilePath() const
{
	return m_imageFilePath;
}


//-----------------------------------------------------------------------------------------------
IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}


//-----------------------------------------------------------------------------------------------
Rgba8 Image::GetTexelColor( IntVec2 const& texelCoords ) const
{
	if ( m_rgbaTexels == nullptr )
	{
		return Rgba8::BLACK;
	}
	int index = texelCoords.y * m_dimensions.x + texelCoords.x;
	return m_rgbaTexels[index];
}


//-----------------------------------------------------------------------------------------------
void Image::SetTexelColor( IntVec2 const& texelCoords, Rgba8 const& newColor )
{
	if ( m_rgbaTexels == nullptr )
	{
		return;
	}
	int index = texelCoords.y * m_dimensions.x + texelCoords.x;
	m_rgbaTexels[index] = newColor;
}