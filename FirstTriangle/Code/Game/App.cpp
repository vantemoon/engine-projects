#include "Game/App.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

#if defined( _DEBUG )
#define ENGINE_DEBUG_RENDER
#endif

#if defined( ENGINE_DEBUG_RENDER )
#include <dxgidebug.h>
#pragma comment( lib, "dxguid.lib" )
#endif

ID3D11Device* m_device = nullptr;
ID3D11DeviceContext* m_deviceContext = nullptr;
IDXGISwapChain* m_swapChain = nullptr;
ID3D11RenderTargetView* m_renderTargetView = nullptr;
ID3D11VertexShader* m_vertexShader = nullptr;
ID3D11PixelShader* m_pixelShader = nullptr;
ID3D11InputLayout* m_inputLayoutForVertex_PCU = nullptr;
ID3D11Buffer* m_vertexBuffer = nullptr;
ID3D11RasterizerState* m_rasterizerState = nullptr;

std::vector<uint8_t> m_vertexShaderByteCode;
std::vector<uint8_t> m_pixelShaderByteCode;

#if defined( ENGINE_DEBUG_RENDER )
void* m_dxgiDebug = nullptr;
void* m_dxgiDebugModule = nullptr;
#endif


//-----------------------------------------------------------------------------------------------
App* g_app = nullptr;


//-----------------------------------------------------------------------------------------------
App::App()
{
	EngineConfig engineConfig;

	// Set up window config
	engineConfig.m_windowConfig.m_clientAspect = 2.0f;
	engineConfig.m_windowConfig.m_windowTitle = "First Triangle";

	// Set up renderer config
	engineConfig.m_rendererConfig.m_isEnabled = false;

	g_engine = new Engine( engineConfig );

	m_lastFrameStartTime = GetCurrentTimeSeconds();

	g_engine->m_eventSystem->SubscribeEventCallbackFunction( "Quit", Command_Quit );

	Startup();
}


//-----------------------------------------------------------------------------------------------
App::~App()
{
	Shutdown();

	delete g_engine;
	g_engine = nullptr;
}


//-----------------------------------------------------------------------------------------------
void App::Startup()
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
		(__uuidof( IDXGIDebug ), &m_dxgiDebug);

	if ( m_dxgiDebug == nullptr )
	{
		ERROR_AND_DIE( "Could not load debug module." );
	}
#endif
}


//-----------------------------------------------------------------------------------------------
void App::Shutdown()
{
	m_rasterizerState->Release();
	m_vertexBuffer->Release();
	m_vertexShader->Release();
	m_pixelShader->Release();
	m_inputLayoutForVertex_PCU->Release();
	m_renderTargetView->Release();
	m_swapChain->Release();
	m_deviceContext->Release();
	m_device->Release();

	// Report error leaks and release debug module
#if defined(ENGINE_DEBUG_RENDER)
	( ( IDXGIDebug* )m_dxgiDebug )->ReportLiveObjects(
		DXGI_DEBUG_ALL,
		( DXGI_DEBUG_RLO_FLAGS )( DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL )
	);

	( ( IDXGIDebug* ) m_dxgiDebug )->Release();
	m_dxgiDebug = nullptr;

	::FreeLibrary( ( HMODULE ) m_dxgiDebugModule );
	m_dxgiDebugModule = nullptr;
#endif
}


//-----------------------------------------------------------------------------------------------
void App::RunMainLoop()
{
	while ( !IsQuitting() )
	{
		RunFrame();
	}
}


//-----------------------------------------------------------------------------------------------
void App::RunFrame()
{
	g_engine->BeginFrame();

	double currentTime = GetCurrentTimeSeconds();
	float deltaSeconds = static_cast<float> ( currentTime - m_lastFrameStartTime );
	deltaSeconds = GetClamped( deltaSeconds, 0.f, 0.1f );

	m_lastFrameStartTime = currentTime;

	Update( deltaSeconds );
	Render(); // Draw the current state of the game
	g_engine->EndFrame(); // Allow engine subsystems to do post-frame stuff
}


//-----------------------------------------------------------------------------------------------
void App::UpdateFromKeyboard()
{
	if ( g_engine->m_inputSystem->WasKeyJustPressed( KEYCODE_ESCAPE ) )
	{
		SetIsQuitting();
	}
}


//-----------------------------------------------------------------------------------------------
void App::UpdateFromController()
{
}


//-----------------------------------------------------------------------------------------------
void App::HardReset()
{
}


//-----------------------------------------------------------------------------------------------
void App::Update( [[maybe_unused]] float deltaSeconds )
{
	UpdateFromKeyboard();
}


//-----------------------------------------------------------------------------------------------
void App::Render() const
{
	// Set render target
	m_deviceContext->OMSetRenderTargets( 1, &m_renderTargetView, nullptr );

	// Clear the screen
	Rgba8 clearColor( 127, 127, 127, 255 );
	float clearColorAsFloats[4];
	clearColor.GetAsFloats( clearColorAsFloats );
	m_deviceContext->ClearRenderTargetView( m_renderTargetView, clearColorAsFloats );

	// Present
	HRESULT hr;
	hr = m_swapChain->Present( 0, 0 );
	if ( hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET )
	{
		ERROR_AND_DIE( "Device has been lost, application will now terminate." );
	}
}


//-----------------------------------------------------------------------------------------------
bool App::Command_Quit( EventArgs& args )
{
	UNUSED( args );
	if ( g_app && !g_app->m_isQuitting )
	{
		g_app->SetIsQuitting();
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
void App::SetIsQuitting()
{
	m_isQuitting = true;
}


//-----------------------------------------------------------------------------------------------
bool App::IsQuitting() const
{
	return m_isQuitting;
}