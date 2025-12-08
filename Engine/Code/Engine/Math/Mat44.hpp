#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"


//-----------------------------------------------------------------------------------------------
struct Mat44
{
	enum { Ix, Iy, Iz, Iw, Jx, Jy, Jz, Jw, Kx, Ky, Kz, Kw, Tx, Ty, Tz, Tw };
	float m_values[16];

	Mat44();
	explicit Mat44( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY );
	explicit Mat44( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D );
	explicit Mat44( Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D );
	explicit Mat44( float const* sixteenValuesBasisMajor );

	static Mat44 const MakeTranslation2D( Vec2 const& translation2D );
	static Mat44 const MakeTranslation3D( Vec3 const& translation3D );
	static Mat44 const MakeUniformScale2D( float uniformScaleXY );
	static Mat44 const MakeUniformScale3D( float uniformScaleXYZ );
	static Mat44 const MakeNonUniformScale2D( Vec2 const& nonUniformScaleXY );
	static Mat44 const MakeNonUniformScale3D( Vec3 const& nonUniformScaleXYZ );
	static Mat44 const MakeZRotationDegrees( float rotationDegreesAboutZ );
	static Mat44 const MakeYRotationDegrees( float rotationDegreesAboutY );
	static Mat44 const MakeXRotationDegrees( float rotationDegreesAboutX );

	Vec2 const TransformVectorQuantity2D( Vec2 const& vectorQuantityXY ) const;  // Assumes z = 0, w = 0
	Vec3 const TransformVectorQuantity3D( Vec3 const& vectorQuantityXYZ ) const; // Assumes w = 0
	Vec2 const TransformPosition2D( Vec2 const& positionXY ) const;              // Assumes z = 0, w = 1
	Vec3 const TransformPosition3D( Vec3 const& positionXYZ ) const;             // Assumes w = 1
	Vec4 const TransformHomogeneous3D( Vec4 const& homogeneousPoint3D ) const;   // w is provided

	float* GetAsFloatArray();
	float const* GetAsFloatArray() const;
	Vec2 const GetIBasis2D() const;
	Vec2 const GetJBasis2D() const;
	Vec2 const GetTranslation2D() const;
	Vec3 const GetIBasis3D() const;
	Vec3 const GetJBasis3D() const;
	Vec3 const GetKBasis3D() const;
	Vec3 const GetTranslation3D() const;
	Vec4 const GetIBasis4D() const;
	Vec4 const GetJBasis4D() const;
	Vec4 const GetKBasis4D() const;
	Vec4 const GetTranslation4D() const;

	void SetTranslation2D( Vec2 const& translationXY );  // Set translationZ = 0, translationW = 1
	void SetTranslation3D( Vec3 const& translation3D );  // Set translationW = 1
	void SetIJ2D( Vec2 const& iBasis2D, Vec2 const& jBasis2D ); // Set z = 0, w = 0 for i and j; does not modify k or t
	void SetIJT2D( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY ); // Set z = 0, w = 0 for i and j; w = 1 for t; does not modify k basis
	void SetIJK3D( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D );      // Set w = 0 for i, j, k; does not modify t
	void SetIJKT3D( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D ); // Set w = 0 for i, j, k; w = 1 for t
	void SetIJKT4D( Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D ); // All 16 values provided

	void Append( Mat44 const& appendThis );							// Multiply on the right in column notation / on the left in row notation
	void AppendZRotation( float rotationDegreesAboutZ );			// Same as appending (*= in column notation) a z-rotation matrix
	void AppendYRotation( float rotationDegreesAboutY );			// Same as appending (*= in column notation) a y-rotation matrix
	void AppendXRotation( float rotationDegreesAboutX );			// Same as appending (*= in column notation) an x-rotation matrix
	void AppendTranslation2D( Vec2 const& translationXY );			// Same as appending (*= in column notation) a 2D translation matrix
	void AppendTranslation3D( Vec3 const& translation3D );			// Same as appending (*= in column notation) a 3D translation matrix
	void AppendScaleUniform2D( float uniformScaleXY );				// k and t bases should remain unaffected
	void AppendScaleUniform3D( float uniformScaleXYZ );				// translation should remain unaffected
	void AppendScaleNonUniform2D( Vec2 const& nonUniformScaleXY );	// k and t bases should remain unaffected
	void AppendScaleNonUniform3D( Vec3 const& nonUniformScaleXYZ ); // translation should remain unaffected
};