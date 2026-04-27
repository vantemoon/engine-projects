#pragma once
#include "Engine/Renderer/SpriteDefinition.hpp"


//-----------------------------------------------------------------------------------------------
enum SpriteAnimPlaybackType
{
	ONCE,		// for 5-frame animation, plays 0,1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4...
	LOOP,		// for 5-frame animation, plays 0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0...
	PINGPONG,	// for 5-frame animation, plays 0,1,2,3,4,3,2,1,0,1,2,3,4,3,2,1,0,1,2,3,4,3,2,1,0,1...
};


//-----------------------------------------------------------------------------------------------
class SpriteAnimDefinition
{
public:
	SpriteAnimDefinition() = default;
	SpriteAnimDefinition( SpriteSheet const& sheet, int startSpriteIndex, int endSpriteIndex,
						  float framesPerSecond, SpriteAnimPlaybackType playbackType = SpriteAnimPlaybackType::LOOP );

	SpriteDefinition const& GetSpriteDefAtTime( float seconds ) const; // Most of the logic for this class is done here!

private:
	SpriteSheet const&		m_spriteSheet;
	int						m_startSpriteIndex = -1;
	int						m_endSpriteIndex = -1;
	float					m_framesPerSecond = 1.f;
	SpriteAnimPlaybackType	m_playbackType = SpriteAnimPlaybackType::LOOP;
};