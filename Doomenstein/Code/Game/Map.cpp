#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
Map::Map( MapDefinition const& mapDefinition, int mapIndex )
	: m_definition( &mapDefinition )
	, m_index( mapIndex )
{
	InitializeActors();
}


//-----------------------------------------------------------------------------------------------
Map::~Map()
{
	for ( Tile* tile : m_tiles )
	{
		delete tile;
	}
	m_tiles.clear();

	for ( Actor* actor : m_actors )
	{
		delete actor;
	}
	m_actors.clear();

	delete m_tileVertexBuffer;
	m_tileVertexBuffer = nullptr;

	delete m_tileIndexBuffer;
	m_tileIndexBuffer = nullptr;

	delete m_actorVertexBuffer;
	m_actorVertexBuffer = nullptr;

	delete m_actorIndexBuffer;
	m_actorIndexBuffer = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Map::Update()
{
	for ( Actor* actor : m_actors )
	{
		actor->Update();
	}
}


//-----------------------------------------------------------------------------------------------
void Map::Render() const
{
	for ( Actor* actor : m_actors )
	{
		actor->Render();
	}
}


//-----------------------------------------------------------------------------------------------
void Map::InitializeActors()
{
	Actor* enemyActorA = new Actor();
	enemyActorA->m_physicsRadius = 0.35f;
	enemyActorA->m_physicsHeight = 0.75f;
	enemyActorA->m_color = Rgba8::RED;
	enemyActorA->m_isStatic = true;
	enemyActorA->m_position = Vec3( 7.5f, 8.5f, 0.25f );
	m_actors.push_back( enemyActorA );

	Actor* enemyActorB = new Actor();
	enemyActorB->m_physicsRadius = 0.35f;
	enemyActorB->m_physicsHeight = 0.75f;
	enemyActorB->m_color = Rgba8::RED;
	enemyActorB->m_isStatic = true;
	enemyActorB->m_position = Vec3( 8.5f, 8.5f, 0.125f );
	m_actors.push_back( enemyActorB );

	Actor* enemyActorC = new Actor();
	enemyActorC->m_physicsRadius = 0.35f;
	enemyActorC->m_physicsHeight = 0.75f;
	enemyActorC->m_color = Rgba8::RED;
	enemyActorC->m_isStatic = true;
	enemyActorC->m_position = Vec3( 9.5f, 8.5f, 0.0f );
	m_actors.push_back( enemyActorC );

	Actor* projectileActor = new Actor();
	projectileActor->m_physicsRadius = 0.0625f;
	projectileActor->m_physicsHeight = 0.125f;
	projectileActor->m_color = Rgba8::BLUE;
	projectileActor->m_isStatic = false;
	projectileActor->m_position = Vec3( 5.5f, 8.5f, 0.0f );
	m_actors.push_back( projectileActor );
}


//-----------------------------------------------------------------------------------------------
void Map::PopulateMap()
{
	if ( m_definition == nullptr )
	{
		return;
	}

	for ( Tile* tile : m_tiles )
	{
		delete tile;
	}
	m_tiles.clear();

	Image mapImage( m_definition->m_imagePath.c_str() );
	m_dimensions = mapImage.GetDimensions();
	if ( m_dimensions.x <= 0 || m_dimensions.y <= 0 )
	{
		return;
	}

	m_tiles.reserve( m_dimensions.x * m_dimensions.y );

	for ( int tileY = 0; tileY < m_dimensions.y; ++tileY )
	{
		for ( int tileX = 0; tileX < m_dimensions.x; ++tileX )
		{
			IntVec2 tileCoords( tileX, tileY );
			Rgba8 pixelColor = mapImage.GetTexelColor( tileCoords );

			TileDefinition const* tileDef = TileDefinition::GetTileDefinitionFromColor( pixelColor );
			if ( tileDef == nullptr )
			{
				for ( auto const& defPair : TileDefinition::s_definitions )
				{
					if ( defPair.second != nullptr )
					{
						tileDef = defPair.second;
						break;
					}
				}
			}

			Tile* newTile = new Tile( tileCoords, tileDef );
			m_tiles.push_back( newTile );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::AddVertsForTiles( std::vector<Vertex>& verts )
{
	if ( m_tileVertexBuffer != nullptr )
	{
		delete m_tileVertexBuffer;
		m_tileVertexBuffer = nullptr;
	}
	if ( m_tileIndexBuffer != nullptr )
	{
		delete m_tileIndexBuffer;
		m_tileIndexBuffer = nullptr;
	}
	m_tileVertexCount = 0;

	if ( g_engine == nullptr || g_engine->m_renderer == nullptr || m_definition == nullptr || m_tiles.empty() )
	{
		return;
	}

	m_spriteTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( m_definition->m_spriteSheetTexturePath.c_str() );
	if ( m_spriteTexture == nullptr )
	{
		return;
	}
	SpriteSheet spriteSheet( *m_spriteTexture, m_definition->m_spriteSheetCellCount );
	std::vector<Vertex_PCUTBN> tileVerts;
	std::vector<unsigned int> tileIndices;

	for ( Tile* tile : m_tiles )
	{
		if ( tile == nullptr || tile->m_definition == nullptr )
		{
			continue;
		}
		
		float minX = ( float ) tile->m_tileCoords.x;
		float minY = ( float ) tile->m_tileCoords.y;
		float maxX = minX + 1.f;
		float maxY = minY + 1.f;
		float floorZ = 0.f;
		float ceilingZ = 1.f;

		TileDefinition const& tileDef = tile->GetDefinition();
	}
}


//-----------------------------------------------------------------------------------------------
Tile* Map::GetTileAtTileCoords( IntVec2 const& tileCoords ) const
{
	int tileIndex = tileCoords.y * m_definition->m_spriteSheetCellCount.x + tileCoords.x;
	if ( tileIndex < 0 || tileIndex >= (int)m_tiles.size() )
	{
		return nullptr;
	}
	return m_tiles[tileIndex];
}


//-----------------------------------------------------------------------------------------------
bool Map::IsTileSolidAtTileCoords( IntVec2 const& tileCoords ) const
{
	Tile* tile = GetTileAtTileCoords( tileCoords );
	if ( tile == nullptr )
	{
		return false;
	}
	return tile->IsSolid();
}