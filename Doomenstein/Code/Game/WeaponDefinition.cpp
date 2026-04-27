#include "Game/WeaponDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"


//-----------------------------------------------------------------------------------------------
std::map<std::string, WeaponDefinition*> WeaponDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void WeaponDefinition::InitializeDefinitions()
{
	XmlDocument weaponDefDoc;
	XmlResult loadResult = weaponDefDoc.LoadFile( "Data/Definitions/WeaponDefinitions.xml" );
	if ( loadResult != tinyxml2::XML_SUCCESS )
	{
		return;
	}
	tinyxml2::XMLElement* rootElement = weaponDefDoc.RootElement();
	if ( rootElement == nullptr )
	{
		return;
	}
	tinyxml2::XMLElement* weaponDefElement = rootElement->FirstChildElement( "WeaponDefinition" );
	while ( weaponDefElement != nullptr )
	{
		WeaponDefinition newWeaponDef;
		newWeaponDef.m_name = ParseXmlAttribute( *weaponDefElement, "name", newWeaponDef.m_name );
		newWeaponDef.m_refireTime = ParseXmlAttribute( *weaponDefElement, "refireTime", newWeaponDef.m_refireTime );
		
		newWeaponDef.m_rayCount = ParseXmlAttribute( *weaponDefElement, "rayCount", newWeaponDef.m_rayCount );
		newWeaponDef.m_rayCone = ParseXmlAttribute( *weaponDefElement, "rayCone", newWeaponDef.m_rayCone );
		newWeaponDef.m_rayRange = ParseXmlAttribute( *weaponDefElement, "rayRange", newWeaponDef.m_rayRange );
		char const* rayDamageText = weaponDefElement->Attribute( "rayDamage" );
		if ( rayDamageText != nullptr )
		{
			newWeaponDef.m_rayDamage.SetFromText( rayDamageText );
		}
		newWeaponDef.m_rayImpulse = ParseXmlAttribute( *weaponDefElement, "rayImpulse", newWeaponDef.m_rayImpulse );
		
		newWeaponDef.m_projectileCount = ParseXmlAttribute( *weaponDefElement, "projectileCount", newWeaponDef.m_projectileCount );
		newWeaponDef.m_projectileCone = ParseXmlAttribute( *weaponDefElement, "projectileCone", newWeaponDef.m_projectileCone );
		newWeaponDef.m_projectileSpeed = ParseXmlAttribute( *weaponDefElement, "projectileSpeed", newWeaponDef.m_projectileSpeed );
		newWeaponDef.m_projectileActorDefName = ParseXmlAttribute( *weaponDefElement, "projectileActor", newWeaponDef.m_projectileActorDefName );
		
		newWeaponDef.m_meleeCount = ParseXmlAttribute( *weaponDefElement, "meleeCount", newWeaponDef.m_meleeCount );
		newWeaponDef.m_meleeRange = ParseXmlAttribute( *weaponDefElement, "meleeRange", newWeaponDef.m_meleeRange );
		newWeaponDef.m_meleeArc = ParseXmlAttribute( *weaponDefElement, "meleeArc", newWeaponDef.m_meleeArc );
		char const* meleeDamageText = weaponDefElement->Attribute( "meleeDamage" );
		if ( meleeDamageText != nullptr )
		{
			newWeaponDef.m_meleeDamage.SetFromText( meleeDamageText );
		}
		newWeaponDef.m_meleeImpulse = ParseXmlAttribute( *weaponDefElement, "meleeImpulse", newWeaponDef.m_meleeImpulse );

		tinyxml2::XMLElement* hudElement = weaponDefElement->FirstChildElement( "HUD" );
		if ( hudElement != nullptr )
		{
			newWeaponDef.m_hudShader = ParseXmlAttribute( *hudElement, "shader", newWeaponDef.m_hudShader );
			newWeaponDef.m_hudTexture = ParseXmlAttribute( *hudElement, "baseTexture", newWeaponDef.m_hudTexture );
			newWeaponDef.m_rectileTexture = ParseXmlAttribute( *hudElement, "reticleTexture", newWeaponDef.m_rectileTexture );
			newWeaponDef.m_rectileSize = ParseXmlAttribute( *hudElement, "reticleSize", newWeaponDef.m_rectileSize );
			newWeaponDef.m_spriteSize = ParseXmlAttribute( *hudElement, "spriteSize", newWeaponDef.m_spriteSize );
			newWeaponDef.m_spritePivot = ParseXmlAttribute( *hudElement, "spritePivot", newWeaponDef.m_spritePivot );

			tinyxml2::XMLElement* animationElement = hudElement->FirstChildElement( "Animation" );
			while ( animationElement != nullptr )
			{
				newWeaponDef.m_animationNames.push_back(
					ParseXmlAttribute( *animationElement, "name", std::string() ) );

				newWeaponDef.m_animationShaders.push_back(
					ParseXmlAttribute( *animationElement, "shader", std::string( "Default" ) ) );

				newWeaponDef.m_animationSpriteSheets.push_back(
					ParseXmlAttribute( *animationElement, "spriteSheet", std::string() ) );

				newWeaponDef.m_animationCellCounts.push_back(
					ParseXmlAttribute( *animationElement, "cellCount", Vec2::ONE ) );

				newWeaponDef.m_animationGroupSecondsPerFrame.push_back(
					ParseXmlAttribute( *animationElement, "secondsPerFrame", 0.f ) );

				newWeaponDef.m_animationStartFrames.push_back(
					ParseXmlAttribute( *animationElement, "startFrame", 0 ) );

				newWeaponDef.m_animationEndFrames.push_back(
					ParseXmlAttribute( *animationElement, "endFrame", 0 ) );

				animationElement = animationElement->NextSiblingElement( "Animation" );
			}
		}

		tinyxml2::XMLElement* soundsElement = weaponDefElement->FirstChildElement( "Sounds" );
		if ( soundsElement != nullptr )
		{
			tinyxml2::XMLElement* soundElement = soundsElement->FirstChildElement( "Sound" );
			while ( soundElement != nullptr )
			{
				newWeaponDef.m_soundTypes.push_back(
					ParseXmlAttribute( *soundElement, "sound", std::string() ) );

				newWeaponDef.m_soundNames.push_back(
					ParseXmlAttribute( *soundElement, "name", std::string() ) );

				soundElement = soundElement->NextSiblingElement( "Sound" );
			}
		}

		if ( !newWeaponDef.m_name.empty() )
		{
			auto existingDefIter = s_definitions.find( newWeaponDef.m_name );
			if ( existingDefIter != s_definitions.end() )
			{
				delete existingDefIter->second;
				existingDefIter->second = nullptr;
			}
			s_definitions[newWeaponDef.m_name] = new WeaponDefinition( newWeaponDef );
		}

		weaponDefElement = weaponDefElement->NextSiblingElement( "WeaponDefinition" );
	}
}


//-----------------------------------------------------------------------------------------------
void WeaponDefinition::ClearDefinitions()
{
	for ( auto& defPair : s_definitions )
	{
		delete defPair.second;
	}
	s_definitions.clear();
}


//-----------------------------------------------------------------------------------------------
WeaponDefinition const* WeaponDefinition::GetWeaponDefinitionByName( std::string const& name )
{
	auto defIter = s_definitions.find( name );
	if ( defIter != s_definitions.end() )
	{
		return defIter->second;
	}
	return nullptr;
}