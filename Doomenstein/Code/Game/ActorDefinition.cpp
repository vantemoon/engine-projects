#include "Game/ActorDefinition.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"


//-----------------------------------------------------------------------------------------------
std::map<std::string, ActorDefinition*> ActorDefinition::s_definitions;


//-----------------------------------------------------------------------------------------------
void ActorDefinition::InitializeDefinitions()
{
	LoadDefinitionsFromFile( "Data/Definitions/ProjectileActorDefinitions.xml", true );
	WeaponDefinition::InitializeDefinitions();
	LoadDefinitionsFromFile( "Data/Definitions/ActorDefinitions.xml", false );
}


//-----------------------------------------------------------------------------------------------
void ActorDefinition::LoadDefinitionsFromFile( char const* filepath, bool isProjectile )
{
	XmlDocument actorDefDoc;
	XmlResult loadResult = actorDefDoc.LoadFile( filepath );
	if ( loadResult != tinyxml2::XML_SUCCESS )
	{
		return;
	}

	tinyxml2::XMLElement* rootElement = actorDefDoc.RootElement();
	if ( rootElement == nullptr )
	{
		return;
	}

	tinyxml2::XMLElement* actorDefElement = nullptr;
	if ( std::string( rootElement->Name() ) == "ActorDefinition" )
	{
		actorDefElement = rootElement;
	}
	else
	{
		actorDefElement = rootElement->FirstChildElement( "ActorDefinition" );
	}

	while ( actorDefElement != nullptr )
	{
		ActorDefinition newActorDef;
		newActorDef.m_isProjectile = isProjectile;

		std::string actorName = ParseXmlAttribute( *actorDefElement, "name", "" );
		newActorDef.m_isVisible = ParseXmlAttribute( *actorDefElement, "visible", newActorDef.m_isVisible );
		newActorDef.m_health = ParseXmlAttribute( *actorDefElement, "health", newActorDef.m_health );
		newActorDef.corpseLifetime = ParseXmlAttribute( *actorDefElement, "corpseLifetime", newActorDef.corpseLifetime );
		newActorDef.m_faction = ParseXmlAttribute( *actorDefElement, "faction", newActorDef.m_faction );
		newActorDef.m_canBePossessed = ParseXmlAttribute( *actorDefElement, "canBePossessed", newActorDef.m_canBePossessed );

		// Collision
		tinyxml2::XMLElement* collisionElement = actorDefElement->FirstChildElement( "Collision" );
		if ( collisionElement != nullptr )
		{
			newActorDef.m_physicsRadius = ParseXmlAttribute( *collisionElement, "radius", newActorDef.m_physicsRadius );
			newActorDef.m_physicsHeight = ParseXmlAttribute( *collisionElement, "height", newActorDef.m_physicsHeight );
			newActorDef.m_collideWithWorld = ParseXmlAttribute( *collisionElement, "collidesWithWorld", newActorDef.m_collideWithWorld );
			newActorDef.m_collideWithActors = ParseXmlAttribute( *collisionElement, "collidesWithActors", newActorDef.m_collideWithActors );
			newActorDef.m_dieOnCollide = ParseXmlAttribute( *collisionElement, "dieOnCollide", newActorDef.m_dieOnCollide );

			char const* damageOnCollideText = collisionElement->Attribute( "damageOnCollide" );
			if ( damageOnCollideText != nullptr )
			{
				newActorDef.m_damageOnCollide.SetFromText( damageOnCollideText );
			}

			newActorDef.m_impulseOnCollide = ParseXmlAttribute( *collisionElement, "impulseOnCollide", newActorDef.m_impulseOnCollide );
		}

		// Physics
		tinyxml2::XMLElement* physicsElement = actorDefElement->FirstChildElement( "Physics" );
		if ( physicsElement != nullptr )
		{
			newActorDef.m_isSimulated = ParseXmlAttribute( *physicsElement, "simulated", newActorDef.m_isSimulated );
			newActorDef.m_isFlying = ParseXmlAttribute( *physicsElement, "flying", newActorDef.m_isFlying ); // fixed
			newActorDef.m_walkSpeed = ParseXmlAttribute( *physicsElement, "walkSpeed", newActorDef.m_walkSpeed );
			newActorDef.m_runSpeed = ParseXmlAttribute( *physicsElement, "runSpeed", newActorDef.m_runSpeed );
			newActorDef.m_drag = ParseXmlAttribute( *physicsElement, "drag", newActorDef.m_drag );
			newActorDef.m_turnSpeed = ParseXmlAttribute( *physicsElement, "turnSpeed", newActorDef.m_turnSpeed );
		}

		// Camera
		tinyxml2::XMLElement* cameraElement = actorDefElement->FirstChildElement( "Camera" );
		if ( cameraElement != nullptr )
		{
			newActorDef.m_eyeHeight = ParseXmlAttribute( *cameraElement, "eyeHeight", newActorDef.m_eyeHeight );
			newActorDef.m_cameraFOVDegrees = ParseXmlAttribute( *cameraElement, "cameraFOV", newActorDef.m_cameraFOVDegrees );
		}

		// AI
		tinyxml2::XMLElement* aiElement = actorDefElement->FirstChildElement( "AI" );
		if ( aiElement != nullptr )
		{
			newActorDef.m_aiEnabled = ParseXmlAttribute( *aiElement, "aiEnabled", newActorDef.m_aiEnabled );
			newActorDef.m_sightRadius = ParseXmlAttribute( *aiElement, "sightRadius", newActorDef.m_sightRadius );
			newActorDef.m_sightAngle = ParseXmlAttribute( *aiElement, "sightAngle", newActorDef.m_sightAngle );
		}

		// Inventory
		tinyxml2::XMLElement* inventoryElement = actorDefElement->FirstChildElement( "Inventory" );
		if ( inventoryElement != nullptr )
		{
			tinyxml2::XMLElement* weaponElement = inventoryElement->FirstChildElement( "Weapon" );
			while ( weaponElement != nullptr )
			{
				std::string weaponName = ParseXmlAttribute( *weaponElement, "name", "" );
				if ( !weaponName.empty() )
				{
					newActorDef.m_weaponDefNames.push_back( weaponName );
				}
				weaponElement = weaponElement->NextSiblingElement( "Weapon" );
			}
		}

		if ( !actorName.empty() )
		{
			auto existingDefIter = s_definitions.find( actorName );
			if ( existingDefIter != s_definitions.end() )
			{
				delete existingDefIter->second;
				existingDefIter->second = nullptr;
			}
			s_definitions[actorName] = new ActorDefinition( newActorDef );
		}

		actorDefElement = actorDefElement->NextSiblingElement( "ActorDefinition" );
	}
}


//-----------------------------------------------------------------------------------------------
void ActorDefinition::ClearDefinitions()
{
	for ( auto& defPair : s_definitions )
	{
		delete defPair.second;
	}
	s_definitions.clear();

	WeaponDefinition::ClearDefinitions();
}


//-----------------------------------------------------------------------------------------------
ActorDefinition const* ActorDefinition::GetActorDefinitionByName( std::string const& name )
{
	auto defIter = s_definitions.find( name );
	if ( defIter != s_definitions.end() )
	{
		return defIter->second;
	}
	return nullptr;
}