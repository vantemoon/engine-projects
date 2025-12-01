#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>


//-----------------------------------------------------------------------------------------------
class Image
{
public:
	Image( char const* imageFilePath );
	std::string const&	GetImageFilePath() const;
	IntVec2				GetDimensions() const;
	Rgba8				GetTexelColor( IntVec2 const& texelCoords ) const;
	void				SetTexelColor( IntVec2 const& texelCoords, Rgba8 const& newColor );

private:
	std::string			m_imageFilePath;
	IntVec2				m_dimensions = IntVec2( 0, 0 );
	Rgba8*				m_rgbaTexels = nullptr;
};
