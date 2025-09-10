#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#pragma comment( lib, "opengl32" )

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
HGLRC g_openGLRenderingContext = nullptr;			// ...becomes void* Renderer::m_apiRenderingContext


//-----------------------------------------------------------------------------------------------
Renderer::Renderer()
{
	// DO NOTHING
}


//-----------------------------------------------------------------------------------------------
Renderer::~Renderer()
{
	// DO NOTHING
}


//-----------------------------------------------------------------------------------------------
void Renderer::Startup()
{
	CreateRenderingContext();
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
	// DO NOTHING
}


//-----------------------------------------------------------------------------------------------
void Renderer::CreateRenderingContext()
{
	// Creates an OpenGL rendering context (RC) and binds it to the current window's device context (DC)
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset( &pixelFormatDescriptor, 0, sizeof( pixelFormatDescriptor ) );
	pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 24;
	pixelFormatDescriptor.cDepthBits = 24;
	pixelFormatDescriptor.cAccumBits = 0;
	pixelFormatDescriptor.cStencilBits = 8;

	HWND windowHandle = ::GetActiveWindow();
	HDC displayDeviceContext = GetDC( windowHandle );

	// These two OpenGL-like functions (wglCreateContext and wglMakeCurrent) will remain here for now.
	int pixelFormatCode = ChoosePixelFormat( displayDeviceContext, &pixelFormatDescriptor );
	SetPixelFormat( displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor );
	g_openGLRenderingContext = wglCreateContext( displayDeviceContext );
	wglMakeCurrent( displayDeviceContext, g_openGLRenderingContext );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}


//-----------------------------------------------------------------------------------------------
void Renderer::ClearScreen( Rgba8 const& clearColor )
{
	float redByte = static_cast< float >( clearColor.r ) / 255.f;
	float greenByte = static_cast< float >( clearColor.g ) / 255.f;
	float blueByte = static_cast< float >( clearColor.b ) / 255.f;
	float alphaByte = static_cast< float >( clearColor.a ) / 255.f;
	glClearColor( redByte, greenByte, blueByte, alphaByte ); // Note; glClearColor takes colors as floats in [0,1], not bytes in [0,255]
	glClear( GL_COLOR_BUFFER_BIT ); // ALWAYS clear the screen at the top of each frame's Render()!
}


//-----------------------------------------------------------------------------------------------
void Renderer::BeginCamera( Camera const& camera )
{
	// Establish a 2D (orthographic) drawing coordinate system: (0,0) bottom-left to (10,10) top-right
	glLoadIdentity();
	glOrtho( 0.f, 200.f, 0.f, 100.f, 0.f, 1.f ); // arguments are: xLeft, xRight, yBottom, yTop, zNear, zFar
}


//-----------------------------------------------------------------------------------------------
void Renderer::EndCamera( Camera const& camera )
{
	// DO NOTHING
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray( int numVertexes, Vertex const* vertexes )
{
	glBegin(GL_TRIANGLES);
	{
		for (int vertexIndex = 0; vertexIndex < numVertexes; ++vertexIndex)
		{
			Vertex const& currentVertex = vertexes[vertexIndex];
			glColor4ub( currentVertex.m_color.r, currentVertex.m_color.g, currentVertex.m_color.b, currentVertex.m_color.a );
			glTexCoord2f( currentVertex.m_uvTexCoords.x, currentVertex.m_uvTexCoords.y );
			glVertex2f( currentVertex.m_position.x, currentVertex.m_position.y );
		}
	}
	glEnd();
}