#include "Game/Weapon.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
Weapon::Weapon( WeaponDefinition const* definition )
	: m_definition( definition )
	, m_refireTimer( 0.0 )
{
	if ( m_definition != nullptr )
	{
		m_refireTimer.m_period = m_definition->m_refireTime;
	}
}


//-----------------------------------------------------------------------------------------------
Weapon::~Weapon()
{
}


//-----------------------------------------------------------------------------------------------
Vec3 Weapon::GetRandomDirectionInCone( Vec3 const& baseForward, float coneDegrees ) const
{
	RandomNumberGenerator rng;

	if ( coneDegrees <= 0.f )
	{
		return baseForward;
	}

	float halfCone = coneDegrees * 0.5f;

	float baseYaw = Atan2Degrees( baseForward.y, baseForward.x );
	float basePitch = Atan2Degrees( baseForward.z, Vec2( baseForward.x, baseForward.y ).GetLength() );

	float yaw = baseYaw + rng.RollRandomFloatInRange( -halfCone, halfCone );
	float pitch = basePitch + rng.RollRandomFloatInRange( -halfCone, halfCone );

	float cosPitch = CosDegrees( pitch );
	Vec3 dir = Vec3(
		cosPitch * CosDegrees( yaw ),
		cosPitch * SinDegrees( yaw ),
		SinDegrees( pitch ) );

	if ( dir == Vec3::ZERO )
	{
		return baseForward;
	}

	return dir.GetNormalized();
}


//-----------------------------------------------------------------------------------------------
bool Weapon::CanFire( Actor const* owner ) const
{
	if ( m_definition == nullptr || owner == nullptr || owner->m_definition == nullptr || owner->m_map == nullptr )
	{
		return false;
	}

	if ( m_refireTimer.m_period > 0.0 && !m_refireTimer.IsStopped() && !m_refireTimer.HasPeriodElapsed() )
	{
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
void Weapon::Fire( Actor* owner )
{
	if ( m_definition == nullptr || owner == nullptr || owner->m_definition == nullptr || owner->m_map == nullptr )
	{
		return;
	}

	if ( m_refireTimer.m_period > 0.0 && !m_refireTimer.IsStopped() && !m_refireTimer.HasPeriodElapsed() )
	{
		return;
	}

	RandomNumberGenerator damageRng;

	Vec3 fireStartPos;
	Vec3 baseForward;

	Player* owningPlayer = g_app->m_game->GetPlayerFromActor( owner );
	bool isPlayerOwner = owningPlayer != nullptr;

	if ( isPlayerOwner )
	{
		Camera* camera = owningPlayer->m_playerWorldCamera;

		fireStartPos = camera->GetPosition();
		baseForward = camera->GetForwardDir();
	}
	else
	{
		fireStartPos = owner->m_position;
		fireStartPos.z += owner->m_definition->m_eyeHeight;

		baseForward = owner->m_orientation.GetForwardDir_IFwd_JLeft_KUp();
	}

	if ( baseForward.GetLengthSquared() <= 0.f )
	{
		baseForward = Vec3( 1.f, 0.f, 0.f );
	}

	baseForward = baseForward.GetNormalized();

	float spawnOffset = owner->m_definition->m_physicsRadius;
	Vec3 attackSpawnCenter = fireStartPos + baseForward * spawnOffset;

	float aimDistance = m_definition->m_rayRange;
	if ( aimDistance <= 0.f )
	{
		aimDistance = 1000.f;
	}

	Vec3 aimTargetPos = fireStartPos + baseForward * aimDistance;

	if ( isPlayerOwner )
	{
		Actor* ignoredHitActor = nullptr;
		RaycastResult3D aimResult = owner->m_map->RaycastAll(
			fireStartPos,
			baseForward,
			aimDistance,
			owner,
			&ignoredHitActor
		);

		if ( aimResult.m_didImpact )
		{
			aimTargetPos = aimResult.m_impactPos;
		}
	}

	Vec3 correctedForward = aimTargetPos - attackSpawnCenter;
	if ( correctedForward.GetLengthSquared() <= 0.f )
	{
		correctedForward = baseForward;
	}
	else
	{
		correctedForward = correctedForward.GetNormalized();
	}

	// Rays
	for ( int rayIndex = 0; rayIndex < m_definition->m_rayCount; ++rayIndex )
	{
		Vec3 rayDir = GetRandomDirectionInCone( correctedForward, m_definition->m_rayCone );

		Actor* rayHitActor = nullptr;
		RaycastResult3D rayResult = owner->m_map->RaycastAll(
			attackSpawnCenter,
			rayDir,
			m_definition->m_rayRange,
			owner,
			&rayHitActor
		);

		if ( rayResult.m_didImpact )
		{
			Vec3 debugOffset( 0.f, 0.f, -0.08f );
			Vec3 debugStart = attackSpawnCenter + debugOffset;
			Vec3 debugEnd = rayResult.m_impactPos + debugOffset;

			SpawnInfo hitSpawnInfo;

			if ( rayHitActor != nullptr )
			{
				hitSpawnInfo.m_actor = "BloodSplatter";
			}
			else
			{
				hitSpawnInfo.m_actor = "BulletHit";
			}

			hitSpawnInfo.m_position = rayResult.m_impactPos;

			float yaw = Atan2Degrees( rayResult.m_impactNormal.y, rayResult.m_impactNormal.x );
			float pitch = Atan2Degrees(
				rayResult.m_impactNormal.z,
				Vec2( rayResult.m_impactNormal.x, rayResult.m_impactNormal.y ).GetLength()
			);

			hitSpawnInfo.m_orientation = Vec3( yaw, pitch, 0.f );

			owner->m_map->SpawnActor( hitSpawnInfo );
		}

		if ( rayHitActor != nullptr )
		{
			int minDamage = ( int ) m_definition->m_rayDamage.m_min;
			int maxDamage = ( int ) m_definition->m_rayDamage.m_max;

			if ( minDamage > maxDamage )
			{
				int temp = minDamage;
				minDamage = maxDamage;
				maxDamage = temp;
			}

			if ( maxDamage > 0 )
			{
				int damageAmount = damageRng.RollRandomIntInRange( minDamage, maxDamage );
				if ( damageAmount > 0 )
				{
					rayHitActor->TakeDamage( damageAmount, owner );
				}
			}

			if ( m_definition->m_rayImpulse > 0.f )
			{
				rayHitActor->AddImpulse( rayDir * m_definition->m_rayImpulse );
			}
		}
	}

	// Projectiles
	for ( int projectileIndex = 0; projectileIndex < m_definition->m_projectileCount; ++projectileIndex )
	{
		if ( m_definition->m_projectileActorDefName.empty() )
		{
			break;
		}

		Vec3 projectileForward = aimTargetPos - attackSpawnCenter;
		if ( projectileForward.GetLengthSquared() <= 0.f )
		{
			projectileForward = baseForward;
		}
		else
		{
			projectileForward = projectileForward.GetNormalized();
		}

		Vec3 projectileDir = GetRandomDirectionInCone( projectileForward, m_definition->m_projectileCone );

		SpawnInfo spawnInfo;
		spawnInfo.m_actor = m_definition->m_projectileActorDefName;

		ActorDefinition const* projectileDef = ActorDefinition::GetActorDefinitionByName( m_definition->m_projectileActorDefName );

		float projectileHalfHeight = 0.f;
		if ( projectileDef != nullptr )
		{
			projectileHalfHeight = projectileDef->m_physicsHeight * 0.5f;
		}

		spawnInfo.m_position = attackSpawnCenter;
		spawnInfo.m_position.z -= projectileHalfHeight;

		float yaw = Atan2Degrees( projectileDir.y, projectileDir.x );
		float pitch = Atan2Degrees( projectileDir.z, Vec2( projectileDir.x, projectileDir.y ).GetLength() );

		spawnInfo.m_orientation = Vec3( yaw, pitch, 0.f );
		spawnInfo.m_velocity = projectileDir * m_definition->m_projectileSpeed;

		Actor* spawnedProjectile = owner->m_map->SpawnActor( spawnInfo );
		if ( spawnedProjectile != nullptr )
		{
			spawnedProjectile->m_ownerHandle = owner->m_handle;
		}
	}

	// Melee
	for ( int meleeIndex = 0; meleeIndex < m_definition->m_meleeCount; ++meleeIndex )
	{
		Actor* meleeTarget = owner->m_map->GetClosestActorInSector(
			owner->m_position,
			baseForward,
			m_definition->m_meleeRange,
			m_definition->m_meleeArc,
			owner
		);

		if ( meleeTarget == nullptr )
		{
			continue;
		}

		int minDamage = ( int ) m_definition->m_meleeDamage.m_min;
		int maxDamage = ( int ) m_definition->m_meleeDamage.m_max;

		if ( minDamage > maxDamage )
		{
			int temp = minDamage;
			minDamage = maxDamage;
			maxDamage = temp;
		}

		if ( maxDamage > 0 )
		{
			int damageAmount = damageRng.RollRandomIntInRange( minDamage, maxDamage );
			if ( damageAmount > 0 )
			{
				meleeTarget->TakeDamage( damageAmount, owner );
			}
		}

		if ( m_definition->m_meleeImpulse > 0.f )
		{
			Vec3 impulseDir = meleeTarget->m_position - owner->m_position;
			if ( impulseDir != Vec3::ZERO )
			{
				impulseDir = impulseDir.GetNormalized();
				meleeTarget->AddImpulse( impulseDir * m_definition->m_meleeImpulse );
			}
		}
	}

	if ( m_refireTimer.m_period > 0.0 )
	{
		m_attackStartTime = GetCurrentTimeSeconds();
		m_refireTimer.Start();
	}
}


//-----------------------------------------------------------------------------------------------
void Weapon::Render( Actor const* owner ) const
{
	if ( owner == nullptr || m_definition == nullptr )
	{
		return;
	}

	if ( g_engine == nullptr || g_engine->m_renderer == nullptr )
	{
		return;
	}

	if ( g_app == nullptr || g_app->m_game == nullptr )
	{
		return;
	}

	Player* renderingPlayer = g_app->m_game->m_currentRenderingPlayer;
	if ( renderingPlayer == nullptr )
	{
		return;
	}

	if ( renderingPlayer->GetActor() != owner )
	{
		return;
	}

	if ( renderingPlayer->m_playerScreenCamera == nullptr )
	{
		return;
	}

	if ( m_definition->m_hudTexture.empty() )
	{
		return;
	}

	Renderer* renderer = g_engine->m_renderer;

	Texture* hudBaseTexture = renderer->CreateOrGetTextureFromFile( m_definition->m_hudTexture.c_str() );
	if ( hudBaseTexture == nullptr )
	{
		return;
	}

	Camera screenCamera = *renderingPlayer->m_playerScreenCamera;

	renderer->BeginCamera( screenCamera );
	renderer->SetBlendMode( BlendMode::ALPHA );
	renderer->SetDepthMode( DepthMode::DISABLED );
	renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_NONE );

	// Animation
	if ( !m_definition->m_animationNames.empty() )
	{
		PlayAnimation();
	}

	// HUD base
	AABB2 viewport = renderingPlayer->m_playerScreenCamera->GetViewport();
	float screenWidth = SCREEN_SIZE_X;
	float screenHeight = SCREEN_SIZE_Y * viewport.GetDimensions().y;

	IntVec2 hudDimensions = hudBaseTexture->GetDimensions();
	bool multiplayer = g_app->m_game->m_players.size() > 1;
	float hudHeightScale;
	if ( multiplayer ) hudHeightScale = 0.5f;
	else hudHeightScale = 1.f;

	float hudScaleX = screenWidth / ( float ) hudDimensions.x;
	float hudScaleY = hudScaleX * hudHeightScale;

	AABB2 hudBounds(
		Vec2( 0.f, 0.f ),
		Vec2( screenWidth, ( float ) hudDimensions.y * hudScaleY )
	);

	std::vector<Vertex> hudVerts;
	AddVertsForAABB2D( hudVerts, hudBounds, Rgba8::WHITE );

	renderer->BindTexture( hudBaseTexture );
	renderer->SetModelConstants();
	renderer->DrawVertexArray( hudVerts );
	renderer->BindTexture( nullptr );

	// Reticle
	if ( !m_definition->m_rectileTexture.empty() )
	{
		Texture* rectileTexture = renderer->CreateOrGetTextureFromFile( m_definition->m_rectileTexture.c_str() );

		if ( rectileTexture != nullptr )
		{
			Vec2 rectileSize = m_definition->m_rectileSize;

			if ( rectileSize == Vec2::ZERO )
			{
				IntVec2 dims = rectileTexture->GetDimensions();
				rectileSize = Vec2( ( float ) dims.x, ( float ) dims.y );
			}

			Vec2 reticleSize = m_definition->m_rectileSize;
			Vec2 center( screenWidth * 0.5f, screenHeight * 0.5f );
			AABB2 rectileBounds( center - rectileSize * 0.5f, center + rectileSize * 0.5f );

			std::vector<Vertex> rectileVerts;
			AddVertsForAABB2D( rectileVerts, rectileBounds, Rgba8::WHITE );

			renderer->BindTexture( rectileTexture );
			renderer->SetModelConstants();
			renderer->DrawVertexArray( rectileVerts );
			renderer->BindTexture( nullptr );
		}
	}

	// Player health
	int health = owner->m_currentHealth;
	std::string healthText = Stringf( "%d", health );

	BitmapFont* hudFont = renderer->CreateOrGetBitmapFontFromFile( "Data/Fonts/SquirrelFixedFont" );
	if ( hudFont == nullptr ) return;

	float healthMinX = hudBounds.m_mins.x + hudBounds.GetDimensions().x * 0.25f + 15.f;
	float healthMaxX = hudBounds.m_mins.x + hudBounds.GetDimensions().x * 0.35f;

	AABB2 healthBox(
		Vec2( healthMinX, hudBounds.m_mins.y + 6.f ),
		Vec2( healthMaxX, hudBounds.m_maxs.y )
	);

	std::vector<Vertex> healthVerts;
	hudFont->AddVertsForTextInBox2D(
		healthVerts,
		healthText,
		healthBox,
		45.f,
		Rgba8::WHITE,
		1.f,
		Vec2( 0.5f, 0.5f ) );

	renderer->BindTexture( &hudFont->GetTexture() );
	renderer->SetModelConstants();
	renderer->DrawVertexArray( healthVerts );

	// Player kill count
	Player* player = g_app->m_game->GetPlayerFromActor( owner );
	int killCount = player->m_killCount;
	std::string killCountText = Stringf( "%d", killCount );

	float killCountMinX = hudBounds.m_mins.x;
	float killCountMaxX = hudBounds.m_mins.x + hudBounds.GetDimensions().x * 0.12f + 15.f;

	AABB2 killCountBox(
		Vec2( killCountMinX, hudBounds.m_mins.y + 6.f),
		Vec2( killCountMaxX, hudBounds.m_maxs.y )
	);

	std::vector<Vertex> killCountVerts;
	hudFont->AddVertsForTextInBox2D(
		killCountVerts,
		killCountText,
		killCountBox,
		45.f,
		Rgba8::WHITE,
		1.f,
		Vec2( 0.5f, 0.5f ) );

	renderer->BindTexture( &hudFont->GetTexture() );
	renderer->SetModelConstants();
	renderer->DrawVertexArray( killCountVerts );
	renderer->BindTexture( nullptr );


	renderer->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
	renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
	renderer->EndCamera( screenCamera );
}


//-----------------------------------------------------------------------------------------------
void Weapon::PlayAnimation() const
{
	if ( m_definition == nullptr || g_engine == nullptr || g_engine->m_renderer == nullptr )
	{
		return;
	}

	if ( m_definition->m_animationNames.empty() )
	{
		return;
	}

	Renderer* renderer = g_engine->m_renderer;

	int idleIndex = -1;
	int attackIndex = -1;

	for ( int i = 0; i < ( int ) m_definition->m_animationNames.size(); ++i )
	{
		if ( m_definition->m_animationNames[i] == "Idle" )
		{
			idleIndex = i;
		}
		else if ( m_definition->m_animationNames[i] == "Attack" )
		{
			attackIndex = i;
		}
	}

	bool isAttackPlaying = false;

	if ( attackIndex >= 0 )
	{
		int startFrame = m_definition->m_animationStartFrames[attackIndex];
		int endFrame = m_definition->m_animationEndFrames[attackIndex];

		if ( endFrame < startFrame )
		{
			int temp = startFrame;
			startFrame = endFrame;
			endFrame = temp;
		}

		int frameCount = endFrame - startFrame + 1;
		float secondsPerFrame = m_definition->m_animationGroupSecondsPerFrame[attackIndex];

		if ( frameCount > 0 && secondsPerFrame > 0.f )
		{
			float duration = ( float ) frameCount * secondsPerFrame;
			double elapsed = GetCurrentTimeSeconds() - m_attackStartTime;

			isAttackPlaying = elapsed < duration;
		}
	}

	int animIndex = idleIndex;
	if ( isAttackPlaying && attackIndex >= 0 )
	{
		animIndex = attackIndex;
	}

	if ( animIndex < 0 )
	{
		return;
	}

	std::string const& spriteSheet = m_definition->m_animationSpriteSheets[animIndex];
	if ( spriteSheet.empty() )
	{
		return;
	}

	Texture* texture = renderer->CreateOrGetTextureFromFile( spriteSheet.c_str() );
	if ( texture == nullptr )
	{
		return;
	}

	Vec2 cellCount = m_definition->m_animationCellCounts[animIndex];
	int cellsWide = ( int ) cellCount.x;
	int cellsHigh = ( int ) cellCount.y;

	if ( cellsWide <= 0 || cellsHigh <= 0 )
	{
		return;
	}

	int startFrame = m_definition->m_animationStartFrames[animIndex];
	int endFrame = m_definition->m_animationEndFrames[animIndex];

	if ( endFrame < startFrame )
	{
		int temp = startFrame;
		startFrame = endFrame;
		endFrame = temp;
	}

	int frameCount = endFrame - startFrame + 1;
	float secondsPerFrame = m_definition->m_animationGroupSecondsPerFrame[animIndex];

	int localFrame = 0;

	if ( secondsPerFrame > 0.f && frameCount > 1 )
	{
		double time = GetCurrentTimeSeconds();

		if ( isAttackPlaying )
		{
			time -= m_attackStartTime;

			localFrame = ( int ) ( time / secondsPerFrame );

			if ( localFrame >= frameCount )
			{
				localFrame = frameCount - 1;
			}
		}
		else
		{
			localFrame = ( int ) ( time / secondsPerFrame ) % frameCount;
		}
	}

	int frameIndex = startFrame + localFrame;

	int cellX = frameIndex % cellsWide;
	int cellY = frameIndex / cellsWide;

	Vec2 uvMins;
	Vec2 uvMaxs;

	uvMins.x = ( float ) cellX / ( float ) cellsWide;
	uvMaxs.x = ( float ) ( cellX + 1 ) / ( float ) cellsWide;

	uvMins.y = 1.f - ( float ) ( cellY + 1 ) / ( float ) cellsHigh;
	uvMaxs.y = 1.f - ( float ) cellY / ( float ) cellsHigh;


	Vec2 spriteSize = m_definition->m_spriteSize;

	if ( spriteSize == Vec2::ZERO )
	{
		IntVec2 texDim = texture->GetDimensions();
		spriteSize = Vec2(
			( float ) texDim.x / ( float ) cellsWide,
			( float ) texDim.y / ( float ) cellsHigh );
	}

	bool multiplayer = g_app->m_game->m_players.size() > 1;
	float spriteScale;
	if ( multiplayer ) spriteScale = 0.5f;
	else spriteScale = 1.f;

	spriteSize *= spriteScale;

	Vec2 pivot = m_definition->m_spritePivot;

	Player* renderingPlayer = g_app->m_game->m_currentRenderingPlayer;
	AABB2 viewport = renderingPlayer->m_playerScreenCamera->GetViewport();

	float screenWidth = SCREEN_SIZE_X;

	Texture* hudBaseTexture = g_engine->m_renderer->CreateOrGetTextureFromFile( m_definition->m_hudTexture.c_str() );
	IntVec2 hudDimensions = hudBaseTexture->GetDimensions();
	float hudHeightScale;
	if ( multiplayer ) hudHeightScale = 0.5f;
	else hudHeightScale = 1.f;

	float hudScaleX = screenWidth / ( float ) hudDimensions.x;
	float hudScaleY = hudScaleX * hudHeightScale;
	float hudHeight = ( float ) hudDimensions.y * hudScaleY;

	Vec2 anchor( screenWidth * 0.5f, hudHeight );
	Vec2 mins = anchor - Vec2( spriteSize.x * pivot.x, spriteSize.y * pivot.y );
	Vec2 maxs = mins + spriteSize;

	AABB2 bounds( mins, maxs );

	std::vector<Vertex> verts;
	AddVertsForAABB2D( verts, bounds, Rgba8::WHITE, uvMins, uvMaxs );

	renderer->BindTexture( texture );
	renderer->SetModelConstants();
	renderer->DrawVertexArray( verts );
	renderer->BindTexture( nullptr );
}