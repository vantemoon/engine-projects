#include "Game/Entity.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/Mat44.hpp"


//-----------------------------------------------------------------------------------------------
Entity::Entity( Game* owner )
	: m_game( owner )
{
}


//-----------------------------------------------------------------------------------------------
Entity::~Entity()
{
}


//-----------------------------------------------------------------------------------------------
void Entity::Render() const
{
	g_engine->m_renderer->SetModelConstants( GetModelToWorldTransform(), m_color );
}

//-----------------------------------------------------------------------------------------------
Mat44 Entity::GetModelToWorldTransform() const
{
	Mat44 modelToWorld;
	Mat44 translation = Mat44::MakeTranslation3D( m_position );
	Mat44 rotation = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
	modelToWorld.Append( translation );
	modelToWorld.Append( rotation );
	return modelToWorld;
}