#pragma once
#include <map>
#include <string>


//-----------------------------------------------------------------------------------------------
class Texture;


//-----------------------------------------------------------------------------------------------
class CardDefinition
{
public:
	std::string m_name = "Card";
	std::string m_category = "None";
	std::string m_description = "";
	std::string m_texturePath = "";

	Texture* m_texture = nullptr;

	int m_maxStackSize = 1;

	static std::map<std::string, CardDefinition*> s_definitions;

public:
	static void InitializeDefinitions();
	static void ClearDefinitions();
	static CardDefinition const* GetCardDefinitionByName( std::string const& name );

private:
	static void LoadDefinitionsFromFile( char const* filepath );
};