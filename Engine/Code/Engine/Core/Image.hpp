#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
class Image
{
	friend class Renderer;

public:
	Image();
	~Image();
	Image( char const* imageFilePath );
	Image( IntVec2 size, Rgba8 color );

	IntVec2				GetDimensions() const;
	std::string const&	GetImageFilePath() const;
	void* const			GetRawData() const;
	Rgba8				GetTexelColor( IntVec2 const& texelCoords ) const;
	void				SetTexelColor( IntVec2 const& texelCoords, Rgba8 const& newColor );

private:
	std::string				m_imageFilePath;
	IntVec2					m_dimensions = IntVec2( 0, 0 );
	std::vector<Rgba8>		m_texelRgba8Data;
};
