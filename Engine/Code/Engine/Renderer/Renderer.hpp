#pragma once
#include "Game/EngineBuildPreferences.hpp"
#include <cstdint>
#include <vector>

#define DX_SAFE_RELEASE( dxObject )\
{\
    if ( ( dxObject ) != nullptr )\
    {\
        ( dxObject )->Release();\
        ( dxObject ) = nullptr;\
    }\
}


//-----------------------------------------------------------------------------------------------
struct IntVec2;
struct Rgba8;
struct Vertex;
class BitmapFont;
class Camera;
class Texture;
class Shader;
class ConstantBuffer;
class VertexBuffer;

struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;


//-----------------------------------------------------------------------------------------------
struct RenderConfig
{
	bool m_isEnabled = true;
};


//-----------------------------------------------------------------------------------------------
enum BlendMode
{
	ALPHA,
	ADDITIVE,
};


//-----------------------------------------------------------------------------------------------
class Renderer
{
public:
	Renderer( RenderConfig const& config );
	~Renderer();
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void ClearScreen( Rgba8 const& clearColor );
	void BeginCamera( Camera const& camera );
	void EndCamera( Camera const& camera );
	void DrawVertexArray( std::vector<Vertex> const& verts );
	void DrawVertexArray( int numVertexes, Vertex const* vertexes );
	void BindTexture( Texture* texture );
	void SetBlendMode( BlendMode mode );

	Texture* CreateOrGetTextureFromFile( char const* imageFilePath );
	BitmapFont* CreateOrGetBitmapFontFromFile( char const* fontFilePathNameWithNoExtension );

	Shader* CreateShader( char const* shaderName );
	Shader* CreateShader( char const* shaderName, char const* shaderSource );
	bool CompileShaderToBytecode( std::vector<unsigned char>& outBytecode, char const* name, char const* source, char const* entryPoint, char const* target );
	void BindShader( Shader* shader );

	VertexBuffer* CreateVertexBuffer( const unsigned int size, unsigned int stride );
	void CopyCPUToGPU( void const* data, unsigned int size, VertexBuffer* vbo );
	void BindVertexBuffer( VertexBuffer* vbo );
	void DrawVertexBuffer( VertexBuffer* vbo, unsigned int vertexCount );

	ConstantBuffer* CreateConstantBuffer( unsigned int const size );
	void CopyCPUToGPU( void const* data, unsigned int size, ConstantBuffer* cbo );
	void BindConstantBuffer( int slot, ConstantBuffer* cbo );
	
private:
	Texture* CreateTextureFromFile( char const* imageFilePath );
	Texture* CreateTextureFromData( char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData );
	Texture* GetTextureForFileName( char const* imageFilePath );

	BitmapFont* CreateBitmapFontFromFile( char const* fontFilePathNameWithNoExtension, Texture& fontTexture );
	BitmapFont* GetBitmapFontForFileName( char const* fontFilePathNameWithNoExtension );

public:
	RenderConfig m_config;
	std::vector<Texture*> m_loadedTextures;
	std::vector<BitmapFont*> m_loadedFonts;

protected:
	ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;

	std::vector<Shader*> m_loadedShaders;
	Shader* m_defaultShader = nullptr;
	Shader* m_currentShader = nullptr;

	VertexBuffer* m_immediateVBO = nullptr;
	ConstantBuffer* m_cameraCBO = nullptr;
};