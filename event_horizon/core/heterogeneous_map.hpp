//
// Created by Dado on 2019-01-28.
//

#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <core/htypes_shared.hpp>
#include <core/math/vector4f.h>
#include <core/math/matrix4f.h>
#include <core/hashable.hpp>
#include <core/serialization.hpp>

class HeterogeneousMap : public virtual Hashable<> {
public:
    HeterogeneousMap() = default;
    virtual ~HeterogeneousMap() = default;
    HeterogeneousMap( const HeterogeneousMap& _source );
    explicit HeterogeneousMap( std::shared_ptr<HeterogeneousMap> _source );
    explicit HeterogeneousMap( std::string _type ) : mType( std::move( _type )) {}

    JSONSERIAL( HeterogeneousMap, mType, mNumUniforms, mStrings, mInts, mFloats, mV2fs, mV3fs, mV3fvs, mV4fs, mM3fs, mM4fs );

    void inject( const HeterogeneousMap& source );
    void injectIfNotPresent( const HeterogeneousMap& source );
    void assign( const std::string& uniformName, int data );
    void assign( const std::string& uniformName, float data );
    void assign( const std::string& uniformName, double data );
    void assign( const std::string& uniformName, const std::string& data );
    void assign( const std::string& uniformName, const Vector2f& data );
    void assign( const std::string& uniformName, const Vector3f& data );
    void assign( const std::string& uniformName, const Vector4f& data );
    void assign( const std::string& uniformName, const Matrix4f& data );
    void assign( const std::string& uniformName, const Matrix3f& data );
    void assign( const std::string& uniformName, const std::vector<Vector3f>& data );

    void assignIfNotPresent( const std::string& uniformName, int data );
    void assignIfNotPresent( const std::string& uniformName, float data );
    void assignIfNotPresent( const std::string& uniformName, double data );
    void assignIfNotPresent( const std::string& uniformName, const std::string& data );
    void assignIfNotPresent( const std::string& uniformName, const Vector2f& data );
    void assignIfNotPresent( const std::string& uniformName, const Vector3f& data );
    void assignIfNotPresent( const std::string& uniformName, const Vector4f& data );
    void assignIfNotPresent( const std::string& uniformName, const Matrix4f& data );
    void assignIfNotPresent( const std::string& uniformName, const Matrix3f& data );
    void assignIfNotPresent( const std::string& uniformName, const std::vector<Vector3f>& data );

    int NumUniforms() const { return mNumUniforms; }

    std::vector<std::string> getTextureNames() const;
    std::unordered_map<std::string, std::string> getTextureNameMap() const;
    std::string getTexture( const std::string& uniformName ) const;
    float getInt( const std::string & uniformName ) const;
    float getFloatWithDefault( const std::string& uniformName, float def ) const;
    float getFloat( const std::string & uniformName ) const;
    Vector2f getVector2f( const std::string & uniformName ) const;
    Vector3f getVector3f( const std::string & uniformName ) const;
    Vector4f getVector4f( const std::string & uniformName ) const;
    Matrix4f getMatrix4f( const std::string & uniformName ) const;
    Matrix3f getMatrix3f( const std::string & uniformName ) const;
    void get( const std::string& uniformName, std::string& ret ) const;
    void get( const std::string& uniformName, int& ret ) const;
    bool get( const std::string& uniformName, float& ret ) const;
    void get( const std::string& uniformName, Vector2f& ret ) const;
    void get( const std::string& uniformName, Vector3f& ret ) const;
    void get( const std::string& uniformName, Vector4f& ret ) const;
    void get( const std::string& uniformName, Matrix3f& ret ) const;
    void get( const std::string& uniformName, Matrix4f& ret ) const;

    bool hasTexture( const std::string& uniformName ) const;
    bool hasInt( const std::string& uniformName ) const;
    bool hasFloat( const std::string& uniformName ) const;
    bool hasVector2f( const std::string& uniformName ) const;
    bool hasVector3f( const std::string& uniformName ) const;
    bool hasVector4f( const std::string& uniformName ) const;
    bool hasMatrix4f( const std::string& uniformName ) const;
    bool hasMatrix3f( const std::string& uniformName ) const;
    bool hasVectorOfVector3f( const std::string& uniformName ) const;

    std::shared_ptr<HeterogeneousMap> clone();
    void clone( const HeterogeneousMap& _map );

    template <typename T>
    void visit( const T& _visitor ) {
        if ( mNumUniforms == 0 ) return;

        for ( auto& u : mStrings )  { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mFloats )   { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mInts )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mV2fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mV3fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mV4fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mM3fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mM4fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mV3fvs )    { _visitor.visit( u.first.c_str(), u.second ); }
    }

private:
    void calcTotalNumUniforms();

private:
    std::string mType;
    std::unordered_map<std::string, std::string>           mStrings;
    std::unordered_map<std::string, int>                   mInts;
    std::unordered_map<std::string, float>                 mFloats;
    std::unordered_map<std::string, Vector2f>              mV2fs;
    std::unordered_map<std::string, Vector3f>              mV3fs;
    std::unordered_map<std::string, std::vector<Vector3f>> mV3fvs;
    std::unordered_map<std::string, Vector4f>              mV4fs;
    std::unordered_map<std::string, Matrix3f>              mM3fs;
    std::unordered_map<std::string, Matrix4f>              mM4fs;
    int mNumUniforms = 0;
};
