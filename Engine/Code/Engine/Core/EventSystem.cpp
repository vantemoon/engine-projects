#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include <algorithm>
#include <cctype>


//-----------------------------------------------------------------------------------------------
EventSystem::EventSystem( EventSystemConfig const& config )
	: m_config( config )
{
}


//-----------------------------------------------------------------------------------------------
void EventSystem::Startup()
{
}


//-----------------------------------------------------------------------------------------------
void EventSystem::Shutdown()
{
}


//-----------------------------------------------------------------------------------------------
void EventSystem::BeginFrame()
{
}


//-----------------------------------------------------------------------------------------------
void EventSystem::EndFrame()
{
}


//-----------------------------------------------------------------------------------------------
void EventSystem::SubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction funtionPtr )
{
	m_subscriptionListsByEventName[eventName].push_back( funtionPtr );
}


//-----------------------------------------------------------------------------------------------
void EventSystem::UnsubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction funtionPtr )
{
	EventCallbackFunctionList& callbackList = m_subscriptionListsByEventName[eventName];
	for ( EventCallbackFunction& callbackFunction : callbackList )
	{
		if ( callbackFunction == funtionPtr )
		{
			callbackFunction = nullptr;
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void EventSystem::FireEvent( std::string const& eventName, EventArgs& args )
{
	EventCallbackFunctionList& callbackList = m_subscriptionListsByEventName[eventName];
	for ( EventCallbackFunction callbackFunction : callbackList )
	{
		if ( callbackFunction != nullptr )
		{
			bool wasConsumed = callbackFunction( args );
			if ( wasConsumed )
			{
				break;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void EventSystem::FireEvent( std::string const& eventName )
{
	EventArgs args;
	FireEvent( eventName, args );
}


//-----------------------------------------------------------------------------------------------
std::vector<std::string> EventSystem::GetEventNames( bool includeRequiredArgs ) const
{
	std::vector<std::string> eventNames;
	for ( int eventIndex = 0; eventIndex < ( int ) m_subscriptionListsByEventName.size(); ++ eventIndex )
	{
		std::string eventName = std::next( m_subscriptionListsByEventName.begin(), eventIndex )->first;
		if ( eventName == "KeyDown" || eventName == "KeyUp" || eventName == "CharacterInput" )
		{
			continue; // skip raw keyboard events
		}

		if ( includeRequiredArgs )
		{
			auto requiredArgsIter = m_requiredArgsByEventName.find( eventName );
			if ( requiredArgsIter != m_requiredArgsByEventName.end() && !requiredArgsIter->second.empty() )
			{
				std::string eventNameWithArgs = eventName;
				for ( std::string const& requiredArg : requiredArgsIter->second )
				{
					eventNameWithArgs += " ";
					eventNameWithArgs += requiredArg;
				}
				eventNames.push_back( eventNameWithArgs );
				continue;
			}
		}

		eventNames.push_back( eventName );
	}
	return eventNames;
}


//-----------------------------------------------------------------------------------------------
void EventSystem::SetEventRequiredArgs( std::string const& eventName, EventRequiredArgsList const& requiredArgs )
{
	m_requiredArgsByEventName[eventName] = requiredArgs;
}


//-----------------------------------------------------------------------------------------------
EventRequiredArgsList EventSystem::GetEventRequiredArgs( std::string const& eventName ) const
{
	// Return a list of required arguments for the event, or an empty list if the event doesn't have required arguments
	auto requiredArgsIter = m_requiredArgsByEventName.find( eventName );
	if ( requiredArgsIter != m_requiredArgsByEventName.end() )
	{
		return requiredArgsIter->second;
	}
	else
	{
		return EventRequiredArgsList();
	}
}


//-----------------------------------------------------------------------------------------------
void SubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction funtionPtr )
{
	if ( g_engine && g_engine->m_eventSystem )
	{
		g_engine->m_eventSystem->SubscribeEventCallbackFunction( eventName, funtionPtr );
	}
}


//-----------------------------------------------------------------------------------------------
void UnsubscribeEventCallbackFunction( std::string const& eventName, EventCallbackFunction funtionPtr )
{
	if ( g_engine && g_engine->m_eventSystem )
	{
		g_engine->m_eventSystem->UnsubscribeEventCallbackFunction( eventName, funtionPtr );
	}
}


//-----------------------------------------------------------------------------------------------
void FireEvent( std::string const& eventName, EventArgs& args )
{
	if ( g_engine && g_engine->m_eventSystem )
	{
		g_engine->m_eventSystem->FireEvent( eventName, args );
	}
}


//-----------------------------------------------------------------------------------------------
void FireEvent( std::string const& eventName )
{
	if ( g_engine && g_engine->m_eventSystem )
	{
		g_engine->m_eventSystem->FireEvent( eventName );
	}
}