#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class  Camera;
class  ChessMatch;
class  Entity;
class  Player;
class  Prop;  
struct Vec3;  
struct Vertex;


//-----------------------------------------------------------------------------------------------
class Game
{
public:
	std::vector<Entity*> m_entities;
	Player*		m_player = nullptr;
	Camera*     m_screenCamera = nullptr;
	GameState   m_currentGameState;
	bool        m_isDebugFeaturesOn = false;
	Clock*		m_gameClock = nullptr;
	bool        m_hasControlsBeenShown = false;

	ChessMatch* m_chessMatch = nullptr;

public:
	Game();
	~Game();

	void Startup();
	void Update();
	void UpdateAttractMode();
	void UpdateEntities();
	void UpdateFromKeyboard();
	void UpdateFromController();
	void Render()													  const;
	void RenderEntities()											  const;
	void RenderHUD()												  const;
	void RenderAttractMode()										  const;
	void Reset();

	static bool Command_Controls( EventArgs& args );
	static bool Command_PrintBoard( EventArgs& args );

private:
	void DeleteGarbageEntities();
	void DebugDraw()												   const;
	Vec3 TransformWorldToScreen( Vec3 const& worldPosition )		   const;
	bool IsOnScreen( Vec2 const& worldPosition, float cosmeticRadius ) const;
	void AddInstructionsToDevConsole()								   const;
	void AddVertsForCube( std::vector<Vertex>& verts, float size) const;
	void PrintBoardStateToDevConsole()								   const;
};