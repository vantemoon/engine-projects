#include "Game/Actor.hpp"
#include "Game/Controller.hpp"
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
	, m_deadTimer( 0.0 )
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
	, m_deadTimer( 0.0 )
{
	if ( m_definition != nullptr )
	{
		m_isProjectile = m_definition->m_isProjectile;
		m_deadTimer.m_period = m_definition->corpseLifetime;

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
				Rgba8 lighterColor = Rgba8(
					static_cast<unsigned char>( GetClamped( static_cast<float>( m_color.r ) + 150.f, 0.f, 255.f ) ),
					static_cast<unsigned char>( GetClamped( static_cast<float>( m_color.g ) + 150.f, 0.f, 255.f ) ),
					static_cast<unsigned char>( GetClamped( static_cast<float>( m_color.b ) + 150.f, 0.f, 255.f ) ),
					m_color.a );
				AddVertsForCylinderZ3D( m_verts, Vec3::ZERO, Vec3( 0.f, 0.f, m_definition->m_physicsHeight ), m_definition->m_physicsRadius, m_color );

				float eyeHeight = m_definition->m_eyeHeight - 0.05f;
				float eyeConeLength = m_definition->m_physicsRadius * 0.3f;
				float eyeConeRadius = m_definition->m_physicsRadius * 0.3f;

				float eyeOffsetFromCenter = m_definition->m_physicsRadius;
				Vec3 eyeBase( eyeOffsetFromCenter, 0.f, eyeHeight );
				Vec3 eyeTip( eyeOffsetFromCenter + eyeConeLength, 0.f, eyeHeight );

				if ( !m_isProjectile )
				{
					AddVertsForCone3D( m_verts, eyeBase, eyeTip, eyeConeRadius, m_color );
				}

				m_solidVertCount = ( int ) m_verts.size();
				AddVertsForCylinderZWireframe3D( m_verts, Vec3::ZERO, Vec3( 0.f, 0.f, m_definition->m_physicsHeight ), m_definition->m_physicsRadius, lighterColor );

				if ( !m_isProjectile )
				{
					AddVertsForConeWireframe3D( m_verts, eyeBase, eyeTip, eyeConeRadius, lighterColor );
				}
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
	if ( m_definition == nullptr || m_isDestroyed )
	{
		return;
	}

	if ( m_isDead )
	{
		m_velocity = Vec3::ZERO;
		m_acceleration = Vec3::ZERO;

		if ( m_deadTimer.m_period <= 0.0 || m_deadTimer.HasPeriodElapsed() )
		{
			m_isDestroyed = true;
		}
		return;
	}

	UpdatePhysics();
}


//-----------------------------------------------------------------------------------------------
void Actor::UpdatePhysics()
{
	if ( m_definition == nullptr || !m_definition->m_isSimulated || m_isDead || m_isDestroyed )
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
		m_position.z = 0.f;
	}

	m_acceleration = Vec3::ZERO;
}


//-----------------------------------------------------------------------------------------------
void Actor::TakeDamage( int damageAmount )
{
	if ( m_definition == nullptr || damageAmount <= 0 || m_isDead || m_isDestroyed )
	{
		return;
	}

	m_currentHealth -= damageAmount;

	if ( m_currentHealth < 0 )
	{
		m_isDead = true;
		m_currentHealth = 0;

		m_velocity = Vec3::ZERO;
		m_acceleration = Vec3::ZERO;

		m_deadTimer.m_period = m_definition->corpseLifetime;
		if ( m_deadTimer.m_period <= 0.0 )
		{
			m_isDestroyed = true;
		}
		else
		{
			m_deadTimer.Start();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Actor::AddForce( Vec3 const& force )
{
	if ( m_definition == nullptr || !m_definition->m_isSimulated || m_isDead || m_isDestroyed )
	{
		return;
	}

	m_acceleration += force;
}


//-----------------------------------------------------------------------------------------------
void Actor::AddImpulse( Vec3 const& impulse )
{
	if ( m_definition == nullptr || !m_definition->m_isSimulated || m_isDead || m_isDestroyed )
	{
		return;
	}

	m_velocity += impulse;
}


//-----------------------------------------------------------------------------------------------
void Actor::OnCollide()
{

}


//-----------------------------------------------------------------------------------------------
void Actor::MoveInDirection( Vec3 const& moveDirection, float speed )
{
	if ( m_definition == nullptr || moveDirection == Vec3::ZERO || speed <= 0.f )
	{
		return;
	}

	Vec3 moveDirectionNormalized = moveDirection.GetNormalized();
	Vec3 movementForce = moveDirectionNormalized * ( speed * m_definition->m_drag );
	AddForce( movementForce );
}


//-----------------------------------------------------------------------------------------------
void Actor::TurnInDirection( Vec3 const& turnDirection, float maxTurnDegrees )
{
	if ( m_definition == nullptr || turnDirection == Vec3::ZERO || maxTurnDegrees <= 0.f )
	{
		return;
	}

	float targetYaw = turnDirection.GetOrientationAboutZDegrees();
	float yawDifference = GetShortestAngularDispDegrees( m_orientation.m_yawDegrees, targetYaw );
	float clampedYawChange = GetClamped( yawDifference, -maxTurnDegrees, maxTurnDegrees );

	m_orientation.m_yawDegrees += clampedYawChange;
}


//-----------------------------------------------------------------------------------------------
void Actor::Attack()
{
	if ( m_definition == nullptr || m_currentWeapon == nullptr || m_isDead || m_isDestroyed )
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

	Rgba8 modelTint = Rgba8::WHITE;
	if ( m_isDead )
	{
		modelTint = Rgba8( 80, 80, 80, 255 );
	}

	g_engine->m_renderer->SetModelConstants( modelMatrix, modelTint );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
	g_engine->m_renderer->DrawVertexArray( m_verts );

	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
}


//-----------------------------------------------------------------------------------------------
void Actor::SetSolidColor( Rgba8 const& color )
{
	m_color = color;

	int solidEndIndex = m_solidVertCount;
	if ( solidEndIndex > ( int ) m_verts.size() )
	{
		solidEndIndex = ( int ) m_verts.size();
	}

	Rgba8 lighterColor = Rgba8(
		static_cast<unsigned char>( GetClamped( static_cast<float>( color.r ) + 150.f, 0.f, 255.f ) ),
		static_cast<unsigned char>( GetClamped( static_cast<float>( color.g ) + 150.f, 0.f, 255.f ) ),
		static_cast<unsigned char>( GetClamped( static_cast<float>( color.b ) + 150.f, 0.f, 255.f ) ),
		color.a );

	for ( int index = 0; index < solidEndIndex; ++index )
	{
		m_verts[index].m_color = color;
	}

	for ( int index = solidEndIndex; index < ( int ) m_verts.size(); ++index )
	{
		m_verts[index].m_color = lighterColor;
	}
}


//-----------------------------------------------------------------------------------------------
Mat44 Actor::GetModelMatrix() const
{
	Mat44 modelToWorld;
	Mat44 translation = Mat44::MakeTranslation3D( m_position );

	EulerAngles yawOnlyOrientation( m_orientation.m_yawDegrees, 0.f, 0.f );
	Mat44 rotation = yawOnlyOrientation.GetAsMatrix_IFwd_JLeft_KUp();

	modelToWorld.Append( translation );
	modelToWorld.Append( rotation );

	return modelToWorld;
}