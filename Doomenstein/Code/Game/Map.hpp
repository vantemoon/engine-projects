#pragma once
#include <vector>


//-----------------------------------------------------------------------------------------------
class Actor;


//-----------------------------------------------------------------------------------------------
class Map
{
public:
	Map();
	~Map();

	void Update();
	void Render() const;

private:
	void InitializeActors();

public:
	std::vector<Actor*> m_actors;
};