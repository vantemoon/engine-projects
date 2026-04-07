#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/Game.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
Map::Map( Game* game, MapDefinition const& mapDef )
	: m_game( game )
	, m_definition( &mapDef )
{
	m_shader = m_definition->m_shader;
	m_texture = m_definition->m_spriteSheetTexture;

	CreateTiles();
	CreateTestActors();
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

	for ( Actor* actor : m_actors )
	{
		delete actor;
	}
	m_actors.clear();
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
void Map::CreateTestActors()
{
	Actor* enemyActor1 = new Actor();
	enemyActor1->m_physicsRadius = 0.35f;
	enemyActor1->m_physicsHeight = 0.75f;
	enemyActor1->m_color = Rgba8::RED;
	enemyActor1->m_isStatic = true;
	enemyActor1->m_position = Vec3( 7.5f, 8.5f, 0.25f );
	m_actors.push_back( enemyActor1 );

	Actor* enemyActor2 = new Actor();
	enemyActor2->m_physicsRadius = 0.35f;
	enemyActor2->m_physicsHeight = 0.75f;
	enemyActor2->m_color = Rgba8::RED;
	enemyActor2->m_isStatic = true;
	enemyActor2->m_position = Vec3( 8.5f, 8.5f, 0.125f );
	m_actors.push_back( enemyActor2 );

	Actor* enemyActor3 = new Actor();
	enemyActor3->m_physicsRadius = 0.35f;
	enemyActor3->m_physicsHeight = 0.75f;
	enemyActor3->m_color = Rgba8::RED;
	enemyActor3->m_isStatic = true;
	enemyActor3->m_position = Vec3( 9.5f, 8.5f, 0.0f );
	m_actors.push_back( enemyActor3 );

	Actor* projectileActor = new Actor();
	projectileActor->m_physicsRadius = 0.0625f;
	projectileActor->m_physicsHeight = 0.125f;
	projectileActor->m_color = Rgba8::BLUE;
	projectileActor->m_isStatic = false;
	projectileActor->m_position = Vec3( 5.5f, 8.5f, 0.0f );
	m_actors.push_back( projectileActor );
	m_fakeProjectileActor = projectileActor;
}


//-----------------------------------------------------------------------------------------------
Actor* Map::GetFakeProjectileActor() const
{
	return m_fakeProjectileActor;
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

	if ( m_verts.empty() || m_indices.empty() )
	{
		return;
	}

	unsigned int const vertexBufferSize = static_cast< unsigned int >( m_verts.size() * sizeof( Vertex ) );
	unsigned int const indexBufferSize = static_cast< unsigned int >( m_indices.size() * sizeof( unsigned int ) );

	m_vertexBuffer = g_engine->m_renderer->CreateVertexBuffer( vertexBufferSize, sizeof( Vertex ) );
	g_engine->m_renderer->CopyCPUToGPU( m_verts.data(), vertexBufferSize, m_vertexBuffer );

	m_indexBuffer = g_engine->m_renderer->CreateIndexBuffer( indexBufferSize );
	g_engine->m_renderer->CopyCPUToGPU( m_indices.data(), indexBufferSize, m_indexBuffer );
}


//-----------------------------------------------------------------------------------------------
void Map::Update()
{
	for ( Actor* actor : m_actors )
	{
		if ( actor != nullptr )
		{
			actor->Update();
		}
	}

	CollideActors();
	CollideActorWithMap();
}


//-----------------------------------------------------------------------------------------------
void Map::CollideActors()
{
	for ( size_t i = 0; i < m_actors.size(); i++ )
	{
		Actor* actorA = m_actors[i];
		if ( actorA == nullptr )
		{
			continue;
		}

		for ( size_t j = i + 1; j < m_actors.size(); j++ )
		{
			Actor* actorB = m_actors[j];
			if ( actorB == nullptr )
			{
				continue;
			}

			CollideActors( actorA, actorB );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::CollideActors( Actor* actorA, Actor* actorB )
{
	if ( actorA == nullptr || actorB == nullptr )
	{
		return;
	}

	float aMinZ = actorA->m_position.z;
	float aMaxZ = actorA->m_position.z + actorA->m_physicsHeight;
	float bMinZ = actorB->m_position.z;
	float bMaxZ = actorB->m_position.z + actorB->m_physicsHeight;

	bool zOverlaps = ( aMinZ < bMaxZ ) && ( bMinZ < aMaxZ );
	if ( !zOverlaps )
	{
		return;
	}

	Vec2 centerA = Vec2( actorA->m_position.x, actorA->m_position.y );
	Vec2 centerB = Vec2( actorB->m_position.x, actorB->m_position.y );

	if ( actorA->m_isStatic && actorB->m_isStatic )
	{
		return;
	}
	else if ( actorA->m_isStatic )
	{
		PushDiscOutOfFixedDisc2D( centerB, actorB->m_physicsRadius, centerA, actorA->m_physicsRadius );
	}
	else if ( actorB->m_isStatic )
	{
		PushDiscOutOfFixedDisc2D( centerA, actorA->m_physicsRadius, centerB, actorB->m_physicsRadius );
	}
	else
	{
		PushDiscsOutOfEachOther2D( centerA, actorA->m_physicsRadius, centerB, actorB->m_physicsRadius );
	}

	actorA->m_position.x = centerA.x;
	actorA->m_position.y = centerA.y;
	actorB->m_position.x = centerB.x;
	actorB->m_position.y = centerB.y;
}


//-----------------------------------------------------------------------------------------------
void Map::CollideActorWithMap()
{
	for ( Actor* actor : m_actors )
	{
		if ( actor != nullptr )
		{
			CollideActorWithMap( actor );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::CollideActorWithMap( Actor* actor )
{
	if ( actor == nullptr )
	{
		return;
	}

	Vec2 actorCenter = Vec2( actor->m_position.x, actor->m_position.y );
	int actorTileX = ( int ) RoundDownToInt( actorCenter.x );
	int actorTileY = ( int ) RoundDownToInt( actorCenter.y );

	Tile* currTile = GetTileAtCoords( actorTileX, actorTileY );
	if ( currTile != nullptr && currTile->IsSolid() )
	{
		AABB2 tileBounds2D = AABB2(
			Vec2( currTile->m_bounds.m_mins.x, currTile->m_bounds.m_mins.y ),
			Vec2( currTile->m_bounds.m_maxs.x, currTile->m_bounds.m_maxs.y ) );

		PushDiscOutOfFixedAABB2D( actorCenter, actor->m_physicsRadius, tileBounds2D );
	}

	IntVec2 const cardinalOffsets[4] =
	{
		IntVec2( 1, 0 ),
		IntVec2( -1, 0 ),
		IntVec2( 0, 1 ),
		IntVec2( 0, -1 )
	};

	for ( IntVec2 const& offset : cardinalOffsets )
	{
		Tile* tile = GetTileAtCoords( actorTileX + offset.x, actorTileY + offset.y );
		if ( tile == nullptr || !tile->IsSolid() )
		{
			continue;
		}

		AABB2 tileBounds2D = AABB2(
			Vec2( tile->m_bounds.m_mins.x, tile->m_bounds.m_mins.y ),
			Vec2( tile->m_bounds.m_maxs.x, tile->m_bounds.m_maxs.y ) );

		PushDiscOutOfFixedAABB2D( actorCenter, actor->m_physicsRadius, tileBounds2D );
	}

	IntVec2 const diagonalOffsets[4] =
	{
		IntVec2( 1, 1 ),
		IntVec2( 1, -1 ),
		IntVec2( -1, 1 ),
		IntVec2( -1, -1 )
	};

	for ( IntVec2 const& offset : diagonalOffsets )
	{
		Tile* tile = GetTileAtCoords( actorTileX + offset.x, actorTileY + offset.y );
		if ( tile == nullptr || !tile->IsSolid() )
		{
			continue;
		}

		AABB2 tileBounds2D = AABB2(
			Vec2( tile->m_bounds.m_mins.x, tile->m_bounds.m_mins.y ),
			Vec2( tile->m_bounds.m_maxs.x, tile->m_bounds.m_maxs.y ) );

		PushDiscOutOfFixedAABB2D( actorCenter, actor->m_physicsRadius, tileBounds2D );
	}

	actor->m_position.x = actorCenter.x;
	actor->m_position.y = actorCenter.y;

	float actorTopZ = actor->m_position.z + actor->m_physicsHeight;
	if ( actorTopZ > 1.f )
	{
		actor->m_position.z -= ( actorTopZ - 1.f );
	}
	if ( actor->m_position.z < 0.f )
	{
		actor->m_position.z = 0.f;
	}
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

	Vec3 const normalizedSunDirection = Vec3( m_sunDirection[0], m_sunDirection[1], m_sunDirection[2] ).GetNormalized();
	g_engine->m_renderer->SetLightingConstants( normalizedSunDirection, m_sunIntensity, m_ambientIntensity );

	g_engine->m_renderer->BindShader( m_shader );
	g_engine->m_renderer->BindTexture( m_texture );

	g_engine->m_renderer->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );

	g_engine->m_renderer->DrawIndexedVertexBuffer(
		m_vertexBuffer,
		m_indexBuffer,
		static_cast< unsigned int >( m_indices.size() ) );

	g_engine->m_renderer->BindShader( nullptr );
	g_engine->m_renderer->BindTexture( nullptr );

	for ( Actor* actor : m_actors )
	{
		if ( actor != nullptr )
		{
			actor->Render();
		}
	}
}


//-----------------------------------------------------------------------------------------------
RaycastResult3D Map::RaycastAll( Vec3 const& startPos, Vec3 const& forwardNormal, float maxLength, Actor* owner ) const
{
	RaycastResult3D result;
	float closestImpactDist = maxLength;

	// Raycast against actors
	RaycastResult3D actorResult = RaycastWorldActors( startPos, forwardNormal, maxLength, owner );
	if ( actorResult.m_didImpact && actorResult.m_impactDist < closestImpactDist )
	{
		result = actorResult;
		closestImpactDist = actorResult.m_impactDist;
	}

	// Raycast against floor and ceiling
	RaycastResult3D floorCeilingResult = RaycastWorldZ( startPos, forwardNormal, maxLength );
	if ( floorCeilingResult.m_didImpact && floorCeilingResult.m_impactDist < closestImpactDist )
	{
		result = floorCeilingResult;
		closestImpactDist = floorCeilingResult.m_impactDist;
	}

	// Raycast against walls
	RaycastResult3D wallResult = RaycastWorldXY( startPos, forwardNormal, maxLength );
	if ( wallResult.m_didImpact && wallResult.m_impactDist < closestImpactDist )
	{
		result = wallResult;
		closestImpactDist = wallResult.m_impactDist;
	}

	return result;
}


//-----------------------------------------------------------------------------------------------
RaycastResult3D Map::RaycastWorldXY( Vec3 const& startPos, Vec3 const& forwardNormal, float maxLength ) const
{
	RaycastResult3D result;
	result.m_rayStartPos = startPos;
	result.m_rayFwdNormal = forwardNormal;
	result.m_rayMaxLength = maxLength;
	result.m_didImpact = false;
	result.m_impactDist = maxLength;
	result.m_impactPos = startPos + ( forwardNormal * maxLength );
	result.m_impactNormal = Vec3( 0.f, 0.f, 0.f );

	if ( maxLength <= 0.f )
	{
		return result;
	}

	IntVec2 tileCoords( ( int ) RoundDownToInt( startPos.x ), ( int ) RoundDownToInt( startPos.y ) );
	Tile* tile = GetTileAtCoords( tileCoords.x, tileCoords.y );
	if ( tile != nullptr && tile->IsSolid() )
	{
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = Vec3( -forwardNormal.x, -forwardNormal.y, 0.f ).GetNormalized();
		return result;
	}

	float fwdDistPerXCrossing = 0.f;
	float fwdDistAtNextXCrossing = maxLength + 1.f;
	int tileStepDirectionX = 0;

	if ( forwardNormal.x > 0.f )
	{
		tileStepDirectionX = 1;
		float xAtFirstXCrossing = ( float ) ( tileCoords.x + 1 );
		fwdDistPerXCrossing = 1.f / fabsf( forwardNormal.x );
		fwdDistAtNextXCrossing = ( xAtFirstXCrossing - startPos.x ) * fwdDistPerXCrossing;
	}
	else if ( forwardNormal.x < 0.f )
	{
		tileStepDirectionX = -1;
		float xAtFirstXCrossing = ( float ) tileCoords.x;
		fwdDistPerXCrossing = 1.f / fabsf( forwardNormal.x );
		fwdDistAtNextXCrossing = ( startPos.x - xAtFirstXCrossing ) * fwdDistPerXCrossing;
	}

	float fwdDistPerYCrossing = 0.f;
	float fwdDistAtNextYCrossing = maxLength + 1.f;
	int tileStepDirectionY = 0;

	if ( forwardNormal.y > 0.f )
	{
		tileStepDirectionY = 1;
		float yAtFirstYCrossing = ( float ) ( tileCoords.y + 1 );
		fwdDistPerYCrossing = 1.f / fabsf( forwardNormal.y );
		fwdDistAtNextYCrossing = ( yAtFirstYCrossing - startPos.y ) * fwdDistPerYCrossing;
	}
	else if ( forwardNormal.y < 0.f )
	{
		tileStepDirectionY = -1;
		float yAtFirstYCrossing = ( float ) tileCoords.y;
		fwdDistPerYCrossing = 1.f / fabsf( forwardNormal.y );
		fwdDistAtNextYCrossing = ( startPos.y - yAtFirstYCrossing ) * fwdDistPerYCrossing;
	}

	if ( tileStepDirectionX == 0 && tileStepDirectionY == 0 )
	{
		return result;
	}

	while ( true )
	{
		bool stepX = ( fwdDistAtNextXCrossing <= fwdDistAtNextYCrossing );

		float distOfNextCrossing = 0.f;
		Vec3 impactSurfaceNormal;

		if ( stepX )
		{
			distOfNextCrossing = fwdDistAtNextXCrossing;
			if ( distOfNextCrossing > maxLength )
			{
				return result;
			}

			tileCoords.x += tileStepDirectionX;
			fwdDistAtNextXCrossing += fwdDistPerXCrossing;
			impactSurfaceNormal = Vec3( ( float ) -tileStepDirectionX, 0.f, 0.f );
		}
		else
		{
			distOfNextCrossing = fwdDistAtNextYCrossing;
			if ( distOfNextCrossing > maxLength )
			{
				return result;
			}

			tileCoords.y += tileStepDirectionY;
			fwdDistAtNextYCrossing += fwdDistPerYCrossing;
			impactSurfaceNormal = Vec3( 0.f, ( float ) -tileStepDirectionY, 0.f );
		}

		tile = GetTileAtCoords( tileCoords.x, tileCoords.y );
		if ( tile != nullptr && tile->IsSolid() )
		{
			result.m_didImpact = true;
			result.m_impactDist = distOfNextCrossing;
			result.m_impactPos = startPos + ( forwardNormal * distOfNextCrossing );
			result.m_impactNormal = impactSurfaceNormal;
			return result;
		}
	}
}


//-----------------------------------------------------------------------------------------------
RaycastResult3D Map::RaycastWorldZ( Vec3 const& startPos, Vec3 const& forwardNormal, float maxLength ) const
{
	RaycastResult3D result;
	result.m_rayStartPos = startPos;
	result.m_rayFwdNormal = forwardNormal;
	result.m_rayMaxLength = maxLength;
	result.m_didImpact = false;

	if ( maxLength <= 0.f )
	{
		return result;
	}

	Vec3 rayVector = forwardNormal * maxLength;
	float rayZ = rayVector.z;

	if ( rayZ > 0.f )
	{
		float t = ( 1.f - startPos.z ) / rayZ;
		if ( t >= 0.f && t <= 1.f )
		{
			result.m_didImpact = true;
			result.m_impactDist = t * maxLength;
			result.m_impactPos = startPos + rayVector * t;
			result.m_impactNormal = Vec3( 0.f, 0.f, -1.f );
		}
	}
	else if ( rayZ < 0.f )
	{
		float t = ( -startPos.z ) / rayZ;
		if ( t >= 0.f && t <= 1.f )
		{
			result.m_didImpact = true;
			result.m_impactDist = t * maxLength;
			result.m_impactPos = startPos + rayVector * t;
			result.m_impactNormal = Vec3( 0.f, 0.f, 1.f );
		}
	}

	return result;
}


//-----------------------------------------------------------------------------------------------
RaycastResult3D Map::RaycastWorldActors( Vec3 const& startPos, Vec3 const& forwardNormal, float maxLength, Actor* owner ) const
{
	RaycastResult3D result;
	float closestImpactDist = maxLength;

	for ( Actor* actor : m_actors )
	{
		if ( actor == nullptr || actor == owner )
		{
			continue;
		}

		Vec2 actorCenter = Vec2( actor->m_position.x, actor->m_position.y );
		float actorMinZ = actor->m_position.z;
		float actorMaxZ = actor->m_position.z + actor->m_physicsHeight;
		RaycastResult3D actorResult = RaycastVsCylinderZ3D( startPos, forwardNormal, maxLength, actorCenter, actorMinZ, actorMaxZ, actor->m_physicsRadius );

		if ( actorResult.m_didImpact && actorResult.m_impactDist < closestImpactDist )
		{
			result = actorResult;
			closestImpactDist = actorResult.m_impactDist;
		}
	}

	return result;
}


//-----------------------------------------------------------------------------------------------
Vec3 Map::GetSunDirection() const
{
	return Vec3( m_sunDirection[0], m_sunDirection[1], m_sunDirection[2] );
}


//-----------------------------------------------------------------------------------------------
float Map::GetSunIntensity() const
{
	return m_sunIntensity;
}


//-----------------------------------------------------------------------------------------------
float Map::GetAmbientIntensity() const
{
	return m_ambientIntensity;
}


//-----------------------------------------------------------------------------------------------
void Map::AddToSunDirectionX( float delta )
{
	m_sunDirection[0] += delta;
}


//-----------------------------------------------------------------------------------------------
void Map::AddToSunDirectionY( float delta )
{
	m_sunDirection[1] += delta;
}


//-----------------------------------------------------------------------------------------------
void Map::AddToSunIntensity( float delta )
{
	m_sunIntensity = GetClamped( m_sunIntensity + delta, 0.f, 1.f );
}


//-----------------------------------------------------------------------------------------------
void Map::AddToAmbientIntensity( float delta )
{
	m_ambientIntensity = GetClamped( m_ambientIntensity + delta, 0.f, 1.f );
}

//-----------------------------------------------------------------------------------------------
bool Map::IsPositionInBounds( Vec3 const& position ) const
{
	return AreCoordsInBounds( ( int ) position.x, ( int ) position.y );
}


//-----------------------------------------------------------------------------------------------
bool Map::AreCoordsInBounds( int x, int y ) const
{
	if ( x < 0 || x >= m_dimensions.x || y < 0 || y >= m_dimensions.y )
	{
		return false;
	}
	return true;
}


//-----------------------------------------------------------------------------------------------
Tile* const Map::GetTileAtCoords( int x, int y ) const
{
	if ( !AreCoordsInBounds( x, y ) )
	{
		return nullptr;
	}
	int tileIndex = y * m_dimensions.x + x;
	return m_tiles[tileIndex];
}