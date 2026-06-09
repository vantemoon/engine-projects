#include "Game/CardDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
std::map<std::string, CardDefinition*> CardDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void CardDefinition::InitializeDefinitions()
{
	LoadDefinitionsFromFile( "Data/Definitions/CardDefinitions.xml" );
}


//-----------------------------------------------------------------------------------------------
void CardDefinition::LoadDefinitionsFromFile( char const* filepath )
{
	XmlDocument cardDefDoc;
	XmlResult loadResult = cardDefDoc.LoadFile( filepath );
	if ( loadResult != tinyxml2::XML_SUCCESS )
	{
		return;
	}

	tinyxml2::XMLElement* rootElement = cardDefDoc.RootElement();
	if ( rootElement == nullptr )
	{
		return;
	}

	tinyxml2::XMLElement* cardDefElement = nullptr;
	if ( std::string( rootElement->Name() ) == "CardDefinition" )
	{
		cardDefElement = rootElement;
	}
	else
	{
		cardDefElement = rootElement->FirstChildElement( "CardDefinition" );
	}

	while ( cardDefElement != nullptr )
	{
		CardDefinition newCardDef;

		std::string cardName = ParseXmlAttribute( *cardDefElement, "name", "" );
		newCardDef.m_name = cardName;

		newCardDef.m_textureFilePath = ParseXmlAttribute( *cardDefElement, "texture", "" );
		if ( !newCardDef.m_textureFilePath.empty() )
		{
			newCardDef.m_texture = g_engine->m_renderer->CreateOrGetTextureFromFile( newCardDef.m_textureFilePath.c_str() );
		}

		if ( !cardName.empty() )
		{
			auto existingDefIter = s_definitions.find( cardName );
			if ( existingDefIter != s_definitions.end() )
			{
				delete existingDefIter->second;
				existingDefIter->second = nullptr;
			}

			s_definitions[cardName] = new CardDefinition( newCardDef );
		}

		cardDefElement = cardDefElement->NextSiblingElement( "CardDefinition" );
	}
}


//-----------------------------------------------------------------------------------------------
void CardDefinition::ClearDefinitions()
{
	for ( auto& defPair : s_definitions )
	{
		delete defPair.second;
	}

	s_definitions.clear();
}


//-----------------------------------------------------------------------------------------------
CardDefinition const* CardDefinition::GetCardDefinitionByName( std::string const& name )
{
	auto defIter = s_definitions.find( name );
	if ( defIter != s_definitions.end() )
	{
		return defIter->second;
	}

	return nullptr;
}