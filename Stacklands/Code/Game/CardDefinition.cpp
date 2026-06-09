#include "Game/CardDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/XmlUtils.hpp"


//-----------------------------------------------------------------------------------------------
std::map<std::string, CardDefinition*> CardDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void CardDefinition::InitializeDefinitions()
{
	ClearDefinitions();
	LoadDefinitionsFromFile( "Data/Definitions/CardDefinitions.xml" );
}


//-----------------------------------------------------------------------------------------------
void CardDefinition::ClearDefinitions()
{
	for ( auto& pair : s_definitions )
	{
		delete pair.second;
	}

	s_definitions.clear();
}


//-----------------------------------------------------------------------------------------------
CardDefinition const* CardDefinition::GetCardDefinitionByName( std::string const& name )
{
	auto foundIter = s_definitions.find( name );
	if ( foundIter != s_definitions.end() )
	{
		return foundIter->second;
	}

	return nullptr;
}


//-----------------------------------------------------------------------------------------------
void CardDefinition::LoadDefinitionsFromFile( char const* filepath )
{
	XmlDocument cardDefDoc;
	XmlResult loadResult = cardDefDoc.LoadFile( filepath );
	GUARANTEE_OR_DIE( loadResult == tinyxml2::XML_SUCCESS, "Failed to load CardDefinitions.xml" );

	tinyxml2::XMLElement* rootElement = cardDefDoc.RootElement();
	GUARANTEE_OR_DIE( rootElement != nullptr, "CardDefinitions.xml has no root element" );

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

		newCardDef.m_name = ParseXmlAttribute( *cardDefElement, "name", newCardDef.m_name );
		newCardDef.m_category = ParseXmlAttribute( *cardDefElement, "category", newCardDef.m_category );
		newCardDef.m_description = ParseXmlAttribute( *cardDefElement, "description", newCardDef.m_description );
		newCardDef.m_texturePath = ParseXmlAttribute( *cardDefElement, "texture", newCardDef.m_texturePath );
		newCardDef.m_maxStackSize = ParseXmlAttribute( *cardDefElement, "maxStackSize", newCardDef.m_maxStackSize );
		GUARANTEE_OR_DIE( !newCardDef.m_name.empty(), "CardDefinition is missing name" );
		GUARANTEE_OR_DIE( !newCardDef.m_texturePath.empty(), "CardDefinition is missing texture path" );

		if ( !newCardDef.m_texturePath.empty() )
		{
			newCardDef.m_texture = g_engine->m_renderer->CreateOrGetTextureFromFile( newCardDef.m_texturePath.c_str() );
		}

		auto foundIter = s_definitions.find( newCardDef.m_name );
		GUARANTEE_OR_DIE( foundIter == s_definitions.end(), "Duplicate CardDefinition name found" );

		s_definitions[newCardDef.m_name] = new CardDefinition( newCardDef );

		cardDefElement = cardDefElement->NextSiblingElement( "CardDefinition" );
	}
}