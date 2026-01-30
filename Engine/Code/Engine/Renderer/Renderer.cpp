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

const char* g_shaderSource = R"(	
struct vs_input_t
{
	float3 localPosition : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct v2p_t
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

v2p_t VertexMain( vs_input_t input )
{
	v2p_t v2p;
	v2p.position = float4( input.localPosition, 1 );
	v2p.color = input.color;
	v2p.uv = input.uv;
	return v2p;
}

float4 PixelMain( v2p_t input ) : SV_Target0
{
	return float4( input.color );
}
)";


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
	Shader* defaultShader = CreateShader( "default", g_shaderSource );
	BindShader( defaultShader );

	// Create vertex buffer
	m_immediateVBO = CreateVertexBuffer( sizeof( Vertex ), sizeof( Vertex ) );

	// Set rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState( &rasterizerDesc, &m_rasterizerState );
	if ( !SUCCEEDED( hr ) )
	{
		ERROR_AND_DIE( Stringf( "Could not create rasterizer state." ) );
	}
	m_deviceContext->RSSetState( m_rasterizerState );
}


//-----------------------------------------------------------------------------------------------
void Renderer::Shutdown()
{
	m_rasterizerState->Release();
	m_renderTargetView->Release();
	m_swapChain->Release();
	m_deviceContext->Release();
	m_device->Release();

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
void Renderer::BeginCamera( [[maybe_unused]] Camera const& camera )
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


//------------------------------------------------------------------------------------------------
void Renderer::BindShader( Shader* shader )
{
	if ( shader != m_currentShader )
	{
		m_currentShader = shader;

		m_deviceContext->IASetInputLayout( shader->m_inputLayout );
		m_deviceContext->VSSetShader( shader->m_vertexShader, nullptr, 0 );
		m_deviceContext->PSSetShader( shader->m_pixelShader, nullptr, 0 );
	}
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
	m_deviceContext->Draw( vertexCount, 0 );
}