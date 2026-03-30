#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/Game.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
Map::Map( Game* game, MapDefinition const& mapDef )
	: m_game( game )
	, m_definition( &mapDef )
{
	m_shader = m_definition->m_shader;
	m_texture = m_definition->m_spriteSheetTexture;

	CreateTiles();
	CreateBuffers();
}


//-----------------------------------------------------------------------------------------------
Map::~Map()
{
	for ( Tile* tile : m_tiles )
	{
		delete tile;
	}
	m_tiles.clear();
}


//-----------------------------------------------------------------------------------------------
void Map::CreateTiles()
{
	Image const& mapImage = m_definition->m_image;
	m_dimensions = mapImage.GetDimensions();

	for ( int y = 0; y < m_dimensions.y; y++ )
	{
		for ( int x = 0; x < m_dimensions.x; x++ )
		{
			Rgba8 pixelColor = mapImage.GetTexelColor( IntVec2( x, y ) );
			TileDefinition const* tileDef = TileDefinition::GetTileDefinitionFromColor( pixelColor );
			if ( tileDef != nullptr )
			{
				AABB3 tileBounds = AABB3( Vec3( ( float ) x, ( float ) y, 0.f ), Vec3( ( float ) ( x + 1 ), ( float ) ( y + 1 ), 1.f ) );
				Tile* newTile = new Tile( tileBounds, tileDef );
				m_tiles.push_back( newTile );
			}
		}
	}

	CreateGeometry();
}


//-----------------------------------------------------------------------------------------------
void Map::CreateGeometry()
{
	for ( Tile* tile : m_tiles )
	{
		AABB3 const& bounds = tile->m_bounds;
		TileDefinition const& def = tile->GetDefinition();
		IntVec2 const& cellCount = m_definition->m_spriteSheetCellCount;

		float uvCellWidth = 1.f / ( float ) cellCount.x;
		float uvCellHeight = 1.f / ( float ) cellCount.y;

		if ( def.m_wallSpriteCoords != IntVec2::ZERO )
		{
			float uvMinX = ( float ) def.m_wallSpriteCoords.x * uvCellWidth;
			float uvMinY = ( float ) ( ( cellCount.y - 1 ) - def.m_wallSpriteCoords.y ) * uvCellHeight;
			float uvMaxX = uvMinX + uvCellWidth;
			float uvMaxY = uvMinY + uvCellHeight;

			AddGeometryForWall( bounds, AABB2( Vec2( uvMinX, uvMinY ), Vec2( uvMaxX, uvMaxY ) ) );
		}

		if ( def.m_floorSpriteCoords != IntVec2::ZERO )
		{
			float uvMinX = ( float ) def.m_floorSpriteCoords.x * uvCellWidth;
			float uvMinY = ( float ) ( ( cellCount.y - 1 ) - def.m_floorSpriteCoords.y ) * uvCellHeight;
			float uvMaxX = uvMinX + uvCellWidth;
			float uvMaxY = uvMinY + uvCellHeight;

			AddGeometryForFloor( bounds, AABB2( Vec2( uvMinX, uvMinY ), Vec2( uvMaxX, uvMaxY ) ) );
		}

		if ( def.m_ceilingSpriteCoords != IntVec2::ZERO )
		{
			float uvMinX = ( float ) def.m_ceilingSpriteCoords.x * uvCellWidth;
			float uvMinY = ( float ) ( ( cellCount.y - 1 ) - def.m_ceilingSpriteCoords.y ) * uvCellHeight;
			float uvMaxX = uvMinX + uvCellWidth;
			float uvMaxY = uvMinY + uvCellHeight;

			AddGeometryForCeiling( bounds, AABB2( Vec2( uvMinX, uvMinY ), Vec2( uvMaxX, uvMaxY ) ) );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::AddGeometryForWall( AABB3 const& bounds, AABB2 const& uvCoords )
{
	// Y min face
	AddVertsForQuad3D( m_verts, m_indices,
		Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ),
		Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ),
		Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ),
		Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ),
		Rgba8::WHITE, uvCoords );

	// Y max face
	AddVertsForQuad3D( m_verts, m_indices,
		Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ),
		Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ),
		Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ),
		Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ),
		Rgba8::WHITE, uvCoords );

	// X min face
	AddVertsForQuad3D( m_verts, m_indices,
		Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ),
		Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ),
		Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ),
		Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ),
		Rgba8::WHITE, uvCoords );

	// X max face
	AddVertsForQuad3D( m_verts, m_indices,
		Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ),
		Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ),
		Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ),
		Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ),
		Rgba8::WHITE, uvCoords );
}


//-----------------------------------------------------------------------------------------------
void Map::AddGeometryForFloor( AABB3 const& bounds, AABB2 const& uvCoords )
{
	AddVertsForQuad3D( m_verts, m_indices,
		Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ),
		Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ),
		Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ),
		Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ),
		Rgba8::WHITE, uvCoords );
}


//-----------------------------------------------------------------------------------------------
void Map::AddGeometryForCeiling( AABB3 const& bounds, AABB2 const& uvCoords )
{
	AddVertsForQuad3D( m_verts, m_indices,
		Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ),
		Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ),
		Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ),
		Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ),
		Rgba8::WHITE, uvCoords );
}


//-----------------------------------------------------------------------------------------------
void Map::CreateBuffers()
{
	if ( g_engine == nullptr || g_engine->m_renderer == nullptr )
	{
		return;
	}

	/*delete m_vertexBuffer;
	m_vertexBuffer = nullptr;
	delete m_indexBuffer;
	m_indexBuffer = nullptr;*/

	if ( m_verts.empty() || m_indices.empty() )
	{
		return;
	}

	unsigned int const vertexBufferSize = static_cast< unsigned int >( m_verts.size() * sizeof( Vertex_PCUTBN ) );
	unsigned int const indexBufferSize = static_cast< unsigned int >( m_indices.size() * sizeof( unsigned int ) );

	m_vertexBuffer = g_engine->m_renderer->CreateVertexBuffer( vertexBufferSize, sizeof( Vertex_PCUTBN ) );
	g_engine->m_renderer->CopyCPUToGPU( m_verts.data(), vertexBufferSize, m_vertexBuffer );

	m_indexBuffer = g_engine->m_renderer->CreateIndexBuffer( indexBufferSize );
	g_engine->m_renderer->CopyCPUToGPU( m_indices.data(), indexBufferSize, m_indexBuffer );

}


//-----------------------------------------------------------------------------------------------
void Map::Update()
{
}


//-----------------------------------------------------------------------------------------------
void Map::Render() const
{
	if ( g_engine == nullptr || g_engine->m_renderer == nullptr )
	{
		return;
	}

	if ( m_vertexBuffer == nullptr || m_indexBuffer == nullptr || m_indices.empty() )
	{
		return;
	}

	g_engine->m_renderer->SetModelConstants();
	g_engine->m_renderer->BindShader( m_shader );
	g_engine->m_renderer->BindTexture( m_texture );

	g_engine->m_renderer->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );

	g_engine->m_renderer->DrawIndexedVertexBuffer(
		m_vertexBuffer,
		m_indexBuffer,
		static_cast<unsigned int>( m_indices.size() ) );
}