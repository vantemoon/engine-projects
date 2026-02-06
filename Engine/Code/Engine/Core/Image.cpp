#define STB_IMAGE_IMPLEMENTATION // Exactly one .CPP (this Image.cpp) should #define this before #including stb_image.h
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/Image.hpp"


//-----------------------------------------------------------------------------------------------
Image::Image()
	: m_imageFilePath( "" )
	, m_dimensions( IntVec2( 0, 0 ) )
	, m_texelRgba8Data()
{
}


//-----------------------------------------------------------------------------------------------
Image::~Image()
{
}


//-----------------------------------------------------------------------------------------------
Image::Image( char const* imageFilePath )
	: m_imageFilePath( imageFilePath )
{
	int numComponents = 0;
	stbi_set_flip_vertically_on_load( 1 );
	unsigned char* texelData = stbi_load( imageFilePath, &m_dimensions.x, &m_dimensions.y, &numComponents, STBI_rgb_alpha );
	if ( texelData == nullptr )
	{
		m_dimensions = IntVec2( 0, 0 );
		m_texelRgba8Data.clear();
		return;
	}
	int totalTexels = m_dimensions.x * m_dimensions.y;
	m_texelRgba8Data.resize( totalTexels );
	memcpy( m_texelRgba8Data.data(), texelData, totalTexels * sizeof( Rgba8 ) );
	stbi_image_free( texelData );
}


//-----------------------------------------------------------------------------------------------
Image::Image( IntVec2 size, Rgba8 color )
	: m_imageFilePath( "" )
	, m_dimensions( size )
{
	int totalTexels = m_dimensions.x * m_dimensions.y;
	m_texelRgba8Data.resize( totalTexels, color );
	for ( int texelIndex = 0; texelIndex < totalTexels; ++texelIndex )
	{
		m_texelRgba8Data[texelIndex] = color;
	}
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
	if ( m_texelRgba8Data.empty() )
	{
		return Rgba8::BLACK;
	}
	int index = texelCoords.y * m_dimensions.x + texelCoords.x;
	return m_texelRgba8Data[index];
}


//-----------------------------------------------------------------------------------------------
void Image::SetTexelColor( IntVec2 const& texelCoords, Rgba8 const& newColor )
{
	if ( m_texelRgba8Data.empty() )
	{
		return;
	}
	int index = texelCoords.y * m_dimensions.x + texelCoords.x;
	m_texelRgba8Data[index] = newColor;
}


//-----------------------------------------------------------------------------------------------
void* const Image::GetRawData() const
{
	if ( m_texelRgba8Data.empty() )
	{
		return nullptr;
	}
	return ( void* ) m_texelRgba8Data.data();
}