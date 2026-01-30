#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "ThirdParty/stb/stb_image.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


//-----------------------------------------------------------------------------------------------
Renderer::Renderer( RenderConfig const& config )
	: m_config( config )
{
}


//-----------------------------------------------------------------------------------------------
Renderer::~Renderer() = default;


//-----------------------------------------------------------------------------------------------
void Renderer::Startup()
{
}


//-----------------------------------------------------------------------------------------------
void Renderer::Shutdown()
{
	// DO NOTHING
}


//-----------------------------------------------------------------------------------------------
void Renderer::BeginFrame()
{
	// DO NOTHING
}


//-----------------------------------------------------------------------------------------------
void Renderer::EndFrame()
{
	// "Present" the back-buffer by swapping the front (visible) and back (working) screen buffers
}


//-----------------------------------------------------------------------------------------------
void Renderer::ClearScreen( [[maybe_unused]] Rgba8 const& clearColor )
{
	/*float redByte = static_cast<float> ( clearColor.r ) / 255.f;
	float greenByte = static_cast<float> ( clearColor.g ) / 255.f;
	float blueByte = static_cast<float> ( clearColor.b ) / 255.f;
	float alphaByte = static_cast<float> ( clearColor.a ) / 255.f;*/
}


//-----------------------------------------------------------------------------------------------
void Renderer::BeginCamera( [[maybe_unused]] Camera const& camera )
{
	Vec2 bottomLeft = camera.GetOrthoBottomLeft();
	Vec2 topRight = camera.GetOrthoTopRight();
}


//-----------------------------------------------------------------------------------------------
void Renderer::EndCamera( [[maybe_unused]] Camera const& camera )
{
	// DO NOTHING
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray( [[maybe_unused]] int numVertexes, [[maybe_unused]] Vertex const* vertexes )
{
}


//------------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray( std::vector<Vertex> const& verts )
{
	DrawVertexArray( static_cast< int >( verts.size() ), verts.data() );
}


//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromData( char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData )
{
	// Check if the load was successful
	GUARANTEE_OR_DIE( texelData, Stringf( "CreateTextureFromData failed for \"%s\" - texelData was null!", name ) );
	GUARANTEE_OR_DIE( bytesPerTexel >= 3 && bytesPerTexel <= 4, Stringf( "CreateTextureFromData failed for \"%s\" - unsupported BPP=%i (must be 3 or 4)", name, bytesPerTexel ) );
	GUARANTEE_OR_DIE( dimensions.x > 0 && dimensions.y > 0, Stringf( "CreateTextureFromData failed for \"%s\" - illegal texture dimensions (%i x %i)", name, dimensions.x, dimensions.y ) );

	Texture* newTexture = new Texture();
	newTexture->m_name = name; // NOTE: m_name must be a std::string, otherwise it may point to temporary data!
	newTexture->m_dimensions = dimensions;

	m_loadedTextures.push_back( newTexture );
	return newTexture;
}


//-----------------------------------------------------------------------------------------------
void Renderer::BindTexture( Texture* texture )
{
	if ( texture )
	{
		// Bind the texture
	}
	else
	{
		// Disable texturing
	}
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetBlendMode( BlendMode blendMode )
{
	if ( blendMode == BlendMode::ALPHA )
	{
		// Set standard alpha blending
	}
	else if ( blendMode == BlendMode::ADDITIVE )
	{
		// Set additive blending
	}
	else
	{
		ERROR_AND_DIE( Stringf( "Unknown / unsupported blend mode #%i", blendMode ) );
	}
}


//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromFile( char const* imageFilePath )
{
	IntVec2 dimensions = IntVec2::ZERO;		// This will be filled in for us to indicate image width & height
	int bytesPerTexel = 0;					// ...and how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)

	// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	stbi_set_flip_vertically_on_load( 1 ); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* texelData = stbi_load( imageFilePath, &dimensions.x, &dimensions.y, &bytesPerTexel, 0 );

	// Check if the load was successful
	GUARANTEE_OR_DIE( texelData, Stringf( "Failed to load image \"%s\"", imageFilePath ) );

	Texture* newTexture = CreateTextureFromData( imageFilePath, dimensions, bytesPerTexel, texelData );

	// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
	stbi_image_free( texelData );

	return newTexture;
}


//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateOrGetTextureFromFile( char const* imageFilePath )
{
	// See if we already have this texture previously loaded
	Texture* existingTexture = GetTextureForFileName( imageFilePath ); // You need to write this
	if ( existingTexture )
	{
		return existingTexture;
	}

	// Never seen this texture before!  Let's load it.
	Texture* newTexture = CreateTextureFromFile( imageFilePath );
	return newTexture;
}


//------------------------------------------------------------------------------------------------
Texture* Renderer::GetTextureForFileName( char const* imageFilePath )
{
	for ( int textureIndex = 0; textureIndex < ( int ) m_loadedTextures.size(); ++textureIndex )
	{
		Texture* texture = m_loadedTextures[textureIndex];
		if ( texture->m_name == imageFilePath )
		{
			return texture;
		}
	}
	return nullptr;
}


//------------------------------------------------------------------------------------------------
BitmapFont* Renderer::CreateBitmapFontFromFile( char const* fontFilePathNameWithNoExtension, Texture& fontTexture )
{
	BitmapFont* newFont = new BitmapFont( fontFilePathNameWithNoExtension, fontTexture );
	m_loadedFonts.push_back( newFont );
	return newFont;
}


//------------------------------------------------------------------------------------------------
BitmapFont* Renderer::CreateOrGetBitmapFontFromFile( char const* fontFilePathNameWithNoExtension )
{
	// See if we already have this font previously loaded
	BitmapFont* existingFont = GetBitmapFontForFileName( fontFilePathNameWithNoExtension );
	if ( existingFont )
	{
		return existingFont;
	}

	// Never seen this font before!  Let's load it.
	BitmapFont* newFont = CreateBitmapFontFromFile( fontFilePathNameWithNoExtension,
						 *CreateOrGetTextureFromFile( Stringf( "%s.png", fontFilePathNameWithNoExtension ).c_str() ) );
	return newFont;
}


//------------------------------------------------------------------------------------------------
BitmapFont* Renderer::GetBitmapFontForFileName( char const* fontFilePathNameWithNoExtension )
{
	for ( int fontIndex = 0; fontIndex < ( int ) m_loadedFonts.size(); ++fontIndex )
	{
		BitmapFont* font = m_loadedFonts[fontIndex];
		if ( font->m_fontFilePathNameWithNoExtension == fontFilePathNameWithNoExtension )
		{
			return font;
		}
	}
	return nullptr;
}