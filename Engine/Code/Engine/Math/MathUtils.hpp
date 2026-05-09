#pragma once
#include "Engine/Math/Vec2.hpp"


//-----------------------------------------------------------------------------------------------
// Forward type declarations
struct AABB2;
struct AABB3;
struct FloatRange;
struct OBB2;
struct OBB3;
struct Plane2;
struct Plane3;
struct IntVec2;
struct Mat44;
struct Vec3;
struct Vec4;


//-----------------------------------------------------------------------------------------------
enum class BillboardType
{
	NONE = -1,
	WORLD_UP_FACING,
	WORLD_UP_OPPOSING,
	FULL_FACING,
	FULL_OPPOSING,
	COUNT
};


//-----------------------------------------------------------------------------------------------
// Clamp and lerp
float GetClamped( float value, float minValue, float maxValue );
float GetClampedZeroToOne( float value );
float Interpolate( float start, float end, float fractionTowardEnd );
float GetFractionWithinRange( float value, float rangeStart, float rangeEnd );
float RangeMap( float inValue, float inStart, float inEnd, float outStart, float outEnd );
float RangeMapClamped( float inValue, float inStart, float inEnd, float outStart, float outEnd );
float RoundDownToInt( float value );

//-----------------------------------------------------------------------------------------------
// Angle utilities
float ConvertDegreesToRadians( float degrees );
float ConvertRadiansToDegrees( float radians );
float CosDegrees( float degrees );
float SinDegrees( float degrees );
float Atan2Degrees( float y, float x );
float GetShortestAngularDispDegrees( float startDegrees, float endDegrees );
float GetTurnedTowardDegrees( float currentDegrees, float goalDegrees, float maxDeltaDegrees );
float GetAngleDegreesBetweenVectors2D( Vec2 const& a, Vec2 const& b );

//-----------------------------------------------------------------------------------------------
// Dot and cross products
float DotProduct2D( Vec2 const& a, Vec2 const& b );
float DotProduct3D( Vec3 const& a, Vec3 const& b );
float DotProduct4D( Vec4 const& a, Vec4 const& b );
float CrossProduct2D( Vec2 const& a, Vec2 const& b );
Vec3 CrossProduct3D( Vec3 const& a, Vec3 const& b );

//-----------------------------------------------------------------------------------------------
// Distance and projection utilities
float GetDistance2D( Vec2 const& positionA, Vec2 const& positionB );
float GetDistanceSquared2D( Vec2 const& positionA, Vec2 const& positionB );
float GetDistance3D( Vec3 const& positionA, Vec3 const& positionB );
float GetDistanceSquared3D( Vec3 const& positionA, Vec3 const& positionB );
float GetDistanceXY3D( Vec3 const& positionA, Vec3 const& positionB );
float GetDistanceXYSquared3D( Vec3 const& positionA, Vec3 const& positionB );
int GetTaxicabDistance2D( IntVec2 const& pointA, IntVec2 const& pointB );
float GetProjectedLength2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto );
Vec2 const GetProjectedVector2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto );

//-----------------------------------------------------------------------------------------------
// Geometric queries
bool DoDiscsOverlap( Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB );
bool DoSpheresOverlap( Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB );
bool DoAABBsOverlap( AABB2 const& alignedBoxA, AABB2 const& alignedBoxB );
bool DoAABBsOverlap3D( AABB3 const& first, AABB3 const& second );
bool DoSpheresOverlap3D( Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB );
bool DoZCylindersOverlap3D( Vec2 cylinder1CenterXY, float cylinder1Radius, FloatRange cylinder1MinZMaxZ, Vec2 cylinder2CenterXY, float cylinder2Radius, FloatRange cylinder2MinZMaxZ );
bool DoSphereAndAABBOverlap3D( Vec3 const& sphereCenter, float sphereRadius, AABB3 const& box);
bool DoZCylinderAndAABBOverlap3D( Vec2 cylinderCenterXY, float cylinderRadius, FloatRange cylinderMinZMaxZ, AABB3 const& box );
bool DoZCylinderAndSphereOverlap3D( Vec2 cylinderCenterXY, float cylinderRadius, FloatRange cylinderMinZMaxZ, Vec3 const& sphereCenter, float sphereRadius );
bool DoOBBAndSphereOverlap3D( OBB3 const& box, Vec3 const& sphereCenter, float sphereRadius );
bool DoOBBAndPlaneOverlap3D( OBB3 const& box, Plane3 const& plane );
bool DoPlaneAndAABBOverlap3D( Plane3 const& plane, AABB3 const& box );
bool DoPlaneAndSphereOverlap3D( Plane3 const& plane, Vec3 const& sphereCenter, float sphereRadius );
bool PushDiscOutOfFixedPoint2D( Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedPoint );
bool PushDiscOutOfFixedDisc2D( Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius );
bool PushDiscsOutOfEachOther2D( Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius );
bool PushDiscOutOfFixedAABB2D( Vec2& mobileDiscCenter, float mobileDiscRadius, AABB2 const& fixedBox );
bool PushDiscOutOfFixedCapsule2D( Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& capsuleBoneStart, Vec2 const& capsuleBoneEnd, float capsuleRadius );
bool PushDiscOutOfFixedOBB2D( Vec2& mobileDiscCenter, float mobileDiscRadius, OBB2 const& fixedBox );

//-----------------------------------------------------------------------------------------------
// Transform utilities
void TransformPosition2D( Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation );
void TransformPositionXY3D( Vec3& posToTransform, float uniformScale, float zRotationDegrees, Vec2 const& xyTranslation );
void TransformPosition2D( Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation );
void TransformPositionXY3D( Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& xyTranslation );

//-----------------------------------------------------------------------------------------------
// Is point inside shape queries
bool IsPointInsideDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius );
bool IsPointInsideOrientedSector2D( Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius );
bool IsPointInsideDirectedSector2D( Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius );
bool IsPointInsideAABB2D( Vec2 const& point, AABB2 const& alignedBox );
bool IsPointInsideOBB2D( Vec2 const& point, OBB2 const& orientedBox );
bool IsPointInsideCapsule2D( Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius );
bool IsPointInsideTriangle2D( Vec2 const& point, Vec2 const& ccw0, Vec2 const& ccw1, Vec2 const& ccw2 );
bool IsPointInsideCylinderZ3D( Vec3 const& point, Vec2 const& cylinderBaseCenter, float cylinderMinZ, float cylinderMaxZ, float cylinderRadius );
bool IsPointInsideOBB3D( Vec3 const& point, OBB3 const& orientedBox );
bool IsPointInFrontOfPlane3D( Vec3 const& point, Plane3 const& plane );

//-----------------------------------------------------------------------------------------------
// Nearest point calculations
Vec2 GetNearestPointOnDisc2D( Vec2 const& referencePos, Vec2 const& discCenter, float discRadius );
Vec2 GetNearestPointOnAABB2D( Vec2 const& referencePos, AABB2 const& alignedBox );
Vec2 GetNearestPointOnOBB2D( Vec2 const& referencePos, OBB2 const& orientedBox );
Vec2 GetNearestPointOnInfiniteLine2D( Vec2 const& referencePos, Vec2 const& pointOnLine, Vec2 const& anotherPointOnLine );
Vec2 GetNearestPointOnLineSegment2D( Vec2 const& referencePos, Vec2 const& start, Vec2 const& end );
Vec2 GetNearestPointOnCapsule2D( Vec2 const& referencePos, Vec2 const& boneStart, Vec2 const& boneEnd, float radius );
Vec2 GetNearestPointOnTriangle2D( Vec2 const& referencePos, Vec2 const& ccw0, Vec2 const& ccw1, Vec2 const& ccw2 );
Vec2 GetNearestPointOnPlane2D( Vec2 const& referencePos, Plane2 const& plane );
Vec3 GetNearestPointOnAABB3D( Vec3 const& referencePos, Vec3 const& mins, Vec3 const& maxs );
Vec3 GetNearestPointOnSphere3D( Vec3 const& referencePos, Vec3 const& sphereCenter, float sphereRadius );
Vec3 GetNearestPointOnCylinderZ3D( Vec3 const& referencePos, Vec2 const& cylinderBaseCenter, float cylinderMinZ, float cylinderMaxZ, float cylinderRadius );
Vec3 GetNearestPointOnOBB3D( Vec3 const& referencePos, OBB3 const& orientedBox );
Vec3 GetNearestPointOnPlane3D( Vec3 const& referencePos, Plane3 const& plane );

//-----------------------------------------------------------------------------------------------
// Normalized byte conversions
float NormalizeByte( unsigned char byteValue );
unsigned char DenormalizeByte( float zeroToOne);


//-----------------------------------------------------------------------------------------------
// Billboard utilities
Mat44 GetBillboardTransform( BillboardType type, Mat44 const& targetTransform, Vec3 const& billboardPosition, Vec2 billboardScale = Vec2::ONE );


//-----------------------------------------------------------------------------------------------
// 1D Bezier curve utilities
float ComputeCubicBezier1D( float A, float B, float C, float D, float t );
float ComputeQuinticBezier1D( float A, float B, float C, float D, float E, float F, float t );