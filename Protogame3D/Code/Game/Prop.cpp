#include "Game/Prop.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Prop::Prop( Game* owner )
	: Entity( owner )
{
}


//-----------------------------------------------------------------------------------------------
Prop::~Prop()
{
}


//-----------------------------------------------------------------------------------------------
void Prop::Update( float deltaSeconds )
{
	m_orientation.m_yawDegrees += m_angularVelocity.m_yawDegrees * deltaSeconds;
	m_orientation.m_pitchDegrees += m_angularVelocity.m_pitchDegrees * deltaSeconds;
	m_orientation.m_rollDegrees += m_angularVelocity.m_rollDegrees * deltaSeconds;
}


//-----------------------------------------------------------------------------------------------
void Prop::Render() const
{
	Entity::Render();

	g_engine->m_renderer->SetBlendMode( BlendMode::OPAQUE );
	g_engine->m_renderer->BindTexture( m_texture );
	g_engine->m_renderer->DrawVertexArray( ( int ) m_vertexes.size(), m_vertexes.data() );
}