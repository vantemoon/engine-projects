#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class  Actor;      // Forward declaration
class  Camera;     // Forward declaration
class  Map;        // Forward declaration
class  Player;     // Forward declaration
struct Vec3;       // Forward declaration
struct Vertex;     // Forward declaration


//-----------------------------------------------------------------------------------------------
class Game
{
public:
	std::vector<Map*>	m_maps;
	Map*				m_currentMap = nullptr;

	Player*				m_player = nullptr;
	Camera*				m_screenCamera = nullptr;

	GameState			m_currentGameState;
	Clock*				m_gameClock = nullptr;

	bool				m_hasControlsBeenShown = false;

public:
	Game();
	~Game();

	void Startup();
	void LoadGameConfigFromFile( char const* filepath );
	void LoadMaps();
	void Shutdown();
	void DestroyMaps();

	void Update();
	void UpdateAttractMode();
	void UpdatePlayer();
	void UpdateCurrentMap();
	void UpdateFromKeyboard();
	void UpdateFromController();

	void Render()													  const;
	void RenderMap()												  const;
	void RenderEntities()											  const;
	void RenderHUD()												  const;
	void RenderAttractMode()										  const;
	
	void ScreenShake( float intensity );
	void Reset();

	static bool Command_Controls( EventArgs& args );

protected:
	void DeleteGarbageEntities();
	Vec3 TransformWorldToScreen( Vec3 const& worldPosition )		   const;
	bool IsOnScreen( Vec2 const& worldPosition, float cosmeticRadius ) const;
	void AddInstructionsToDevConsole()								   const;
	void AddVertsForCube( std::vector<Vertex>& verts, float size) const;
	void UpdateFakeProjectileFromKeyboard( float deltaSeconds );
};