#include "Engine/Core/NamedStrings.hpp"


//-----------------------------------------------------------------------------------------------
void NamedStrings::PopulateFromXmlElementAttributes( XmlElement const& element )
{
	for ( const XmlAttribute* attr = element.FirstAttribute(); attr != nullptr; attr = attr->Next() )
	{
		std::string key = attr->Name();
		std::string value = attr->Value();
		m_keyValuePairs[key] = value;
	}
}


//-----------------------------------------------------------------------------------------------
void NamedStrings::SetValue( std::string const& keyName, std::string const& newValue )
{
	m_keyValuePairs[keyName] = newValue;
}


//-----------------------------------------------------------------------------------------------
std::string NamedStrings::GetValue( std::string const& keyName, std::string const& defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator iter = m_keyValuePairs.find( keyName );
	if ( iter == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	return iter->second;
}


//-----------------------------------------------------------------------------------------------
std::string NamedStrings::GetValue( std::string const& keyName, char const* defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator iter = m_keyValuePairs.find( keyName );
	if ( iter == m_keyValuePairs.end() )
	{
		return std::string( defaultValue );
	}
	return iter->second;
}


//-----------------------------------------------------------------------------------------------
bool NamedStrings::GetValue( std::string const& keyName, bool defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator iter = m_keyValuePairs.find( keyName );
	if ( iter == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	
	std::string value = iter->second;
	if ( value == "true")
	{
		return true;
	}
	else if ( value == "false")
	{
		return false;
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
int NamedStrings::GetValue( std::string const& keyName, int defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator iter = m_keyValuePairs.find( keyName );
	if ( iter == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	
	char const* value = iter->second.c_str();
	int intValue = static_cast<int>( atoi( value ) );
	return intValue;
}


//-----------------------------------------------------------------------------------------------
float NamedStrings::GetValue( std::string const& keyName, float defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator iter = m_keyValuePairs.find( keyName );
	if ( iter == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	
	char const* value = iter->second.c_str();
	float floatValue = static_cast<float>( atof( value ) );
	return floatValue;
}


//-----------------------------------------------------------------------------------------------
Rgba8 NamedStrings::GetValue( std::string const& keyName, Rgba8 const& defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator iter = m_keyValuePairs.find( keyName );
	if ( iter == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	
	std::string value = iter->second;
	Rgba8 rgbaValue;
	rgbaValue.SetFromText( value.c_str() );
	return rgbaValue;
}


//-----------------------------------------------------------------------------------------------
Vec2 NamedStrings::GetValue( std::string const& keyName, Vec2 const& defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator iter = m_keyValuePairs.find( keyName );
	if ( iter == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	
	std::string value = iter->second;
	Vec2 vec2Value;
	vec2Value.SetFromText( value.c_str() );
	return vec2Value;
}


//-----------------------------------------------------------------------------------------------
IntVec2 NamedStrings::GetValue( std::string const& keyName, IntVec2 const& defaultValue ) const
{
	std::map<std::string, std::string>::const_iterator iter = m_keyValuePairs.find( keyName );
	if ( iter == m_keyValuePairs.end() )
	{
		return defaultValue;
	}
	
	std::string value = iter->second;
	IntVec2 intVec2Value;
	intVec2Value.SetFromText( value.c_str() );
	return intVec2Value;
}