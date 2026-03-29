#pragma once
#include "Game/Tile.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class Actor;
class IndexBuffer;
class MapDefinition;
class Texture;
class VertexBuffer;


//-----------------------------------------------------------------------------------------------
class Map
{
public:
	Map( MapDefinition const& MapDefinition, int mapIndex );
	~Map();

	void Update();
	void Render() const;

private:
	void InitializeActors();
	void PopulateMap();

	void AddVertsForTiles( std::vector<Vertex>& verts );
	Tile* GetTileAtTileCoords( IntVec2 const& tileCoords ) const;
	bool IsTileSolidAtTileCoords( IntVec2 const& tileCoords ) const;

public:
	MapDefinition const* m_definition = nullptr;
	int m_index = -1;
	IntVec2 m_dimensions;

	std::vector<Tile*> m_tiles;
	std::vector<Actor*> m_actors;

	VertexBuffer* m_tileVertexBuffer = nullptr;
	IndexBuffer* m_tileIndexBuffer = nullptr;
	unsigned int m_tileVertexCount = 0;

	VertexBuffer* m_actorVertexBuffer = nullptr;
	IndexBuffer* m_actorIndexBuffer = nullptr;
	unsigned int m_actorVertexCount = 0;

	Texture* m_spriteTexture = nullptr;
};