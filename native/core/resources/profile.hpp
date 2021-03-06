//
//  profile.hpp
//  6thViewImporter
//
//  Created by Dado on 13/10/2015.
//
//

#pragma once

#include <utility>
#include <vector>
#include <array>
#include <core/htypes_shared.hpp>
#include <core/name_policy.hpp>
#include <core/math/vector3f.h>
#include <core/serialization.hpp>

namespace JMATH { class Rect2f; }

class Profile : public NamePolicy<> {
public:
    Profile() = default;
    virtual ~Profile() = default;
    RESOURCE_CTORS_ONLY(Profile);
    JSONSERIALONLY( Profile, mBBox, mPoints, mLengths, mPerimeter, mNormal);
    Profile( const Vector2f& a, const Vector2f& b, WindingOrderT wo = WindingOrder::CCW );

    void createWire( float radius, int numSubDivs );
	void createLine( const Vector2f& a, const Vector2f& b, WindingOrderT wo = WindingOrder::CCW );
	void createRect( const Vector2f& size, WindingOrderT wo = WindingOrder::CCW );
	void createRect( const JMATH::Rect2f& _rect );
	void createArc( float startAngle, float angle, float radius, float numSegments = 10 );
	void createArbitrary(const std::vector<Vector2f>& points );
	void raise( const Vector2f& v );

	void mirror( const Vector2f& axis );
	void flip( const Vector2f& axis );
	void move( const Vector2f& pos );
	void centered();
	[[nodiscard]] std::vector<Vector3f> rotatePoints( const Vector3f& nx, const Vector3f& ny,
										const Vector3f& offset = V3fc::ZERO ) const;

	[[nodiscard]] inline int32_t numVerts() const { return static_cast<int32_t>( mPoints.v.size() ); }
	[[nodiscard]] inline float width() const { return mBBox.x(); }
	[[nodiscard]] inline float height() const { return mBBox.y(); }

	[[nodiscard]] inline Vector2f pointAt( uint64_t index ) const { return mPoints.v[index]; }
	inline float lengthAt( uint64_t index ) { return mLengths[index]; }

	[[nodiscard]] V2fVector Points() const { return mPoints.v; }
    [[maybe_unused]] [[nodiscard, maybe_unused]] V3fVector Points3d( const Vector3f& mainAxis ) const;
	void Points( const V2fVector& val ) { mPoints.v = val; }

	[[nodiscard]] std::vector<float> Lengths() const { return mLengths; }
	void Lengths( std::vector<float> val ) { mLengths = std::move(val); }

	[[nodiscard]] Vector3f Normal() const { return mNormal; }
	void Normal( Vector3f val ) { mNormal = val; }

	[[nodiscard]] const V2fVectorOfVectorWrap& Paths() const { return mPaths; }
    [[maybe_unused]] [[nodiscard, maybe_unused]] V3fVectorOfVectorWrap Paths3d() const;
    [[nodiscard]] VTMVectorOfVectorWrap Paths3dWithUV() const;

    static std::shared_ptr<Profile> makeLine(const std::string& _name, const std::vector<Vector2f>& vv2fs, const std::vector<float>& vfs);
    static std::shared_ptr<Profile> makeWire( float radius, int numSubDivs = 0 );
    static std::shared_ptr<Profile> makeRect( const V2f& sizes, const V2f& offset = V2fc::ZERO );

    static std::shared_ptr<Profile> fromPoints( const std::string& name, const std::vector<Vector2f>& points );

private:
	void calculatePerimeter();
	void calcBBox();

private:
	V2f		        		mBBox = V2fc::ZERO;
	V2fVectorWrap           mPoints;
	std::vector<float>		mLengths;
	float					mPerimeter = 0.0f;
	V3f	        			mNormal = V3fc::X_AXIS;
	V2fVectorOfVectorWrap   mPaths;
};

class ProfileMaker {
public:
	ProfileMaker() = default;
	explicit ProfileMaker( std::string name ) : name( std::move( name )) {}

	ProfileMaker& o();
	ProfileMaker& s( float _s );
	ProfileMaker& sd( uint32_t _sd );
	ProfileMaker& ay( float radius, int32_t subdivs = -1 );
	ProfileMaker& ax( float radius, int32_t subdivs = -1 );
	ProfileMaker& axHalfLeft( float radius, int32_t subdivs = -1 );
	ProfileMaker& axHalfRight( float radius, int32_t subdivs = -1 );
	ProfileMaker& l( const V2f& _p1 );
	ProfileMaker& ly( float _y1 );
	ProfileMaker& lx( float _x1 );

	[[nodiscard]] std::shared_ptr<Profile> make() const { return Profile::fromPoints( name, points ); }

private:
	void add( const V2f& _p );
	[[nodiscard]] int setPointerSubdivs( int _sd ) const;
	[[nodiscard]] V2f pointer() const { return points.empty() ? V2fc::ZERO : points.back(); }

private:
	std::string name;
	std::vector<V2f> points;
	int32_t gSubDivs = 4;
	float scale = 1.0f;
};
