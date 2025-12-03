#include "Engine/Renderer/SpriteAnimDefinition.hpp"


//-----------------------------------------------------------------------------------------------
SpriteAnimDefinition::SpriteAnimDefinition( SpriteSheet const& sheet, int startSpriteIndex, int endSpriteIndex,
											float framesPerSecond, SpriteAnimPlaybackType playbackType )
	: m_spriteSheet( sheet )
	, m_startSpriteIndex( startSpriteIndex )
	, m_endSpriteIndex( endSpriteIndex )
	, m_framesPerSecond( framesPerSecond )
	, m_playbackType( playbackType )
{
}


//-----------------------------------------------------------------------------------------------
SpriteDefinition const& SpriteAnimDefinition::GetSpriteDefAtTime( float seconds ) const
{
	int totalFrames = m_endSpriteIndex - m_startSpriteIndex + 1;
	if ( totalFrames <= 0 )
	{
		return m_spriteSheet.GetSpriteDefinition( 0 );
	}
	int currentFrame = static_cast<int>( seconds * m_framesPerSecond );
	switch ( m_playbackType )
	{
		case SpriteAnimPlaybackType::ONCE:
		{
			if ( currentFrame >= totalFrames )
			{
				currentFrame = totalFrames - 1;
			}
			break;
		}
		case SpriteAnimPlaybackType::LOOP:
		{
			currentFrame = currentFrame % totalFrames;
			break;
		}
		case SpriteAnimPlaybackType::PINGPONG:
		{
			int pingPongFrameCount = ( totalFrames * 2 ) - 2;
			currentFrame = currentFrame % pingPongFrameCount;
			if ( currentFrame >= totalFrames )
			{
				currentFrame = pingPongFrameCount - currentFrame;
			}
			break;
		}
	}
	int spriteIndex = m_startSpriteIndex + currentFrame;
	return m_spriteSheet.GetSpriteDefinition( spriteIndex );
}