#pragma once
#include <cstdint>
#include <vector>


//-----------------------------------------------------------------------------------------------
struct IntVec2;
struct Rgba8;
struct Vertex;
class Camera;
class Texture;


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
	void DrawVertexArray( std::vector<Vertex> const& verts );
	void DrawVertexArray( int numVertexes, Vertex const* vertexes );

	Texture* CreateOrGetTextureFromFile( char const* imageFilePath );
	Texture* CreateTextureFromFile( char const* imageFilePath );
	Texture* CreateTextureFromData( char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData );
	Texture* GetTextureForFileName( char const* imageFilePath );
	void BindTexture( Texture* texture );


public:
	RenderConfig m_config;
	std::vector<Texture*> m_loadedTextures;
};