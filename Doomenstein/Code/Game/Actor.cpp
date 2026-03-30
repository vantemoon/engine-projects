#include "Game/Actor.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
Actor::Actor()
	: m_position( Vec3::ZERO )
	, m_orientation( EulerAngles::ZERO )
	, m_color( Rgba8::WHITE )
{
}


//-----------------------------------------------------------------------------------------------
Actor::~Actor()
{
}


//-----------------------------------------------------------------------------------------------
void Actor::Update()
{
	if ( m_isStatic )
	{
		return;
	}
}


//-----------------------------------------------------------------------------------------------
void Actor::Render() const
{
	if ( g_engine == nullptr || g_engine->m_renderer == nullptr )
	{
		return;
	}

	if ( m_physicsRadius <= 0.f || m_physicsHeight <= 0.f )
	{
		return;
	}

	std::vector<Vertex> solidCylinderVerts;
	std::vector<Vertex> wireCylinderVerts;

	Vec3 cylinderStart = Vec3::ZERO;
	Vec3 cylinderEnd = Vec3( 0.f, 0.f, m_physicsHeight );

	Rgba8 lighterColor = Rgba8(
		static_cast<unsigned char>( m_color.r + ( 255 - m_color.r ) * 0.7f ),
		static_cast<unsigned char>( m_color.g + ( 255 - m_color.g ) * 0.7f ),
		static_cast<unsigned char>( m_color.b + ( 255 - m_color.b ) * 0.7f ),
		255 );

	AddVertsForCylinder3D( solidCylinderVerts, cylinderStart, cylinderEnd, m_physicsRadius, m_color );
	AddVertsForCylinder3D( wireCylinderVerts, cylinderStart, cylinderEnd, m_physicsRadius, lighterColor );

	Mat44 modelMatrix = GetModelMatrix();

	g_engine->m_renderer->BindTexture( nullptr );

	g_engine->m_renderer->SetModelConstants( modelMatrix, Rgba8::WHITE );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::SOLID_CULL_BACK );
	g_engine->m_renderer->DrawVertexArray( solidCylinderVerts );

	g_engine->m_renderer->SetModelConstants( modelMatrix, Rgba8::WHITE );
	g_engine->m_renderer->SetRasterizerMode( RasterizerMode::WIREFRAME_CULL_NONE );
	g_engine->m_renderer->DrawVertexArray( wireCylinderVerts );

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