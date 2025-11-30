#pragma once
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class TileHeatMap
{
public:
	IntVec2 m_dimensions = IntVec2::ZERO;
	float* m_values;

public:
	TileHeatMap( IntVec2 const& dimensions );
	~TileHeatMap();

	void	InitializeValues();
	void	SetAllValues( float value );
	float	GetValueAtTileCoords( IntVec2 const& tileCoords ) const;
	void	SetValueAtTileCoords( IntVec2 const& tileCoords, float value );
	void	AddValueAtTileCoords( IntVec2 const& tileCoords, float valueToAdd );

	void	AddVertsForDebugDraw( std::vector<Vertex>& verts, AABB2 totalBounds, FloatRange valueRange=FloatRange::ZERO_TO_ONE,
								  Rgba8 lowColor=Rgba8::BLACK, Rgba8 highColor=Rgba8::WHITE, float specialValue=999999.f, Rgba8 specialColor=Rgba8::MAGENTA );
};