#pragma once
#include "Game/EngineBuildPreferences.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Mat44.hpp"
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
struct Vertex;
class BitmapFont;
class Camera;
class Image;
class Texture;
class Shader;
class ConstantBuffer;
class VertexBuffer;

struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11BlendState;
struct ID3D11SamplerState;


//-----------------------------------------------------------------------------------------------
struct RenderConfig
{
	bool m_isEnabled = true;
};


//-----------------------------------------------------------------------------------------------
enum class BlendMode
{
	ALPHA,
	ADDITIVE,
	OPAQUE,
	COUNT
};


//-----------------------------------------------------------------------------------------------
enum class SamplerMode
{
	POINT_CLAMP,
	BILINEAR_WRAP,
	COUNT
};


//-----------------------------------------------------------------------------------------------
enum class RasterizerMode
{
	SOLID_CULL_NONE,
	SOLID_CULL_BACK,
	WIREFRAME_CULL_NONE,
	WIREFRAME_CULL_BACK,
	COUNT
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
	void SetSamplerMode( SamplerMode mode );
	void SetRasterizerMode( RasterizerMode mode );
	void SetStatesIfChanged();

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
	void SetModelConstants( Mat44 const& modelToWorldTransform = Mat44(), Rgba8 const& modelColor = Rgba8::WHITE );
	
private:
	Image* CreateImageFromFile( char const* imageFilePath );
	Texture* CreateTextureFromImage( char const* name, Image const& image );
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
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;

	std::vector<Shader*> m_loadedShaders;
	Shader* m_defaultShader = nullptr;
	Shader* m_currentShader = nullptr;

	VertexBuffer* m_immediateVBO = nullptr;
	ConstantBuffer* m_cameraCBO = nullptr;
	ConstantBuffer* m_modelCBO = nullptr;

	ID3D11BlendState* m_blendState = nullptr;
	BlendMode m_desiredBlendMode = BlendMode::ALPHA;
	ID3D11BlendState* m_blendStates[( int ) BlendMode::COUNT] = {};

	Texture* m_defaultTexture = nullptr;

	ID3D11SamplerState* m_samplerState = nullptr;
	SamplerMode m_desiredSamplerMode = SamplerMode::POINT_CLAMP;
	ID3D11SamplerState* m_samplerStates[( int ) SamplerMode::COUNT] = {};

	ID3D11RasterizerState* m_rasterizerState = nullptr;
	RasterizerMode m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	ID3D11RasterizerState* m_rasterizerStates[( int ) RasterizerMode::COUNT] = {};
};