#pragma once
#include "Game/ActorDefinition.hpp"
#include "Game/ActorHandle.hpp"
#include "Game/Weapon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class AI;
class Controller;
class Map;


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

	void Render() const;
	void SetSolidColor( Rgba8 const& color );
	Mat44 GetModelMatrix() const;

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
};