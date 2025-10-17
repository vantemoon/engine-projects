#pragma once


//-----------------------------------------------------------------------------------------------
struct Rgba8;
struct Vertex;
class Camera;


//-----------------------------------------------------------------------------------------------
struct RenderConfig
{
	bool m_isEnabled = true;
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

	void CreateRenderingContext();
	void ClearScreen( Rgba8 const& clearColor );
	void BeginCamera( Camera const& camera );
	void EndCamera( Camera const& camera );
	void DrawVertexArray( int numVertexes, Vertex const* vertexes );

	RenderConfig m_config;
};