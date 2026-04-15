#include "Game/Actor.hpp"
#include "Game/Map.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
Actor::Actor()
	: m_position( Vec3::ZERO )
	, m_orientation( EulerAngles::ZERO )
	, m_color( Rgba8::RED )
{
}


//-----------------------------------------------------------------------------------------------
Actor::Actor( ActorHandle handle, ActorDefinition const* definition, Map* map )
	: m_handle( handle )
	, m_definition( definition )
	, m_map( map )
	, m_position( Vec3::ZERO )
	, m_orientation( EulerAngles::ZERO )
	, m_color( Rgba8::RED )
{
	if ( m_definition != nullptr )
	{
		m_maxHealth = m_definition->m_health;
		m_currentHealth = m_maxHealth;

		// Initialize weapon inventory
		m_inventory.reserve( m_definition->m_weaponDefNames.size() );
		for ( std::string const& weaponDefName : m_definition->m_weaponDefNames )
		{
			WeaponDefinition const* weaponDef = WeaponDefinition::GetWeaponDefinitionByName( weaponDefName );
			Weapon newWeapon( weaponDef );
			m_inventory.push_back( newWeapon );
		}
		m_currentWeapon = m_inventory.empty() ? nullptr : &m_inventory[0];

		// Create geometry if visible
		if ( m_definition->m_isVisible )
		{
			if ( m_definition->m_physicsRadius > 0.f && m_definition->m_physicsHeight > 0.f )
			{
				AddVertsForCylinderZ3D( m_verts, Vec3::ZERO, Vec3( 0.f, 0.f, m_definition->m_physicsHeight ), m_definition->m_physicsRadius, m_color );
				AddVertsForCylinderZWireframe3D( m_verts, Vec3::ZERO, Vec3( 0.f, 0.f, m_definition->m_physicsHeight ), m_definition->m_physicsRadius, Rgba8::WHITE );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
Actor::~Actor()
{
}


//-----------------------------------------------------------------------------------------------
void Actor::Update()
{
	if ( m_definition == nullptr )
	{
		return;
	}

	UpdatePhysics();
}


//-----------------------------------------------------------------------------------------------
void Actor::UpdatePhysics()
{
	if ( m_definition == nullptr || !m_definition->m_isSimulated )
	{
		return;
	}

	float deltaSeconds = static_cast<float>( Clock::GetSystemClock().GetDeltaSeconds() );

	if ( !m_definition->m_isFlying )
	{
		m_velocity.z = 0.f;
		m_acceleration.z = 0.f;
	}

	Vec3 dragForce = m_definition->m_drag * -m_velocity;
	AddForce( dragForce );

	m_velocity += m_acceleration * deltaSeconds;
	m_position += m_velocity * deltaSeconds;

	if ( !m_definition->m_isFlying )
	{
		m_velocity.z = 0.f;
	}

	m_acceleration = Vec3::ZERO;
}


//-----------------------------------------------------------------------------------------------
void Actor::TakeDamage( int damageAmount )
{
	if ( m_definition == nullptr || damageAmount <= 0 )
	{
		return;
	}

	m_currentHealth -= damageAmount;

	if ( m_currentHealth < 0 )
	{
		m_currentHealth = 0;
	}
}


//-----------------------------------------------------------------------------------------------
void Actor::AddForce( Vec3 const& force )
{
	if ( m_definition == nullptr || !m_definition->m_isSimulated )
	{
		return;
	}

	m_acceleration += force;
}


//-----------------------------------------------------------------------------------------------
void Actor::AddImpulse( Vec3 const& impulse )
{
	if ( m_definition == nullptr || !m_definition->m_isSimulated )
	{
		return;
	}

	m_velocity += impulse;
}


//-----------------------------------------------------------------------------------------------
void Actor::TurnInDirection( Vec3 const& turnDirection )
{
	if ( m_definition == nullptr || turnDirection == Vec3::ZERO )
	{
		return;
	}

	float targetYaw = turnDirection.GetOrientationAboutZDegrees();
	float yawDifference = GetShortestAngularDispDegrees( m_orientation.m_yawDegrees, targetYaw );
	float maxYawChange = m_definition->m_turnSpeed * static_cast<float>( Clock::GetSystemClock().GetDeltaSeconds() );
	float clampedYawChange = GetClamped( yawDifference, -maxYawChange, maxYawChange );

	m_orientation.m_yawDegrees += clampedYawChange;
}


//-----------------------------------------------------------------------------------------------
void Actor::Attack()
{
	if ( m_definition == nullptr || m_currentWeapon == nullptr )
	{
		return;
	}

	m_currentWeapon->Fire();
}


//-----------------------------------------------------------------------------------------------
void Actor::EquipWeapon( Weapon* weapon )
{
	if ( m_definition == nullptr || weapon == nullptr )
	{
		return;
	}

	for ( int i = 0; i < static_cast<int>( m_inventory.size() ); i++ )
	{
		if ( &m_inventory[i] == weapon )
		{
			m_currentWeapon = &m_inventory[i];
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Actor::Render() const
{
	if ( g_engine == nullptr || g_engine->m_renderer == nullptr )
	{
		return;
	}

	if ( m_definition->m_physicsRadius <= 0.f || m_definition->m_physicsHeight <= 0.f )
	{
		return;
	}

	Mat44 modelMatrix = GetModelMatrix();

	g_engine->m_renderer->BindTexture( nullptr );

	g_engine->m_renderer->SetModelConstants( modelMatrix, Rgba8::WHITE );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
	g_engine->m_renderer->DrawVertexArray( m_verts );

	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
}


//-----------------------------------------------------------------------------------------------
Mat44 Actor::GetModelMatrix() const
{
	Mat44 modelToWorld;
	Mat44 translation = Mat44::MakeTranslation3D( m_position );
	Mat44 rotation = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();

	modelToWorld.Append( translation );
	modelToWorld.Append( rotation );

	return modelToWorld;
}