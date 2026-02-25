#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/DefaultShader.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "ThirdParty/stb/stb_image.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

#if defined( ENGINE_DEBUG_RENDER )
#include <dxgidebug.h>
#pragma comment( lib, "dxguid.lib" )
#endif

#if defined( ENGINE_DEBUG_RENDER )
void* m_dxgiDebug = nullptr;
void* m_dxgiDebugModule = nullptr;
#endif

#if defined(OPAQUE)
#undef OPAQUE
#endif


//-----------------------------------------------------------------------------------------------
struct CameraConstants
{
	Mat44 WorldToCameraTransform;  // View transform
	Mat44 CameraToRenderTransform; // Non-standard transform from game to DirectX conventions
	Mat44 RenderToClipTransform;   // Projection transform
};
static const int k_cameraConstantsSlot = 2;


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
	unsigned int deviceFlags = 0;
#if defined( ENGINE_DEBUG_RENDER )
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create device and swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = g_engine->m_window->GetClientDimensions().x;
	swapChainDesc.BufferDesc.Height = g_engine->m_window->GetClientDimensions().y;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = ( HWND ) g_engine->m_window->GetHwnd();
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		deviceFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_swapChain,
		&m_device,
		nullptr,
		&m_deviceContext
	);
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create D3D 11 device and swap chian." );
	}

	// Get back buffer texture
	ID3D11Texture2D* backBuffer;
	hr = m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( void** ) &backBuffer );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not get swap chain buffer." );
	}

	hr = m_device->CreateRenderTargetView( backBuffer, NULL, &m_renderTargetView );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create render target view for swap chain buffer." );
	}

	backBuffer->Release();

	// Create debug module
#if defined( ENGINE_DEBUG_RENDER )
	m_dxgiDebugModule = ( void* )::LoadLibraryA( "dxgidebug.dll" );
	if ( m_dxgiDebugModule == nullptr )
	{
		ERROR_AND_DIE( "Could not load dxgidebug.dll" );
	}

	typedef HRESULT( WINAPI* GetDebugModuleCB )( REFIID, void** );
	( ( GetDebugModuleCB )::GetProcAddress( ( HMODULE ) m_dxgiDebugModule, "DXGIGetDebugInterface" ) )
		( __uuidof( IDXGIDebug ), &m_dxgiDebug );

	if ( m_dxgiDebug == nullptr )
	{
		ERROR_AND_DIE( "Could not load debug module." );
	}
#endif

	// Create and bind default shader
	m_defaultShader = CreateShader( "Default", g_shaderSource );
	BindShader( m_defaultShader );

	// Create vertex buffer
	m_immediateVBO = CreateVertexBuffer( sizeof( Vertex ), sizeof( Vertex ) );

	// Create camera constant buffer
	m_cameraCBO = CreateConstantBuffer( sizeof( CameraConstants ) );

	// Create rasterizer states for all rasterizer modes
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = true;
	// rasterizerDesc.DepthBias = 0;
	// rasterizerDesc.DepthBiasClamp = 0.f;
	// rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	// rasterizerDesc.ScissorEnable = false;
	// rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState( &rasterizerDesc, 
		&m_rasterizerStates[( int ) RasterizerMode::SOLID_CULL_NONE] );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create rasterizer state: SOLID_CULL_NONE." );
	}

	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	hr = m_device->CreateRasterizerState( &rasterizerDesc, 
		&m_rasterizerStates[( int ) RasterizerMode::SOLID_CULL_BACK] );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create rasterizer state: SOLID_CULL_BACK." );
	}

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	hr = m_device->CreateRasterizerState( &rasterizerDesc, 
		&m_rasterizerStates[( int ) RasterizerMode::WIREFRAME_CULL_NONE] );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create rasterizer state: WIREFRAME_CULL_NONE." );
	}

	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	hr = m_device->CreateRasterizerState( &rasterizerDesc, 
		&m_rasterizerStates[( int ) RasterizerMode::WIREFRAME_CULL_BACK] );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create rasterizer state: WIREFRAME_CULL_BACK." );
	}

	// Create blend states for all blend modes
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_device->CreateBlendState( &blendDesc, &m_blendStates[( int ) BlendMode::OPAQUE] );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create blend state: OPAQUE." );
	}
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	hr = m_device->CreateBlendState( &blendDesc, &m_blendStates[( int ) BlendMode::ALPHA] );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create blend state: ALPHA." );
	}
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	hr = m_device->CreateBlendState( &blendDesc, &m_blendStates[( int ) BlendMode::ADDITIVE] );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create blend state: ADDITIVE." );
	}

	// Create sampler states for all sampler modes
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_device->CreateSamplerState( &samplerDesc, &m_samplerStates[( int ) SamplerMode::POINT_CLAMP] );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create sampler state: POINT_CLAMP." );
	}
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = m_device->CreateSamplerState( &samplerDesc, &m_samplerStates[( int ) SamplerMode::BILINEAR_WRAP] );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create sampler state: BILINEAR_WRAP." );
	}

	// Create and bind the default texture (2x2 white texture)
	m_defaultTexture = CreateTextureFromImage( "Default", Image( IntVec2( 2, 2 ), Rgba8::WHITE ) );
	BindTexture( m_defaultTexture );
}


//-----------------------------------------------------------------------------------------------
void Renderer::Shutdown()
{
	DX_SAFE_RELEASE( m_renderTargetView );

	// Release blend states
	for ( int blendModeIndex = 0; blendModeIndex < ( int ) BlendMode::COUNT; ++blendModeIndex )
	{
		DX_SAFE_RELEASE( m_blendStates[blendModeIndex] );
	}
	m_blendState = nullptr;

	// Release sampler states
	for ( int samplerModeIndex = 0; samplerModeIndex < ( int ) SamplerMode::COUNT; ++samplerModeIndex )
	{
		DX_SAFE_RELEASE( m_samplerStates[samplerModeIndex] );
	}
	m_samplerState = nullptr;

	// Release rasterizer states
	for ( int rasterizerModeIndex = 0; rasterizerModeIndex < ( int ) RasterizerMode::COUNT; ++rasterizerModeIndex )
	{
		DX_SAFE_RELEASE( m_rasterizerStates[rasterizerModeIndex] );
	}

	// Release loaded textures
	for ( Texture* texture : m_loadedTextures )
	{
		delete texture;
	}

	// Release loaded fonts
	for ( BitmapFont* font : m_loadedFonts )
	{
		delete font;
	}

	// Release loaded shaders
	for ( Shader* shader : m_loadedShaders )
	{
		delete shader;
	}

	// Release immediate VBO
	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	// Release camera CBO
	delete m_cameraCBO;
	m_cameraCBO = nullptr;

	// Release device-related objects
	DX_SAFE_RELEASE( m_swapChain );
	DX_SAFE_RELEASE( m_deviceContext );
	DX_SAFE_RELEASE( m_device );

	// Report error leaks and release debug module
#if defined(ENGINE_DEBUG_RENDER)
	( ( IDXGIDebug* ) m_dxgiDebug )->ReportLiveObjects(
		DXGI_DEBUG_ALL,
		( DXGI_DEBUG_RLO_FLAGS ) ( DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL )
	);

	( ( IDXGIDebug* ) m_dxgiDebug )->Release();
	m_dxgiDebug = nullptr;

	::FreeLibrary( ( HMODULE ) m_dxgiDebugModule );
	m_dxgiDebugModule = nullptr;
#endif
}


//-----------------------------------------------------------------------------------------------
void Renderer::BeginFrame()
{
	// Set render target
	m_deviceContext->OMSetRenderTargets( 1, &m_renderTargetView, nullptr );
}


//-----------------------------------------------------------------------------------------------
void Renderer::EndFrame()
{
	// Present
	HRESULT hr;
	hr = m_swapChain->Present( 0, 0 );
	if ( hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET )
	{
		ERROR_AND_DIE( "Device has been lost, application will now terminate." );
	}
}


//-----------------------------------------------------------------------------------------------
void Renderer::ClearScreen( Rgba8 const& clearColor )
{
	// Set render target
	m_deviceContext->OMSetRenderTargets( 1, &m_renderTargetView, nullptr );

	// Clear the screen
	float clearColorAsFloats[4];
	clearColor.GetAsFloats( clearColorAsFloats );
	m_deviceContext->ClearRenderTargetView( m_renderTargetView, clearColorAsFloats );
}


//-----------------------------------------------------------------------------------------------
void Renderer::BeginCamera( Camera const& camera )
{
	// Set viewport
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;
	viewport.Width = ( float ) g_engine->m_window->GetClientDimensions().x;
	viewport.Height = ( float ) g_engine->m_window->GetClientDimensions().y;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	m_deviceContext->RSSetViewports( 1, &viewport );

	// Update camera constant buffer
	CameraConstants cameraData;
	if ( camera.GetMode() == Camera::eMode_Perspective )
	{
		cameraData.WorldToCameraTransform = camera.GetWorldToCameraTransform();
		cameraData.CameraToRenderTransform = camera.GetCameraToRenderTransform();
	}
	cameraData.RenderToClipTransform = camera.GetRenderToClipTransform();
	CopyCPUToGPU( &cameraData, sizeof( CameraConstants ), m_cameraCBO );
	BindConstantBuffer( k_cameraConstantsSlot, m_cameraCBO );
}


//-----------------------------------------------------------------------------------------------
void Renderer::EndCamera( [[maybe_unused]] Camera const& camera )
{
	// DO NOTHING
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray( int numVertexes, Vertex const* vertexes )
{
	CopyCPUToGPU( vertexes, numVertexes * sizeof( Vertex ), m_immediateVBO );
	DrawVertexBuffer( m_immediateVBO, numVertexes );

}


//------------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray( std::vector<Vertex> const& verts )
{
	DrawVertexArray( static_cast< int >( verts.size() ), verts.data() );
}


//-----------------------------------------------------------------------------------------------
void Renderer::BindTexture( Texture* texture )
{
	if ( texture )
	{
		m_deviceContext->PSSetShaderResources( 0, 1, &texture->m_shaderResourceView );
	}
	else
	{
		m_deviceContext->PSSetShaderResources( 0, 1, &m_defaultTexture->m_shaderResourceView );
	}
}


//------------------------------------------------------------------------------------------------
void Renderer::SetBlendMode( BlendMode blendMode )
{
	m_desiredBlendMode = blendMode;
}


//------------------------------------------------------------------------------------------------
void Renderer::SetSamplerMode( SamplerMode samplerMode )
{
	m_desiredSamplerMode = samplerMode;
}


//------------------------------------------------------------------------------------------------
void Renderer::SetRasterizerMode( RasterizerMode rasterizerMode )
{
	m_desiredRasterizerMode = rasterizerMode;
}


//------------------------------------------------------------------------------------------------
void Renderer::SetStatesIfChanged()
{
	ID3D11BlendState* desiredBlendState = m_blendStates[( int ) m_desiredBlendMode];
	if ( desiredBlendState != m_blendState )
	{
		m_blendState = desiredBlendState;
		float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
		UINT sampleMask = 0xffffffff;
		m_deviceContext->OMSetBlendState( m_blendState, blendFactor, sampleMask );
	}

	ID3D11SamplerState* desiredSamplerState = m_samplerStates[( int ) m_desiredSamplerMode];
	if ( desiredSamplerState != m_samplerState )
	{
		m_samplerState = desiredSamplerState;
		m_deviceContext->PSSetSamplers( 0, 1, &m_samplerState );
	}

	ID3D11RasterizerState* desiredRasterizerState = m_rasterizerStates[( int ) m_desiredRasterizerMode];
	if ( desiredRasterizerState != m_rasterizerState )
	{
		m_rasterizerState = desiredRasterizerState;
		m_deviceContext->RSSetState( m_rasterizerState );
	}
}


//------------------------------------------------------------------------------------------------
Image* Renderer::CreateImageFromFile( char const* imageFilePath )
{
	Image* newImage = new Image( imageFilePath );
	return newImage;
}


//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromImage( char const* name, Image const& image )
{
	// Check if the load was successful
	GUARANTEE_OR_DIE( image.GetDimensions().x > 0 && image.GetDimensions().y > 0, Stringf( "CreateTextureFromImage failed for \"%s\" - image dimensions were invalid (%i x %i)", name, image.GetDimensions().x, image.GetDimensions().y ) );

	Texture* newTexture = new Texture();
	newTexture->m_name = name;
	newTexture->m_dimensions = image.GetDimensions();

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = image.GetDimensions().x;
	textureDesc.Height = image.GetDimensions().y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureData;
	textureData.pSysMem = image.GetRawData();
	textureData.SysMemPitch = 4 * image.GetDimensions().x;

	HRESULT hr;
	hr = m_device->CreateTexture2D( &textureDesc, &textureData, &newTexture->m_texture );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( Stringf( "Could not create texture for image \"%s\".", name ) );
	}

	hr = m_device->CreateShaderResourceView( newTexture->m_texture, NULL, &newTexture->m_shaderResourceView );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( Stringf( "Could not create shader resource view for image \"%s\".", name ) );
	}

	m_loadedTextures.push_back( newTexture );
	return newTexture;
}


//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromData( char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData )
{
	// Check if the load was successful
	GUARANTEE_OR_DIE( texelData, Stringf( "CreateTextureFromData failed for \"%s\" - texelData was null!", name ) );
	GUARANTEE_OR_DIE( bytesPerTexel >= 3 && bytesPerTexel <= 4, Stringf( "CreateTextureFromData failed for \"%s\" - unsupported BPP=%i (must be 3 or 4)", name, bytesPerTexel ) );
	GUARANTEE_OR_DIE( dimensions.x > 0 && dimensions.y > 0, Stringf( "CreateTextureFromData failed for \"%s\" - illegal texture dimensions (%i x %i)", name, dimensions.x, dimensions.y ) );

	Image* newImage = new Image( dimensions, Rgba8::WHITE );
	for ( int y = 0; y < dimensions.y; ++y )
	{
		for ( int x = 0; x < dimensions.x; ++x )
		{
			int texelIndex = ( y * dimensions.x + x ) * bytesPerTexel;
			uint8_t r = texelData[texelIndex + 0];
			uint8_t g = texelData[texelIndex + 1];
			uint8_t b = texelData[texelIndex + 2];
			uint8_t a = ( bytesPerTexel == 4 ) ? texelData[texelIndex + 3] : 255;
			newImage->SetTexelColor( IntVec2( x, y ), Rgba8( r, g, b, a ) );
		}
	}
	Texture* newTexture = CreateTextureFromImage( name, *newImage );

	m_loadedTextures.push_back( newTexture );
	return newTexture;
}


//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromFile( char const* imageFilePath )
{
	Image* image = CreateImageFromFile( imageFilePath );
	Texture* newTexture = CreateTextureFromImage( imageFilePath, *image );

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


//------------------------------------------------------------------------------------------------
void Renderer::BindShader( Shader* shader )
{
	if ( shader == nullptr )
	{
		shader = m_defaultShader;
	}
	if ( shader != m_currentShader )
	{
		m_currentShader = shader;

		m_deviceContext->IASetInputLayout( shader->m_inputLayout );
		m_deviceContext->VSSetShader( shader->m_vertexShader, nullptr, 0 );
		m_deviceContext->PSSetShader( shader->m_pixelShader, nullptr, 0 );
	}
}


//------------------------------------------------------------------------------------------------
Shader* Renderer::CreateShader( char const* shaderName )
{
	std::string filename = Stringf( "Data/Shaders/%s.hlsl", shaderName );
	std::string shaderSource;
	if ( ::FileReadToString( shaderSource, filename ) != 0 )
	{
		ERROR_AND_DIE( Stringf( "Could not read shader file \"%s\"", filename.c_str() ) );
	}
	return CreateShader( shaderName, shaderSource.c_str() );
}


//------------------------------------------------------------------------------------------------
Shader* Renderer::CreateShader( char const* shaderName, char const* shaderSource )
{
	ShaderConfig config;
	config.m_name = shaderName;
	Shader* shader = new Shader( config );

	// Compile vertex shader
	std::vector<unsigned char> vertexShaderByteCode;
	if ( !CompileShaderToBytecode( vertexShaderByteCode, shaderName, shaderSource, "VertexMain", "vs_5_0" ) )
	{
		ERROR_AND_DIE( Stringf( "Could not compile vertex shader." ) );
	}

	// Create vertex shader
	HRESULT hr;
	hr = m_device->CreateVertexShader(
		vertexShaderByteCode.data(),
		vertexShaderByteCode.size(),
		NULL, &shader->m_vertexShader );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( Stringf( "Could not create vertex shader." ) );
	}

	// Compile pixel shader
	std::vector<unsigned char> pixelShaderByteCode;
	if ( !CompileShaderToBytecode( pixelShaderByteCode, shaderName, shaderSource, "PixelMain", "ps_5_0" ) )
	{
		ERROR_AND_DIE( Stringf( "Could not compile pixel shader." ) );
	}

	// Create pixel shader
	hr = m_device->CreatePixelShader(
		pixelShaderByteCode.data(),
		pixelShaderByteCode.size(),
		NULL, &shader->m_pixelShader );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( Stringf( "Could not create pixel shader." ) );
	}

	// Create input layout
	D3D11_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE( inputElementDescs );
	hr = m_device->CreateInputLayout(
		inputElementDescs,
		numElements,
		vertexShaderByteCode.data(),
		vertexShaderByteCode.size(),
		&shader->m_inputLayout );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( Stringf( "Could not create vertex layout for PCU vertex." ) );
	}
	
	m_loadedShaders.push_back( shader );
	return shader;
}


//------------------------------------------------------------------------------------------------
bool Renderer::CompileShaderToBytecode( std::vector<unsigned char>& outBytecode, char const* name, char const* source, char const* entryPoint, char const* target )
{
	// Compile vertex shader
	DWORD shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined( ENGINE_DEBUG_RENDER )
	shaderFlags = D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	shaderFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif
	ID3DBlob* shaderBlob = NULL;
	ID3DBlob* errorBlob = NULL;

	HRESULT hr;
	hr = D3DCompile(
		source, strlen( source ),
		name, nullptr, nullptr,
		entryPoint, target, shaderFlags,
		0, &shaderBlob, &errorBlob );

	if ( SUCCEEDED( hr ) )
	{
		outBytecode.resize( shaderBlob->GetBufferSize() );
		memcpy(
			outBytecode.data(),
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize() );
	}
	else
	{
		if ( errorBlob != NULL )
		{
			DebuggerPrintf( ( char* ) errorBlob->GetBufferPointer() );
		}
		ERROR_AND_DIE( Stringf( "Could not compile vertex shader." ) );
	}

	shaderBlob->Release();
	if ( errorBlob != NULL )
	{
		errorBlob->Release();
	}

	return true;
}


//------------------------------------------------------------------------------------------------
VertexBuffer* Renderer::CreateVertexBuffer( const unsigned int size, unsigned int stride )
{
	VertexBuffer* vbo = new VertexBuffer( m_device, size, stride );
	return vbo;
}


//------------------------------------------------------------------------------------------------
void Renderer::CopyCPUToGPU( void const* data, unsigned int size, VertexBuffer* vbo )
{
	unsigned int vboSize = vbo->GetSize();
	if ( size > vboSize )
	{
		vbo->Resize( size );
	}

	// Copy vertices
	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(
		vbo->m_buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&resource );
	memcpy( resource.pData, data, size );
	m_deviceContext->Unmap( vbo->m_buffer, 0 );
}


//------------------------------------------------------------------------------------------------
void Renderer::BindVertexBuffer( VertexBuffer* vbo )
{
	UINT stride = vbo->GetStride();
	UINT offset = 0;
	m_deviceContext->IASetVertexBuffers( 0, 1, &vbo->m_buffer, &stride, &offset );
	m_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}


//------------------------------------------------------------------------------------------------
void Renderer::DrawVertexBuffer( VertexBuffer* vbo, unsigned int vertexCount )
{
	BindVertexBuffer( vbo );
	SetStatesIfChanged();
	m_deviceContext->Draw( vertexCount, 0 );
}


//------------------------------------------------------------------------------------------------
ConstantBuffer* Renderer::CreateConstantBuffer( unsigned int size )
{
	ConstantBuffer* cbo = new ConstantBuffer( size );

	// Create constant buffer
	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = static_cast< UINT >( cbo->m_size );
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr;
	hr = m_device->CreateBuffer(
		&constantBufferDesc,
		nullptr,
		&cbo->m_buffer );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( "Could not create vertex buffer." );
	}

	return cbo;
}


//------------------------------------------------------------------------------------------------
void Renderer::CopyCPUToGPU( void const* data, unsigned int size, ConstantBuffer* cbo )
{
	GUARANTEE_OR_DIE( size <= cbo->m_size, Stringf( "Data size (%u bytes) exceeds constant buffer size (%zu bytes).", size, cbo->m_size ) );

	// Copy data
	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(
		cbo->m_buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&resource );
	memcpy( resource.pData, data, size );
	m_deviceContext->Unmap( cbo->m_buffer, 0 );
}


//------------------------------------------------------------------------------------------------
void Renderer::BindConstantBuffer( int slot, ConstantBuffer* cbo )
{
	m_deviceContext->VSSetConstantBuffers( slot, 1, &cbo->m_buffer );
	m_deviceContext->PSSetConstantBuffers( slot, 1, &cbo->m_buffer );
}