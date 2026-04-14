#pragma once
#include "Game/ActorDefinition.hpp"
#include "Game/ActorHandle.hpp"
#include "Game/Weapon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class Map;


//-----------------------------------------------------------------------------------------------
class Actor
{
public:
	Actor();
	Actor( ActorHandle handle, ActorDefinition const* definition, Map* map );
	~Actor();

	void Update();
	void Render() const;
	Mat44 GetModelMatrix() const;

public:
	ActorHandle m_handle;
	ActorDefinition const* m_definition = nullptr;
	Map* m_map = nullptr;

	Vec3 m_position;
	EulerAngles m_orientation;
	Vec3 m_velocity;
	Vec3 m_acceleration;

	std::vector<Vertex> m_verts;
	Rgba8 m_color;

	int m_maxHealth = 1;
	int m_currentHealth = 1;

	bool m_isStatic = false;

	std::vector<Weapon> m_inventory;
	Weapon* m_currentWeapon = nullptr;
};