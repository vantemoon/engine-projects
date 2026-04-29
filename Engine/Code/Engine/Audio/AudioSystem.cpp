#include "Engine/Audio/AudioSystem.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

//-----------------------------------------------------------------------------------------------
// To disable audio entirely (and remove requirement for fmod.dll / fmod64.dll) for any game,
//	#define ENGINE_DISABLE_AUDIO in your game's Code/Game/EngineBuildPreferences.hpp file.
//
// Note that this #include is an exception to the rule "engine code doesn't know about game code".
//	Purpose: Each game can now direct the engine via #defines to build differently for that game.
//	Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
//
// SD1 NOTE: THIS MEANS *EVERY* GAME MUST HAVE AN EngineBuildPreferences.hpp FILE IN ITS CODE/GAME FOLDER!!
#if defined( ENGINE_DISABLE_AUDIO )
#pragma message( "AudioSystem disabled in EngineBuildPreferences.hpp" )
AudioSystem::AudioSystem( AudioConfig const& config ) : m_config( config ) {}
AudioSystem::~AudioSystem() {}
void AudioSystem::Startup() {}
void AudioSystem::Shutdown() {}
void AudioSystem::BeginFrame() {}
void AudioSystem::EndFrame() {}
#else
#pragma message( "AudioSystem (FMOD) enabled in EngineBuildPreferences.hpp" )


//-----------------------------------------------------------------------------------------------
// Link in the appropriate FMOD static library (32-bit or 64-bit)
//
#if defined( _WIN64 )
#pragma comment( lib, "ThirdParty/fmod/fmod64_vc.lib" )
#else
#pragma comment( lib, "ThirdParty/fmod/fmod_vc.lib" )
#endif


//-----------------------------------------------------------------------------------------------
// Initialization code based on example from "FMOD Studio Programmers API for Windows"
//
AudioSystem::AudioSystem( AudioConfig const& config )
	: m_config( config )
	, m_fmodSystem( nullptr )
{
}


//-----------------------------------------------------------------------------------------------
AudioSystem::~AudioSystem()
{
}


//------------------------------------------------------------------------------------------------
void AudioSystem::Startup()
{
	FMOD_RESULT result;
	result = FMOD::System_Create( &m_fmodSystem );
	ValidateResult( result );

	result = m_fmodSystem->init( 512, FMOD_INIT_3D_RIGHTHANDED, nullptr );
	ValidateResult( result );

	result = m_fmodSystem->set3DSettings( 1.f, 1.f, 1.f );
	ValidateResult( result );
}


//------------------------------------------------------------------------------------------------
void AudioSystem::Shutdown()
{
	FMOD_RESULT result = m_fmodSystem->release();
	ValidateResult( result );

	m_fmodSystem = nullptr; // #Fixme: do we delete/free the object also, or just do this?
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::BeginFrame()
{
	m_fmodSystem->update();
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::EndFrame()
{
}


//-----------------------------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGetSound( const std::string& soundFilePath )
{
	std::map< std::string, SoundID >::iterator found = m_registeredSoundIDs.find( soundFilePath );
	if( found != m_registeredSoundIDs.end() )
	{
		return found->second;
	}
	else
	{
		FMOD::Sound* newSound = nullptr;
		m_fmodSystem->createSound( soundFilePath.c_str(), FMOD_DEFAULT, nullptr, &newSound );
		if( newSound )
		{
			SoundID newSoundID = m_registeredSounds.size();
			m_registeredSoundIDs[ soundFilePath ] = newSoundID;
			m_registeredSounds.push_back( newSound );
			return newSoundID;
		}
	}

	return MISSING_SOUND_ID;
}


//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::StartSound( SoundID soundID, bool isLooped, float volume, float balance, float speed, bool isPaused )
{
	size_t numSounds = m_registeredSounds.size();
	if( soundID < 0 || soundID >= numSounds )
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if( !sound )
		return MISSING_SOUND_ID;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( sound, nullptr, true, &channelAssignedToSound );
	if( channelAssignedToSound )
	{
		int loopCount = isLooped ? -1 : 0;
		unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channelAssignedToSound->setMode(playbackMode);
		channelAssignedToSound->getFrequency( &frequency );
		channelAssignedToSound->setFrequency( frequency * speed );
		channelAssignedToSound->setVolume( volume );
		channelAssignedToSound->setPan( balance );
		channelAssignedToSound->setLoopCount( loopCount );
		channelAssignedToSound->setPaused( isPaused );
	}

	return (SoundPlaybackID) channelAssignedToSound;
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::StopSound( SoundPlaybackID soundPlaybackID )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to stop sound on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->stop();
}


//-----------------------------------------------------------------------------------------------
// Volume is in [0,1]
//
void AudioSystem::SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setVolume( volume );
}


//-----------------------------------------------------------------------------------------------
// Balance is in [-1,1], where 0 is L/R centered
//
void AudioSystem::SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set balance on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setPan( balance );
}


//-----------------------------------------------------------------------------------------------
// Speed is frequency multiplier (1.0 == normal)
//	A speed of 2.0 gives 2x frequency, i.e. exactly one octave higher
//	A speed of 0.5 gives 1/2 frequency, i.e. exactly one octave lower
//
void AudioSystem::SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set speed on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	float frequency;
	FMOD::Sound* currentSound = nullptr;
	channelAssignedToSound->getCurrentSound( &currentSound );
	if( !currentSound )
		return;

	int ignored = 0;
	currentSound->getDefaults( &frequency, &ignored );
	channelAssignedToSound->setFrequency( frequency * speed );
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::ValidateResult( FMOD_RESULT result )
{
	if( result != FMOD_OK )
	{
		ERROR_RECOVERABLE( Stringf( "Engine/Audio SYSTEM ERROR: Got error result code %i - error codes listed in fmod_common.h\n", (int) result ) );
	}
}


//-----------------------------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGetSound3D( std::string const& soundFilePath )
{
	std::map< std::string, SoundID >::iterator found = m_registeredSoundIDs.find( soundFilePath );
	if ( found != m_registeredSoundIDs.end() )
	{
		return found->second;
	}

	FMOD::Sound* newSound = nullptr;
	FMOD_RESULT result = m_fmodSystem->createSound(
		soundFilePath.c_str(),
		FMOD_3D,
		nullptr,
		&newSound );

	ValidateResult( result );

	if ( newSound != nullptr )
	{
		newSound->set3DMinMaxDistance( 1.f, 50.f );

		SoundID newSoundID = m_registeredSounds.size();
		m_registeredSoundIDs[soundFilePath] = newSoundID;
		m_registeredSounds.push_back( newSound );
		return newSoundID;
	}

	return MISSING_SOUND_ID;
}


//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::StartSound3D( SoundID soundID, Vec3 const& position, bool isLooped, float volume, float speed, bool isPaused )
{
	size_t numSounds = m_registeredSounds.size();
	if ( soundID < 0 || soundID >= numSounds )
	{
		return MISSING_SOUND_ID;
	}

	FMOD::Sound* sound = m_registeredSounds[soundID];
	if ( sound == nullptr )
	{
		return MISSING_SOUND_ID;
	}

	FMOD::Channel* channel = nullptr;
	FMOD_RESULT result = m_fmodSystem->playSound( sound, nullptr, true, &channel );
	ValidateResult( result );

	if ( channel != nullptr )
	{
		int loopCount = isLooped ? -1 : 0;
		unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;

		float frequency = 0.f;
		channel->setMode( playbackMode | FMOD_3D );
		channel->getFrequency( &frequency );
		channel->setFrequency( frequency * speed );
		channel->setVolume( volume );
		channel->setLoopCount( loopCount );

		FMOD_VECTOR fmodPos;
		fmodPos.x = position.x;
		fmodPos.y = position.y;
		fmodPos.z = position.z;

		FMOD_VECTOR fmodVel;
		fmodVel.x = 0.f;
		fmodVel.y = 0.f;
		fmodVel.z = 0.f;

		channel->set3DAttributes( &fmodPos, &fmodVel );

		channel->setPaused( isPaused );
	}

	return ( SoundPlaybackID ) channel;
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::SetListener( Vec3 const& position, Vec3 const& forward, Vec3 const& up )
{
	FMOD_VECTOR fmodPos;
	fmodPos.x = position.x;
	fmodPos.y = position.y;
	fmodPos.z = position.z;

	FMOD_VECTOR fmodVel;
	fmodVel.x = 0.f;
	fmodVel.y = 0.f;
	fmodVel.z = 0.f;

	FMOD_VECTOR fmodForward;
	fmodForward.x = forward.x;
	fmodForward.y = forward.y;
	fmodForward.z = forward.z;

	FMOD_VECTOR fmodUp;
	fmodUp.x = up.x;
	fmodUp.y = up.y;
	fmodUp.z = up.z;

	FMOD_RESULT result = m_fmodSystem->set3DListenerAttributes(
		0,
		&fmodPos,
		&fmodVel,
		&fmodForward,
		&fmodUp );

	ValidateResult( result );
}


#endif // !defined( ENGINE_DISABLE_AUDIO )
