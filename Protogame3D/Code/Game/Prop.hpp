#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Vertex.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class Prop : public Entity
{
public:
	Prop( Game* owner );
	~Prop();

	void Update( float deltaSeconds ) override;
	void Render() const override;

public:
	std::vector<Vertex> m_vertexes;
	Rgba8               m_color = Rgba8::WHITE;
	Texture*			m_texture = nullptr;
};