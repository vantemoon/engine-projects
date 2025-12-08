#include "Engine/Core/EventSystem.hpp"


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
			callbackFunction( args );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void EventSystem::FireEvent( std::string const& eventName )
{
	EventArgs args;
	FireEvent( eventName, args );
}