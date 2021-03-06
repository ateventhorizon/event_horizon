//
// Created by Dado on 2019-01-31.
//

#include <graphics/renderer.h>
#include <graphics/vertex_processing.h>
#include <graphics/shader_material.hpp>
#include <core/descriptors/uniform_names.h>

namespace JMATH { class AABB; }

template <typename V>
class VPBuilder {
public:
    VPBuilder( Renderer& _rr,
               ShaderMaterial _sm,
               const std::string& _name ) : rr(_rr), shaderMaterial( std::move( _sm )) {
        uuid = UUIDGen::make();
        name = name.empty() ? uuid : _name;
    };

    VPBuilder( Renderer& _rr,
               CResourceRef _matRef,
               CResourceRef _vDataRef,
               CResourceRef _programRef) : rr(_rr) {
        uuid = UUIDGen::make();
        name = _matRef + _vDataRef;
        renderMaterialSP = rr.getMaterial( _matRef );
        if ( renderMaterialSP->BoundProgram()->getId() != _programRef ) {
            renderMaterialSP->BoundProgram( _rr.P( _programRef ) );
        }
        gpuDataSP = rr.getGPUVData(_vDataRef );
        bStraightRef = true;
    };

    VPBuilder& p( std::shared_ptr<V> _ps ) { ps = _ps; return *this; }
    VPBuilder& n( const std::string& _name ) {
        if ( !_name.empty() ) name = _name;
        return *this;
    }
    VPBuilder& u( const UUID& _uuid) { uuid = _uuid; return *this; }
    VPBuilder& g( const uint64_t _tag) { tag = _tag; return *this; }
    VPBuilder& t( const std::shared_ptr<Matrix4f>& _t ) { transformMatrix = _t; return *this; }
    VPBuilder& t( const Matrix4f& _t ) { transformMatrix = std::make_shared<Matrix4f>(_t); return *this; }
    VPBuilder& b( const AABB* _value ) { bbox3d = _value; return *this; }

    auto build() {
        if ( bStraightRef ) {
            return std::make_shared<VPList>(
                    gpuDataSP,
                    renderMaterialSP,
                    transformMatrix,
                    bbox3d,
                    name,
                    tag,
                    uuid );
        }
        return std::make_shared<VPList>(
               rr.addVDataResource( cpuVBIB{ ps }, name ),
               rr.addMaterialResource( shaderMaterial, name ),
               transformMatrix,
               bbox3d,
               name,
               tag,
               uuid );
    }

    auto buildUpsert() {
        return std::make_shared<VPList>(
                rr.upsertVDataResource( cpuVBIB{ ps }, name ),
                rr.upsertMaterialResource( shaderMaterial, name ),
                transformMatrix,
                bbox3d,
                name,
                tag,
                uuid );
    }

private:
    Renderer& rr;
    uint64_t tag = GT_Generic;
    std::shared_ptr<V> ps;
    ShaderMaterial shaderMaterial;

    std::shared_ptr<GPUVData> gpuDataSP;
    std::shared_ptr<RenderMaterial> renderMaterialSP;
    std::shared_ptr<Matrix4f> transformMatrix;
    const AABB* bbox3d = nullptr;
    UUID uuid;
    std::string name;
    bool bStraightRef = false;
};
