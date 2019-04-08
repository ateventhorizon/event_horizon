#pragma once

#include <core/uuid.hpp>
#include <core/soa_utils.h>
#include <graphics/graphic_constants.h>
#include <graphics/render_material.hpp>

class Renderer;
class Matrix4f;
class GPUVData;

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

    std::unique_ptr<int32_t[]> bufferIndices;
    std::unique_ptr<char[]> bufferVerts;
    int numVerts;
    int numIndices;
    Primitive primiteType;
    int elenentSize;

    cpuVertexDescriptor vElementAttrib[9];
    int vElementAttribSize;
};

class VPList {
public:
    VPList( std::shared_ptr<GPUVData> value,
            std::shared_ptr<RenderMaterial> _mat,
            std::shared_ptr<Matrix4f> _transform,
            uint64_t _tag );

    inline std::shared_ptr<RenderMaterial> getMaterial() const {
        return material;
    }

    inline void setMaterial( std::shared_ptr<RenderMaterial> mp );

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
    void setMaterialColorWithUUID( const Color4f& _color, const UUID& _uuid, Color4f& _oldColor );

    std::shared_ptr<Matrix4f> getTransform() const;

    void setTransform( std::shared_ptr<Matrix4f> lTransform );

    bool hasTag( uint64_t _tag) const;
    uint64_t tag() const { return mTag; }
    void tag( const uint64_t tag ) { mTag = tag; }

    void draw();
    void drawWith( std::shared_ptr<RenderMaterial> _material );

private:
    std::shared_ptr<GPUVData> gpuData;
    std::shared_ptr<RenderMaterial> material;
    uint64_t mTag = GT_Generic;
    std::shared_ptr<Matrix4f> mTransform;
};
