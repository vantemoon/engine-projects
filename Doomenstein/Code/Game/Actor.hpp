#pragma once
#include "Game/ActorDefinition.hpp"
#include "Game/ActorHandle.hpp"
#include "Game/Weapon.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec3.hpp"
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
class AI;
class Controller;
class Map;
class Shader;
class Texture;


//-----------------------------------------------------------------------------------------------
class Actor
{
public:
	Actor();
	Actor( ActorHandle handle, ActorDefinition const* definition, Map* map );
	~Actor();

	void Update();
	void UpdatePhysics();
	void TakeDamage( int damageAmount, Actor* attacker = nullptr );
	void AddForce( Vec3 const& force );
	void AddImpulse( Vec3 const& impulse );
	void OnCollide( Actor* otherActor = nullptr, Vec3 const& collisionNormal = Vec3::ZERO );
	void OnPossessed( Controller* newController );
	void OnUnpossessed();
	void MoveInDirection( Vec3 const& moveDirection, float speed );
	void TurnInDirection( Vec3 const& turnDirection, float maxTurnDegrees );
	void Attack();
	void EquipWeapon( Weapon* weapon );

	void PlayAnimationByName( std::string const& animationGroupName );
	void PlaySoundByType( std::string const& soundType );
	void UpdateSoundPositions();

	void Render() const;
	void SetSolidColor( Rgba8 const& color );
	Mat44 GetModelMatrix() const;

	void UpdateVirtualPet( float deltaSeconds );
	void UpdatePetMessSpawning( float deltaSeconds );
	void UpdatePickupRespawn( float deltaSeconds );
	void UpdatePetMisbehavior( float deltaSeconds );
	void AddHunger( float amount );
	void AddCleanliness( float amount );
	void AddHappiness( float amount );
	void CollectPickup();
	void Discipline();
	bool ShouldMisbehave() const;

	bool IsActorNamed( std::string const& name ) const;
	void DestroyImmediately();

private:
	int GetAnimationGroupIndexByName( std::string const& animationGroupName ) const;
	int GetBestDirectionAnimationIndex() const;
	int GetCurrentAnimationFrameIndex() const;
	bool IsCurrentAnimationGroupComplete() const;
	AABB2 GetUVsForFrameIndex( int frameIndex ) const;

	void AppendBillboardVerts( std::vector<Vertex>& outVerts, AABB2 const& uvs ) const;
	void AppendQuad( std::vector<Vertex>& outVerts, Vec3 const& bl, Vec3 const& br, Vec3 const& tr, Vec3 const& tl, AABB2 const& uvs, Vec3 const& normal ) const;
	void AppendQuadWithVertexNormals( std::vector<Vertex>& outVerts, Vec3 const& bl, Vec3 const& br, Vec3 const& tr, Vec3 const& tl, AABB2 const& uvs, Vec3 const& nbl, Vec3 const& nbr, Vec3 const& ntr, Vec3 const& ntl ) const;

public:
	ActorHandle m_handle;
	ActorDefinition const* m_definition = nullptr;
	Map* m_map = nullptr;

	bool m_isProjectile = false;
	ActorHandle m_ownerHandle = ActorHandle::INVALID;

	bool m_isDead = false;
	Timer m_deadTimer = Timer( 0.0 );
	bool m_isDestroyed = false;

	Vec3 m_position;
	EulerAngles m_orientation;
	Vec3 m_velocity;
	Vec3 m_acceleration;

	std::vector<Vertex> m_verts;
	int m_solidVertCount = 0;
	Rgba8 m_color;

	int m_maxHealth = 1;
	int m_currentHealth = 1;

	Controller* m_controller = nullptr;
	AI* m_aiController = nullptr;

	std::vector<Weapon> m_inventory;
	Weapon* m_currentWeapon = nullptr;

	Clock m_animationClock;
	int m_currentAnimationGroupIndex = -1;
	std::string m_currentAnimationGroupName;

	Texture* m_visualTexture = nullptr;
	Shader* m_visualShader = nullptr;

	std::vector<std::string> m_soundTypes;
	std::vector<SoundID> m_soundIDs;
	std::vector<SoundPlaybackID> m_currentPlayingSounds;

	bool m_isVirtualPet = false;

	float m_hunger = 100.f;
	float m_cleanliness = 100.f;
	float m_happiness = 100.f;

	float m_hungerDecayRate = 2.f;
	float m_cleanlinessDecayRate = 1.f;
	float m_happinessDecayRate = 0.5f;

	float m_messSpawnTimer = 0.f;
	float m_messSpawnInterval = 8.f;
	float m_messCleanlinessThreshold = 60.f;
	int m_maxMessCount = 3;

	bool m_isPickup = false;
	bool m_isPickupActive = true;
	float m_pickupRespawnTimer = 0.f;
	float m_pickupRespawnSeconds = 10.f;

	bool m_isMisbehaving = false;

	float m_misbehaviourTimer = 0.f;
	float m_misbehaviourCheckInterval = 5.f;

	float m_disciplineCooldownTimer = 0.f;
	float m_disciplineCooldownSeconds = 8.f;
};