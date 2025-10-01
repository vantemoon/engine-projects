#pragma once


//-----------------------------------------------------------------------------------------------
// Forward type declarations
struct AABB2;
struct Vec2;
struct Vec3;

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

//-----------------------------------------------------------------------------------------------
// Distance and projection utilities
float GetDistance2D( Vec2 const& positionA, Vec2 const& positionB );
float GetDistanceSquared2D( Vec2 const& positionA, Vec2 const& positionB );
float GetDistance3D( Vec3 const& positionA, Vec3 const& positionB );
float GetDistanceSquared3D( Vec3 const& positionA, Vec3 const& positionB );
float GetDistanceXY3D( Vec3 const& positionA, Vec3 const& positionB );
float GetDistanceXYSquared3D( Vec3 const& positionA, Vec3 const& positionB );
// float GetTaxicabDistance2D( Vec2 const& pointA, Vec2 const& pointB );
float GetProjectedLength2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto );
Vec2 const GetProjectedVector2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto );

//-----------------------------------------------------------------------------------------------
// Geometric queries
bool DoDiscsOverlap( Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB );
bool DoSpheresOverlap( Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB );
bool IsPointInsideDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius );
// bool IsPointInsideOrientatedSector2D( Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius );
// bool IsPointInsideDirectedSector2D( Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius );
Vec2 GetNearestPointOnDisc2D( Vec2 const& referencePos, Vec2 const& discCenter, float discRadius );
bool PushDiscOutOfFixedPoint2D( Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedPoint );
bool PushDiscOutOfFixedDisc2D( Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius );
bool PushDiscsOutOfEachOther2D( Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius );
bool PushDiscOutOfFixedAABB2D( Vec2& mobileDiscCenter, float mobileDiscRadius, AABB2 const& fixedBox );

//-----------------------------------------------------------------------------------------------
// Transform utilities
void TransformPosition2D( Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation );
void TransformPositionXY3D( Vec3& posToTransform, float uniformScale, float zRotationDegrees, Vec2 const& xyTranslation );
// void TransformPosition2D( Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation );
// void TransformPositionXY3D( Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& xyTranslation );