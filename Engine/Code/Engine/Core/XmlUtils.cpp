#include "Engine/Core/XmlUtils.hpp"


//-----------------------------------------------------------------------------------------------
int ParseXmlAttribute( XmlElement const& element, char const* attributeName, int defaultValue )
{
	int value = defaultValue;
	XmlResult result = element.QueryIntAttribute( attributeName, &value );
	if ( result != tinyxml2::XML_SUCCESS )
	{
		return defaultValue;
	}
	return value;
}


//-----------------------------------------------------------------------------------------------
char ParseXmlAttribute( XmlElement const& element, char const* attributeName, char defaultValue )
{
	const char* value = element.Attribute( attributeName );
	if ( value == nullptr || value[0] == '\0' || value[1] != '\0' )
	{
		return defaultValue;
	}
	return value[0];
}


//-----------------------------------------------------------------------------------------------
bool ParseXmlAttribute( XmlElement const& element, char const* attributeName, bool defaultValue )
{
	bool value = defaultValue;
	XmlResult result = element.QueryBoolAttribute( attributeName, &value );
	if ( result != tinyxml2::XML_SUCCESS )
	{
		return defaultValue;
	}
	return value;
}


//-----------------------------------------------------------------------------------------------
float ParseXmlAttribute( XmlElement const& element, char const* attributeName, float defaultValue )
{
	float value = defaultValue;
	XmlResult result = element.QueryFloatAttribute( attributeName, &value );
	if ( result != tinyxml2::XML_SUCCESS )
	{
		return defaultValue;
	}
	return value;
}


//-----------------------------------------------------------------------------------------------
Rgba8 ParseXmlAttribute( XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue )
{
	Rgba8 result = defaultValue;
	const char* value = element.Attribute( attributeName );
	if ( value == nullptr )
	{
		return defaultValue;
	}
	result.SetFromText( value );
	return result;
}


//-----------------------------------------------------------------------------------------------
Vec2 ParseXmlAttribute( XmlElement const& element, char const* attributeName, Vec2 const& defaultValue )
{
	Vec2 result = defaultValue;
	const char* value = element.Attribute( attributeName );
	if ( value == nullptr )
	{
		return defaultValue;
	}
	result.SetFromText( value );
	return result;
}


//-----------------------------------------------------------------------------------------------
IntVec2 ParseXmlAttribute( XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue )
{
	IntVec2 result = defaultValue;
	const char* value = element.Attribute( attributeName );
	if ( value == nullptr )
	{
		return defaultValue;
	}
	result.SetFromText( value );
	return result;
}


//-----------------------------------------------------------------------------------------------
std::string ParseXmlAttribute( XmlElement const& element, char const* attributeName, std::string const& defaultValue )
{
	const char* value = element.Attribute( attributeName );
	if ( value == nullptr )
	{
		return defaultValue;
	}
	return std::string( value );
}


//-----------------------------------------------------------------------------------------------
Strings ParseXmlAttribute( XmlElement const& element, char const* attributeName, Strings const& defaultValues, char delimiter /*= ',' */ )
{
	const char* value = element.Attribute( attributeName );
	if ( value == nullptr )
	{
		return defaultValues;
	}
	return SplitStringOnDelimiter( value, delimiter );
}


//-----------------------------------------------------------------------------------------------
std::string ParseXmlAttribute( XmlElement const& element, char const* attributeName, char const* defaultValue )
{
	const char* value = element.Attribute( attributeName );
	if ( value == nullptr )
	{
		return std::string( defaultValue );
	}
	return std::string( value );
}