#pragma once

#include <core/uuid.hpp>
#include <core/uuidcopyable.hpp>
#include <core/soa_utils.h>
#include <core/math/anim.h>
#include <graphics/graphic_constants.h>
#include <graphics/render_material.hpp>

class Renderer;
class Matrix4f;
class GPUVData;

namespace JMATH { class AABB; }

struct cpuVertexDescriptor {
    int size = 0;
    uintptr_t offset = 0;
};

struct cpuVBIB {
    template<class V>
    explicit cpuVBIB( std::shared_ptr<VertexStripIBVB<V>> vbib ) {
        elenentSize = sizeof( V );
        bufferVerts = std::make_unique<char[]>( elenentSize * vbib->numVerts );
        std::memcpy( bufferVerts.get(), vbib->verts.get(), elenentSize * vbib->numVerts );
        bufferIndices = std::move( vbib->indices );
        numIndices = vbib->numIndices;
        numVerts = vbib->numVerts;
        primiteType = vbib->primiteType;
        vElementAttribSize = V::numElements();
        for ( int32_t t = 0; t < V::numElements(); t++ ) {
            vElementAttrib[t].offset = V::offset( t );
            vElementAttrib[t].size = V::size( t );
        }
    }

    std::unique_ptr<uint32_t[]> bufferIndices;
    std::unique_ptr<char[]> bufferVerts;
    int numVerts;
    int numIndices;
    Primitive primiteType;
    int elenentSize;

    cpuVertexDescriptor vElementAttrib[9];
    int vElementAttribSize;
};

class VPList : public UUIDCopiable {
public:
    using UUIDCopiable::UUIDCopiable;

    VPList( std::shared_ptr<GPUVData> value,
            std::shared_ptr<RenderMaterial> _mat,
            const std::shared_ptr<Matrix4f>& _transform,
            const std::shared_ptr<AABB>& _bbox3d,
            uint64_t _tag,
            const UUID& _uuid );

    inline std::shared_ptr<RenderMaterial> getMaterial() const {
        return material;
    }

    void setMaterial( std::shared_ptr<RenderMaterial> mp );

    template<typename T>
    inline void setMaterialConstant( const std::string& name, const T& val ) {
        material->setConstant( name, val );
    }

    template<typename T>
    inline void setMaterialGlobalConstant( const std::string& name, const T& val ) {
        material->setGlobalConstant( name, val );
    }

    template<typename T>
    inline void setMaterialBufferConstant( const std::string& ubo_name, const std::string& name, const T& val ) {
        material->setBufferConstant( ubo_name, name, val );
    }

    template<typename T>
    inline T getMaterialConstant( const std::string& name, T& value ) {
        material->Uniforms()->Values()->get( name, value );
        return value;
    }

    inline float transparencyValue() const {
        return material->TransparencyValue();
    }

    void setMaterialConstantAlpha( float alpha );

    void setMaterialConstantOpacity( float alpha );

    void setMaterialWithTag( std::shared_ptr<RenderMaterial> mp, uint64_t _tag );
    void setMaterialColorWithTag( const Color4f& _color, uint64_t _tag );
    void setMaterialAlphaWithTag( float _alpha, uint64_t _tag );
    void setMaterialColorWithUUID( const Color4f& _color, const UUID& _uuid, Color4f& _oldColor );

    std::shared_ptr<Matrix4f> getTransform() const;

    void setTransform( std::shared_ptr<Matrix4f> lTransform );

    void updateP3V3( const std::vector<V3f>& _values );
    void updateGPUVData( cpuVBIB&& _vbib );
    void remapUVs( uint32_t *indices, const std::vector<V3f>& _pos, const std::vector<V2f>& _uvs, uint64_t _index, uint64_t _xrefStart );

    const AABB& BBox3d() const;

    bool hasTag( uint64_t _tag) const;
    uint64_t tag() const { return mTag; }
    void tag( const uint64_t tag ) { mTag = tag; }

    void draw( Program* _program = nullptr );
    void drawWith( RenderMaterial* _material, Program* _program = nullptr );
    void drawWithProgram( Program* _program );

    [[nodiscard]] bool isHidden() const;

    void setHidden( bool _bHidden );

private:
    std::shared_ptr<GPUVData>           gpuData;
    std::shared_ptr<RenderMaterial>     material;
    std::shared_ptr<Matrix4f>           mTransform;
    std::shared_ptr<AABB>               bbox3d;
    uint64_t                            mTag = GT_Generic;
    bool                                bHidden = false;
};

template <typename M>
void parseFader( std::vector<std::shared_ptr<VPList>>& vp, const M& _param ) {

    if constexpr ( std::is_same_v<M, std::shared_ptr<VPList>> ) {
        if ( _param ) vp.emplace_back( _param );
    }
    if constexpr ( std::is_same_v<M, std::vector<std::shared_ptr<VPList>>> ) {
        std::copy (_param.begin(), _param.end(), std::back_inserter(vp));
    }
}

template<typename ...Args>
void fader( float _duration, float _value, Args&& ...args ) {

    std::vector<std::shared_ptr<VPList>> vplists{};

    ( parseFader( vplists, std::forward<Args>(args)),... );

    Timeline::intermezzo( _duration, 0, AnimUpdateCallback([vplists, _value, _duration](float _elapsed) {
        for ( auto& vl : vplists ) {
            if ( _elapsed > 0.0f ) {
                if ( _value > 0.0f ) {
                    vl->setMaterialConstantAlpha( min( _value, (_elapsed / _duration) ) );
                } else {
                    vl->setMaterialConstantAlpha( max( _value, (1.0f-_value)-(_elapsed / _duration) ) );
                }
            }
        }
    }) );
}