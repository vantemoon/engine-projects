#include "Engine/Core/TileHeatMap.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
TileHeatMap::TileHeatMap( IntVec2 const& dimensions )
	: m_dimensions( dimensions )
{
	int numOfTiles = m_dimensions.x * m_dimensions.y;
	m_values = new float[numOfTiles];

	InitializeValues();
}


//-----------------------------------------------------------------------------------------------
TileHeatMap::~TileHeatMap()
{
	delete[] m_values;
	m_values = nullptr;
}


//-----------------------------------------------------------------------------------------------
void TileHeatMap::InitializeValues()
{
	int numOfTiles = m_dimensions.x * m_dimensions.y;
	for ( int tileIndex = 0; tileIndex < numOfTiles; ++ tileIndex )
	{
		m_values[tileIndex] = 0.f;
	}
}


//-----------------------------------------------------------------------------------------------
void TileHeatMap::SetAllValues( float value )
{
	int numOfTiles = m_dimensions.x * m_dimensions.y;
	for ( int tileIndex = 0; tileIndex < numOfTiles; ++ tileIndex )
	{
		m_values[tileIndex] = value;
	}
}


//-----------------------------------------------------------------------------------------------
float TileHeatMap::GetValueAtTileCoords( IntVec2 const& tileCoords ) const
{
	int tileIndex = tileCoords.y * m_dimensions.x + tileCoords.x;
	return m_values[tileIndex];
}


//-----------------------------------------------------------------------------------------------
void TileHeatMap::SetValueAtTileCoords( IntVec2 const& tileCoords, float value )
{
	int tileIndex = tileCoords.y * m_dimensions.x + tileCoords.x;
	m_values[tileIndex] = value;
}


//-----------------------------------------------------------------------------------------------
void TileHeatMap::AddValueAtTileCoords( IntVec2 const& tileCoords, float valueToAdd )
{
	int tileIndex = tileCoords.y * m_dimensions.x + tileCoords.x;
	m_values[tileIndex] += valueToAdd;
}


//-----------------------------------------------------------------------------------------------
void TileHeatMap::AddVertsForDebugDraw( std::vector<Vertex>& verts, AABB2 totalBounds, FloatRange valueRange, 
										Rgba8 lowColor, Rgba8 highColor, float specialValue, Rgba8 specialColor )
{
	float tileWidth = totalBounds.GetDimensions().x / static_cast<float>( m_dimensions.x );
	float tileHeight = totalBounds.GetDimensions().y / static_cast<float>( m_dimensions.y );
	for ( int tileY = 0; tileY < m_dimensions.y; ++ tileY )
	{
		for ( int tileX = 0; tileX < m_dimensions.x; ++ tileX )
		{
			IntVec2 tileCoords = IntVec2( tileX, tileY );
			float value = GetValueAtTileCoords( tileCoords );
			Rgba8 color;
			if ( value == specialValue )
			{
				color = specialColor;
			}
			else
			{
				float fractionOfEnd = RangeMapClamped( value, valueRange.m_min, valueRange.m_max, 0.f, 1.f );
				color = lowColor.Interpolate( lowColor, highColor, fractionOfEnd );
			}
			float minX = totalBounds.m_mins.x + static_cast<float>( tileX ) * tileWidth;
			float minY = totalBounds.m_mins.y + static_cast<float>( tileY ) * tileHeight;
			float maxX = minX + tileWidth;
			float maxY = minY + tileHeight;
			AABB2 tileAABB2( minX, minY, maxX, maxY );
			AddVertsForAABB2D( verts, tileAABB2, color );
		}
	}
}