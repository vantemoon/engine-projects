#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"
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
	std::vector<Map*>		m_maps;
	Map*					m_currentMap = nullptr;

	std::vector<Player*>	m_players;
	Player*					m_currentRenderingPlayer = nullptr;
	Camera*					m_screenCamera = nullptr;

	GameState				m_currentGameState;
	Clock*					m_gameClock = nullptr;

	bool					m_hasControlsBeenShown = false;

	float					m_musicVolume = 0.1f;
	SoundID					m_mainMenuMusicID = MISSING_SOUND_ID;
	SoundID					m_gameMusicID = MISSING_SOUND_ID;
	SoundID					m_buttonClickSoundID = MISSING_SOUND_ID;
	SoundPlaybackID			m_mainMenuMusicPlaybackID = MISSING_SOUND_ID;
	SoundPlaybackID			m_gameMusicPlaybackID = MISSING_SOUND_ID;
	bool 					m_isMainMenuMusicPlaying = false;
	bool					m_isGameMusicPlaying = false;

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
	void UpdatePlayers( float deltaSeconds );
	void UpdateCurrentMap( float deltaSeconds );
	void UpdateFromKeyboard();
	void UpdateFromController();

	void Render();
	void RenderMap()												  const;
	void RenderEntities()											  const;
	void RenderHUD()												  const;
	void RenderAttractMode()										  const;
	void RenderLobby()												  const;
	
	void StartGameFromLobby();
	void Reset();

	Player* GetPlayerFromActor( Actor const* actor ) const;
	static bool Command_Controls( EventArgs& args );

protected:
	void InitializeGameAudio();
	void DeleteGarbageEntities();
	Vec3 TransformWorldToScreen( Vec3 const& worldPosition )		   const;
	bool IsOnScreen( Vec2 const& worldPosition, float cosmeticRadius ) const;
	void AddInstructionsToDevConsole()								   const;
	void AddVertsForCube( std::vector<Vertex>& verts, float size)	   const;
	void UpdateAudioListeners();
};