#include "Game/Map.hpp"
#include "Game/AI.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
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

	for ( SpawnInfo const& spawnInfo : m_definition->m_spawnInfos )
	{
		if ( spawnInfo.m_actor == "SpawnPoint" )
		{
			continue;
		}

		SpawnActor( spawnInfo );
	}
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

	delete m_vertexBuffer;
	m_vertexBuffer = nullptr;

	delete m_indexBuffer;
	m_indexBuffer = nullptr;
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

	if ( m_verts.empty() || m_indices.empty() )
	{
		return;
	}

	delete m_vertexBuffer;
	m_vertexBuffer = nullptr;

	delete m_indexBuffer;
	m_indexBuffer = nullptr;

	unsigned int const vertexBufferSize = static_cast<unsigned int>( m_verts.size() * sizeof( Vertex ) );
	unsigned int const indexBufferSize = static_cast<unsigned int>( m_indices.size() * sizeof( unsigned int ) );

	m_vertexBuffer = g_engine->m_renderer->CreateVertexBuffer( vertexBufferSize, sizeof( Vertex ) );
	g_engine->m_renderer->CopyCPUToGPU( m_verts.data(), vertexBufferSize, m_vertexBuffer );

	m_indexBuffer = g_engine->m_renderer->CreateIndexBuffer( indexBufferSize );
	g_engine->m_renderer->CopyCPUToGPU( m_indices.data(), indexBufferSize, m_indexBuffer );
}


//-----------------------------------------------------------------------------------------------
void Map::Update( float deltaSeconds )
{
	for ( Actor* actor : m_actors )
	{
		if ( actor == nullptr )
		{
			continue;
		}

		if ( actor->m_aiController != nullptr && actor->m_controller == actor->m_aiController )
		{
			actor->m_aiController->Update( deltaSeconds );
		}

		actor->Update();
	}

	CollideActors();
	CollideActorWithMap();

	if ( m_game != nullptr )
	{
		for ( int playerIndex = 0; playerIndex < m_game->m_players.size(); playerIndex++ )
		{
			Player* player = m_game->m_players[playerIndex];
			if ( player == nullptr )
			{
				continue;
			}

			Actor* actor = player->GetActor();
			if ( actor != nullptr && actor->m_isDead && actor->m_isDestroyed )
			{
				RespawnPlayer( player );
			}
		}
	}

	DeleteDestroyedActors();
}


//-----------------------------------------------------------------------------------------------
void Map::CollideActors()
{
	for ( size_t i = 0; i < m_actors.size(); i++ )
	{
		Actor* actorA = m_actors[i];
		if ( actorA == nullptr || actorA->m_isDead || actorA->m_isDestroyed )
		{
			continue;
		}

		for ( size_t j = i + 1; j < m_actors.size(); j++ )
		{
			Actor* actorB = m_actors[j];
			if ( actorB == nullptr || actorB->m_isDead || actorB->m_isDestroyed )
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

	if ( actorA->m_handle == actorB->m_ownerHandle || actorB->m_handle == actorA->m_ownerHandle )
	{
		return;
	}

	if ( actorA->m_definition == nullptr || actorB->m_definition == nullptr )
	{
		return;
	}

	if ( !actorA->m_definition->m_collideWithActors || !actorB->m_definition->m_collideWithActors )
	{
		return;
	}

	if ( !actorA->m_definition->m_isSimulated && !actorB->m_definition->m_isSimulated )
	{
		return;
	}

	float aMinZ = actorA->m_position.z;
	float aMaxZ = actorA->m_position.z + actorA->m_definition->m_physicsHeight;
	float bMinZ = actorB->m_position.z;
	float bMaxZ = actorB->m_position.z + actorB->m_definition->m_physicsHeight;

	float overlapZ = fminf( aMaxZ, bMaxZ ) - fmaxf( aMinZ, bMinZ );
	if ( overlapZ <= 0.f )
	{
		return;
	}

	Vec2 centerA( actorA->m_position.x, actorA->m_position.y );
	Vec2 centerB( actorB->m_position.x, actorB->m_position.y );

	Vec2 fromAToB = centerB - centerA;
	float distSqXY = fromAToB.GetLengthSquared();
	float radiusSum = actorA->m_definition->m_physicsRadius + actorB->m_definition->m_physicsRadius;
	float radiusSumSq = radiusSum * radiusSum;
	if ( distSqXY >= radiusSumSq )
	{
		return;
	}

	float distXY = sqrtf( distSqXY );
	float overlapXY = radiusSum - distXY;

	bool resolveAlongZ = overlapZ < overlapXY;

	if ( resolveAlongZ )
	{
		float aCenterZ = aMinZ + ( actorA->m_definition->m_physicsHeight * 0.5f );
		float bCenterZ = bMinZ + ( actorB->m_definition->m_physicsHeight * 0.5f );

		float pushSignForB = ( bCenterZ >= aCenterZ ) ? 1.f : -1.f;

		if ( !actorA->m_definition->m_isSimulated )
		{
			actorB->m_position.z += pushSignForB * overlapZ;
		}
		else if ( !actorB->m_definition->m_isSimulated )
		{
			actorA->m_position.z -= pushSignForB * overlapZ;
		}
		else
		{
			float halfPush = overlapZ * 0.5f;
			actorA->m_position.z -= pushSignForB * halfPush;
			actorB->m_position.z += pushSignForB * halfPush;
		}

		Vec3 collisionNormalAtoB( 0.f, 0.f, pushSignForB );
		actorA->OnCollide( actorB, collisionNormalAtoB );
		actorB->OnCollide( actorA, -collisionNormalAtoB );
		return;
	}

	if ( !actorA->m_definition->m_isSimulated )
	{
		PushDiscOutOfFixedDisc2D( centerB, actorB->m_definition->m_physicsRadius, centerA, actorA->m_definition->m_physicsRadius );
	}
	else if ( !actorB->m_definition->m_isSimulated )
	{
		PushDiscOutOfFixedDisc2D( centerA, actorA->m_definition->m_physicsRadius, centerB, actorB->m_definition->m_physicsRadius );
	}
	else
	{
		PushDiscsOutOfEachOther2D( centerA, actorA->m_definition->m_physicsRadius, centerB, actorB->m_definition->m_physicsRadius );
	}

	actorA->m_position.x = centerA.x;
	actorA->m_position.y = centerA.y;
	actorB->m_position.x = centerB.x;
	actorB->m_position.y = centerB.y;

	Vec2 collisionDirXY = fromAToB;
	if ( collisionDirXY == Vec2::ZERO )
	{
		collisionDirXY = Vec2( 1.f, 0.f );
	}
	else
	{
		collisionDirXY.Normalize();
	}

	Vec3 collisionNormalAtoB( collisionDirXY.x, collisionDirXY.y, 0.f );
	actorA->OnCollide( actorB, collisionNormalAtoB );
	actorB->OnCollide( actorA, -collisionNormalAtoB );
}


//-----------------------------------------------------------------------------------------------
void Map::CollideActorWithMap()
{
	for ( Actor* actor : m_actors )
	{
		if ( actor != nullptr && !actor->m_isDead && !actor->m_isDestroyed )
		{
			CollideActorWithMap( actor );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::CollideActorWithMap( Actor* actor )
{
	if ( actor == nullptr || actor->m_definition == nullptr || !actor->m_definition->m_collideWithWorld )
	{
		return;
	}

	Vec3 const oldPosition = actor->m_position;
	Vec2 actorCenter = Vec2( actor->m_position.x, actor->m_position.y );
	int actorTileX = ( int ) RoundDownToInt( actorCenter.x );
	int actorTileY = ( int ) RoundDownToInt( actorCenter.y );

	Tile* currTile = GetTileAtCoords( actorTileX, actorTileY );
	if ( currTile != nullptr && currTile->IsSolid() )
	{
		AABB2 tileBounds2D = AABB2(
			Vec2( currTile->m_bounds.m_mins.x, currTile->m_bounds.m_mins.y ),
			Vec2( currTile->m_bounds.m_maxs.x, currTile->m_bounds.m_maxs.y ) );

		PushDiscOutOfFixedAABB2D( actorCenter, actor->m_definition->m_physicsRadius, tileBounds2D );
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

		PushDiscOutOfFixedAABB2D( actorCenter, actor->m_definition->m_physicsRadius, tileBounds2D );
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

		PushDiscOutOfFixedAABB2D( actorCenter, actor->m_definition->m_physicsRadius, tileBounds2D );
	}

	actor->m_position.x = actorCenter.x;
	actor->m_position.y = actorCenter.y;

	float actorTopZ = actor->m_position.z + actor->m_definition->m_physicsHeight;
	if ( actorTopZ > 1.f )
	{
		actor->m_position.z -= ( actorTopZ - 1.f );
	}
	if ( actor->m_position.z < 0.f )
	{
		actor->m_position.z = 0.f;
	}

	if ( actor->m_position != oldPosition )
	{
		actor->OnCollide();
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

	Actor* possessedActorToHide = nullptr;

	if ( m_game != nullptr && m_game->m_currentRenderingPlayer != nullptr )
	{
		Player* renderingPlayer = m_game->m_currentRenderingPlayer;

		if ( renderingPlayer->m_cameraMode == CameraMode::FIRST_PERSON )
		{
			possessedActorToHide = renderingPlayer->GetActor();
		}
	}

	for ( Actor* actor : m_actors )
	{
		if ( actor == nullptr )
		{
			continue;
		}

		if ( actor == possessedActorToHide )
		{
			continue;
		}

		actor->Render();
	}
}


//-----------------------------------------------------------------------------------------------
RaycastResult3D Map::RaycastAll( Vec3 const& startPos, Vec3 const& forwardNormal, float maxLength, Actor* owner, Actor** outHitActor ) const
{
	if ( outHitActor != nullptr )
	{
		*outHitActor = nullptr;
	}

	RaycastResult3D result;
	float closestImpactDist = maxLength;
	Actor* closestHitActor = nullptr;

	AABB3 mapBounds = AABB3( Vec3( 0.f, 0.f, 0.f ), Vec3( ( float ) m_dimensions.x, ( float ) m_dimensions.y, 1.f ) );

	Actor* actorHit = nullptr;
	RaycastResult3D actorResult = RaycastWorldActors( startPos, forwardNormal, maxLength, owner, &actorHit );
	if ( actorResult.m_didImpact
		&& actorResult.m_impactDist < closestImpactDist
		&& mapBounds.IsPointInside( actorResult.m_impactPos ) )
	{
		result = actorResult;
		closestImpactDist = actorResult.m_impactDist;
		closestHitActor = actorHit;
	}

	RaycastResult3D floorCeilingResult = RaycastWorldZ( startPos, forwardNormal, maxLength );
	if ( floorCeilingResult.m_didImpact
		&& floorCeilingResult.m_impactDist < closestImpactDist
		&& mapBounds.IsPointInside( floorCeilingResult.m_impactPos ) )
	{
		result = floorCeilingResult;
		closestImpactDist = floorCeilingResult.m_impactDist;
		closestHitActor = nullptr;
	}

	RaycastResult3D wallResult = RaycastWorldXY( startPos, forwardNormal, maxLength );
	if ( wallResult.m_didImpact
		&& wallResult.m_impactDist < closestImpactDist
		&& mapBounds.IsPointInside( wallResult.m_impactPos ) )
	{
		result = wallResult;
		closestImpactDist = wallResult.m_impactDist;
		closestHitActor = nullptr;
	}

	if ( outHitActor != nullptr )
	{
		*outHitActor = closestHitActor;
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
RaycastResult3D Map::RaycastWorldActors( Vec3 const& startPos, Vec3 const& forwardNormal, float maxLength, Actor* owner, Actor** outHitActor ) const
{
	if ( outHitActor != nullptr )
	{
		*outHitActor = nullptr;
	}

	RaycastResult3D result;
	float closestImpactDist = maxLength;
	Actor* closestHitActor = nullptr;

	for ( Actor* actor : m_actors )
	{
		if ( actor == nullptr || actor == owner || actor->m_isDead || actor->m_isDestroyed || actor->m_definition == nullptr )
		{
			continue;
		}

		Vec2 actorCenter = Vec2( actor->m_position.x, actor->m_position.y );
		float actorMinZ = actor->m_position.z;
		float actorMaxZ = actor->m_position.z + actor->m_definition->m_physicsHeight;
		RaycastResult3D actorResult = RaycastVsCylinderZ3D( startPos, forwardNormal, maxLength, actorCenter, actorMinZ, actorMaxZ, actor->m_definition->m_physicsRadius );

		if ( actorResult.m_didImpact && actorResult.m_impactDist < closestImpactDist )
		{
			result = actorResult;
			closestImpactDist = actorResult.m_impactDist;
			closestHitActor = actor;
		}
	}

	if ( outHitActor != nullptr )
	{
		*outHitActor = closestHitActor;
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


//-----------------------------------------------------------------------------------------------
void Map::SpawnPlayer( Player* player )
{
	if ( player == nullptr || m_definition == nullptr )
	{
		return;
	}

	std::vector<SpawnInfo const*> spawnPoints;
	spawnPoints.reserve( m_definition->m_spawnInfos.size() );

	for ( SpawnInfo const& spawnInfo : m_definition->m_spawnInfos )
	{
		if ( spawnInfo.m_actor == "SpawnPoint" )
		{
			spawnPoints.push_back( &spawnInfo );
		}
	}

	if ( spawnPoints.empty() )
	{
		return;
	}

	RandomNumberGenerator rng;
	int const spawnIndex = rng.RollRandomIntLessThan( ( int ) spawnPoints.size() );

	SpawnInfo marineSpawnInfo = *spawnPoints[spawnIndex];
	marineSpawnInfo.m_actor = "Marine";
	marineSpawnInfo.m_velocity = Vec3::ZERO;

	Actor* marineActor = SpawnActor( marineSpawnInfo );
	if ( marineActor == nullptr )
	{
		return;
	}

	player->m_map = this;
	player->m_possessedActor = ActorHandle::INVALID;
	player->Possess( marineActor->m_handle );

	player->m_cameraMode = CameraMode::FIRST_PERSON;
	player->m_orientation = marineActor->m_orientation;
	player->m_orientation.m_rollDegrees = 0.f;
	player->m_position = marineActor->m_position;
	player->m_position.z += marineActor->m_definition->m_eyeHeight;
}


//-----------------------------------------------------------------------------------------------
void Map::RespawnPlayer( Player* player )
{
	if ( player == nullptr )
	{
		return;
	}

	Actor* oldActor = player->GetActor();
	if ( oldActor != nullptr )
	{
		oldActor->OnUnpossessed();
		oldActor->m_isDestroyed = true;
	}

	player->m_possessedActor = ActorHandle::INVALID;

	SpawnPlayer( player );
}


//-----------------------------------------------------------------------------------------------
Actor* Map::SpawnActor( SpawnInfo const& spawnInfo )
{
	std::string const& actorName = spawnInfo.m_actor;
	ActorDefinition const* actorDef = ActorDefinition::GetActorDefinitionByName( actorName );
	if ( actorDef == nullptr )
	{
		return nullptr;
	}

	int actorIndex = -1;
	for ( int index = 0; index < ( int ) m_actors.size(); index++ )
	{
		if ( m_actors[index] == nullptr )
		{
			actorIndex = index;
			break;
		}
	}

	if ( actorIndex < 0 )
	{
		unsigned int const newIndex = static_cast<unsigned int>( m_actors.size() );
		GUARANTEE_OR_DIE(
			newIndex <= ActorHandle::MAX_ACTOR_INDEX,
			"Map::SpawnActor exceeded ActorHandle::MAX_ACTOR_INDEX (too many actors)." );

		actorIndex = ( int ) newIndex;
		m_actors.push_back( nullptr );
	}

	unsigned int const uidStep = ( ( ActorHandle::MAX_ACTOR_UID & 1u ) == 0u ) ? 2u : 1u;
	GUARANTEE_OR_DIE(
		m_nextActorUID <= ( ActorHandle::MAX_ACTOR_UID - uidStep ),
		"Map::SpawnActor exceeded ActorHandle::MAX_ACTOR_UID (no more unique actor handles)." );

	m_nextActorUID += uidStep;

	ActorHandle const newHandle = ActorHandle( m_nextActorUID, static_cast<unsigned int>( actorIndex ) );
	GUARANTEE_OR_DIE(
		newHandle.IsValid(),
		"Map::SpawnActor generated an invalid ActorHandle." );

	Actor* newActor = new Actor( newHandle, actorDef, this );
	newActor->m_position = spawnInfo.m_position;
	newActor->m_orientation = EulerAngles( spawnInfo.m_orientation.x, spawnInfo.m_orientation.y, spawnInfo.m_orientation.z );
	newActor->m_velocity = spawnInfo.m_velocity;

	if ( actorName == "Marine" )
	{
		newActor->SetSolidColor( Rgba8::GREEN );
	}
	else if ( actorName == "Demon" )
	{
		newActor->SetSolidColor( Rgba8::RED );
	}
	else
	{
		newActor->SetSolidColor( Rgba8::BLUE );
	}

	m_actors[actorIndex] = newActor;

	return newActor;
}


//-----------------------------------------------------------------------------------------------
Actor* Map::GetClosestVisibleEnemy( Actor* seeker )
{
	if ( seeker == nullptr || seeker->m_definition == nullptr )
	{
		return nullptr;
	}

	ActorDefinition const* seekerDef = seeker->m_definition;
	float const sightRadius = seekerDef->m_sightRadius;
	if ( sightRadius <= 0.f )
	{
		return nullptr;
	}

	Vec3 seekerEyePos = seeker->m_position;
	seekerEyePos.z += seekerDef->m_eyeHeight;

	Vec3 seekerForward3D = seeker->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
	Vec2 seekerForwardXY = Vec2( seekerForward3D.x, seekerForward3D.y );
	bool hasForwardXY = seekerForwardXY != Vec2::ZERO;
	if ( hasForwardXY )
	{
		seekerForwardXY.Normalize();
	}

	float const sightRadiusSq = sightRadius * sightRadius;
	float const halfSightAngle = seekerDef->m_sightAngle * 0.5f;

	Actor* closestEnemy = nullptr;
	float closestDistSq = FLT_MAX;

	for ( Actor* other : m_actors )
	{
		if ( other == nullptr || other == seeker || other->m_isDead || other->m_isDestroyed || other->m_definition == nullptr )
		{
			continue;
		}

		if ( seekerDef->m_faction == other->m_definition->m_faction )
		{
			continue;
		}
		if ( seekerDef->m_faction == "NEUTRAL" || other->m_definition->m_faction == "NEUTRAL" )
		{
			continue;
		}

		Vec3 targetEyePos = other->m_position;
		targetEyePos.z += other->m_definition->m_eyeHeight;

		Vec3 toTarget = targetEyePos - seekerEyePos;
		float const distSq = toTarget.GetLengthSquared();
		if ( distSq > sightRadiusSq || distSq >= closestDistSq )
		{
			continue;
		}

		if ( seekerDef->m_sightAngle > 0.f && hasForwardXY )
		{
			Vec2 toTargetXY = Vec2( toTarget.x, toTarget.y );
			if ( toTargetXY != Vec2::ZERO )
			{
				toTargetXY.Normalize();
				float const angleToTarget = GetAngleDegreesBetweenVectors2D( seekerForwardXY, toTargetXY );
				if ( angleToTarget > halfSightAngle )
				{
					continue;
				}
			}
		}

		float const distToTarget = sqrtf( distSq );
		Vec3 const dirToTarget = toTarget.GetNormalized();

		RaycastResult3D const wallResult = RaycastWorldXY( seekerEyePos, dirToTarget, distToTarget );
		if ( wallResult.m_didImpact && wallResult.m_impactDist < ( distToTarget - 0.001f ) )
		{
			continue;
		}

		RaycastResult3D const floorCeilingResult = RaycastWorldZ( seekerEyePos, dirToTarget, distToTarget );
		if ( floorCeilingResult.m_didImpact && floorCeilingResult.m_impactDist < ( distToTarget - 0.001f ) )
		{
			continue;
		}

		closestDistSq = distSq;
		closestEnemy = other;
	}

	return closestEnemy;
}


//-----------------------------------------------------------------------------------------------
Actor* Map::GetClosestActorInSector( Vec3 const& startPos, Vec3 const& forwardNormal, float maxLength, float arcDegrees, Actor* owner ) const
{
	if ( maxLength <= 0.f )
	{
		return nullptr;
	}

	Vec2 forwardXY( forwardNormal.x, forwardNormal.y );
	if ( forwardXY == Vec2::ZERO )
	{
		return nullptr;
	}
	forwardXY.Normalize();

	Vec2 startPosXY( startPos.x, startPos.y );
	float halfArcDegrees = arcDegrees * 0.5f;
	float closestDistSq = maxLength * maxLength;
	Actor* closestActor = nullptr;

	for ( int index = 0; index < ( int ) m_actors.size(); index++ )
	{
		Actor* actor = m_actors[index];
		if ( actor == nullptr
			|| actor == owner
			|| actor->m_isDead
			|| actor->m_isDestroyed
			|| actor->m_definition == nullptr
			|| owner->m_definition->m_faction == "NEUTRAL"
			|| actor->m_definition->m_faction == owner->m_definition->m_faction )
		{
			continue;
		}

		Vec2 targetPosXY( actor->m_position.x, actor->m_position.y );
		Vec2 toTargetXY = targetPosXY - startPosXY;
		float distSq = toTargetXY.GetLengthSquared();
		if ( distSq > closestDistSq )
		{
			continue;
		}

		if ( arcDegrees > 0.f && toTargetXY != Vec2::ZERO )
		{
			toTargetXY.Normalize();
			float angleToTarget = GetAngleDegreesBetweenVectors2D( forwardXY, toTargetXY );
			if ( angleToTarget > halfArcDegrees )
			{
				continue;
			}
		}

		closestDistSq = distSq;
		closestActor = actor;
	}

	return closestActor;
}


//-----------------------------------------------------------------------------------------------
void Map::DebugPossessNext()
{
	if ( m_game == nullptr )
	{
		return;
	}

	if ( ( int ) m_game->m_players.size() != 1 )
	{
		return;
	}

	Player* player = m_game->m_players[0];
	if ( player == nullptr )
	{
		return;
	}

	player->m_map = this;

	ActorHandle const previousHandle = player->m_possessedActor;
	Actor* previousActor = player->GetActor();

	ActorHandle currentHandle = ActorHandle::INVALID;
	if ( previousActor != nullptr )
	{
		currentHandle = previousActor->m_handle;
	}

	Actor* nextPossessableActor = GetNextPossessableActor( currentHandle );
	if ( nextPossessableActor == nullptr )
	{
		return;
	}

	player->Possess( nextPossessableActor->m_handle );

	Actor* newlyPossessedActor = player->GetActor();
	if ( newlyPossessedActor != nextPossessableActor )
	{
		player->Possess( previousHandle );
		return;
	}

	player->m_orientation = newlyPossessedActor->m_orientation;
	player->m_orientation.m_rollDegrees = 0.f;
}


//-----------------------------------------------------------------------------------------------
Actor* Map::GetNextPossessableActor( ActorHandle const& currentHandle ) const
{
	int const actorCount = ( int ) m_actors.size();
	if ( actorCount <= 0 )
	{
		return nullptr;
	}

	int currentIndex = -1;
	if ( currentHandle.IsValid() )
	{
		unsigned int const index = currentHandle.GetIndex();
		if ( index < m_actors.size() )
		{
			Actor* actorAtIndex = m_actors[index];
			if ( actorAtIndex != nullptr && actorAtIndex->m_handle == currentHandle )
			{
				currentIndex = ( int ) index;
			}
		}
	}

	for ( int step = 1; step <= actorCount; ++step )
	{
		int const actorIndex = ( currentIndex + step + actorCount ) % actorCount;
		Actor* actor = m_actors[actorIndex];
		if ( actor == nullptr || actor->m_definition == nullptr )
		{
			continue;
		}

		if ( actor->m_isDead || actor->m_isDestroyed )
		{
			continue;
		}

		if ( !actor->m_definition->m_canBePossessed )
		{
			continue;
		}

		return actor;
	}

	return nullptr;
}


//-----------------------------------------------------------------------------------------------
Actor* Map::GetActorByHandle( ActorHandle const actorHandle ) const
{
	if ( !actorHandle.IsValid() )
	{
		return nullptr;
	}

	unsigned int const index = actorHandle.GetIndex();
	if ( index >= m_actors.size() )
	{
		return nullptr;
	}

	Actor* actor = m_actors[index];
	if ( actor == nullptr )
	{
		return nullptr;
	}

	if ( actor->m_handle != actorHandle )
	{
		return nullptr;
	}

	return actor;
}


//-----------------------------------------------------------------------------------------------
void Map::DeleteDestroyedActors()
{
	for ( int index = 0; index < ( int ) m_actors.size(); index++ )
	{
		Actor* actor = m_actors[index];
		if ( actor != nullptr && actor->m_isDestroyed )
		{
			delete actor;
			m_actors[index] = nullptr;
		}
	}
}