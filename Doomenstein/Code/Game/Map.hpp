#pragma once
#include "Game/Tile.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/RaycastUtils.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class Actor;
class Game;
class IndexBuffer;
class MapDefinition;
class Shader;
class Texture;
class VertexBuffer;


//-----------------------------------------------------------------------------------------------
class Map
{
public:
	Map( Game* game, MapDefinition const& MapDefinition );
	~Map();

	void CreateTiles();
	void CreateGeometry();
	void AddGeometryForWall( AABB3 const& bounds, AABB2 const& uvCoords );
	void AddGeometryForFloor( AABB3 const& bounds, AABB2 const& uvCoords );
	void AddGeometryForCeiling( AABB3 const& bounds, AABB2 const& uvCoords );
	void CreateBuffers();
	void CreateTestActors();

	bool IsPositionInBounds( Vec3 const& position ) const;
	bool AreCoordsInBounds( int x, int y ) const;
	Tile* const GetTileAtCoords( int x, int y ) const;
	Actor* GetFakeProjectileActor() const;

	void Update();
	void CollideActors();
	void CollideActors( Actor* actorA, Actor* actorB );
	void CollideActorWithMap();
	void CollideActorWithMap( Actor* actor );

	void Render() const;

	RaycastResult3D RaycastAll( Vec3 const& startPos, Vec3 const& forwardNormal, float maxLength, Actor* owner = nullptr ) const;
	RaycastResult3D RaycastWorldXY( Vec3 const& startPos, Vec3 const& forwardNormal, float maxLength ) const;
	RaycastResult3D RaycastWorldZ( Vec3 const& startPos, Vec3 const& forwardNormal, float maxLength ) const;
	RaycastResult3D RaycastWorldActors( Vec3 const& startPos, Vec3 const& forwardNormal, float maxLength, Actor* owner = nullptr ) const;

	Vec3 GetSunDirection() const;
	float GetSunIntensity() const;
	float GetAmbientIntensity() const;
	void AddToSunDirectionX( float delta );
	void AddToSunDirectionY( float delta );
	void AddToSunIntensity( float delta );
	void AddToAmbientIntensity( float delta );

protected:
	Game* m_game = nullptr;

	MapDefinition const* m_definition = nullptr;
	std::vector<Tile*> m_tiles;
	IntVec2 m_dimensions;

	std::vector<Actor*> m_actors;
	Actor* m_fakeProjectileActor = nullptr;
	unsigned int m_nextActorUID = 0;
	
	std::vector<Vertex> m_verts;
	std::vector<unsigned int> m_indices;
	Texture* m_texture = nullptr;
	Shader* m_shader = nullptr;
	VertexBuffer* m_vertexBuffer = nullptr;
	IndexBuffer* m_indexBuffer = nullptr;

	float m_sunDirection[3] = { 2.f, 1.f, -1.f };
	float m_sunIntensity = 0.85f;
	float m_ambientIntensity = 0.35f;
};