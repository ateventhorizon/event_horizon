//
// Created by Dado on 08/02/2018.
//

#include "scene_graph.h"
#include "core/node.hpp"

void SceneGraph::add( GeomAssetSP _geom ) {
    addImpl(_geom);
    geoms[_geom->Hash()] = _geom;
}

void SceneGraph::add( UIAssetSP _geom ) {
    addImpl(_geom);
    uis[_geom->Hash()] = _geom;
}

void SceneGraph::add( const std::vector<std::shared_ptr<MaterialBuilder>> _materials ) {
    for ( const auto& m : _materials ) {
        m->makeDirect( ML() );
    }
}

void
SceneGraph::add( GeomAssetSP _geom, const std::vector<std::shared_ptr<MaterialBuilder>> _materials ) {
    for ( const auto& m : _materials ) {
        m->makeDirect( ML() );
    }
    add( _geom );
}

void SceneGraph::update() {
    for ( auto& [k,v] : geoms ) {
        v->updateAnim();
    }
}


void SceneGraph::cmdChangeMaterialTag( const std::vector<std::string>& _params ) {
    changeMaterialTagImpl( _params );
}

void SceneGraph::cmdChangeMaterialColorTag( const std::vector<std::string>& _params ) {
    changeMaterialColorTagImpl( _params );
}

void SceneGraph::cmdCreateGeometry( const std::vector<std::string>& _params ) {
    cmdCreateGeometryImpl( _params );
}

void SceneGraph::cmdLoadObject( const std::vector<std::string>& _params ) {
    cmdloadObjectImpl( _params );
}

void SceneGraph::cmdCalcLightmaps( const std::vector<std::string>& _params ) {
    cmdCalcLightmapsImpl( _params );
}

SceneGraph::SceneGraph( CommandQueue& cq, FontManager& _fm ) : fm(_fm) {
    hcs = std::make_shared<CommandScriptSceneGraph>(*this);
    cq.registerCommandScript(hcs);
    mapGeomType(0, "none");
    mapGeomType(1, "generic");
}

size_t SceneGraph::countGeoms() const {
    return geoms.size();
}

std::vector<GeomAssetSP> SceneGraph::Geoms() {
    std::vector<GeomAssetSP> ret;
    for ( auto& [k,v] : geoms ) ret.emplace_back( v );
    return ret;
}

void SceneGraph::mapGeomType( const uint64_t _value, const std::string& _key ) {
    geomTypeMap[_key] = _value;
}

uint64_t SceneGraph::getGeomType( const std::string& _key ) const {
    if ( auto ret = geomTypeMap.find(_key); ret != geomTypeMap.end() ) {
        return ret->second;
    }
    try {
        return static_cast<uint64_t>(std::stoi( _key ));
    }
    catch(...) {
        return 0;
    }
    return 0;
}

void PolySceneGraph::addImpl( [[maybe_unused]] GeomAssetSP _geom ) {
}

void PolySceneGraph::addImpl( [[maybe_unused]] UIAssetSP _geom ) {
}

void AssetManager::add( [[maybe_unused]] const std::string& _key, GeomAssetSP _h ) {
    assetsHierList[_h->Name()] = _h;
}

GeomAssetSP AssetManager::findHier( const std::string& _key ) {
    if ( auto h = assetsHierList.find(_key); h != assetsHierList.end() ) {
        return h->second;
    }
    return nullptr;
}

AssetHierContainerIt AssetManager::begin() {
    return assetsHierList.begin();
}

AssetHierContainerIt AssetManager::end() {
    return assetsHierList.end();
}

AssetHierContainerCIt AssetManager::begin() const {
    return assetsHierList.cbegin();
}

AssetHierContainerCIt AssetManager::end() const {
    return assetsHierList.cend();
}

CommandScriptSceneGraph::CommandScriptSceneGraph( SceneGraph& _hm ) {
    addCommandDefinition("change material", std::bind(&SceneGraph::cmdChangeMaterialTag, &_hm, std::placeholders::_1 ));
    addCommandDefinition("paint", std::bind(&SceneGraph::cmdChangeMaterialColorTag, &_hm, std::placeholders::_1 ));
    addCommandDefinition("build", std::bind(&SceneGraph::cmdCreateGeometry, &_hm, std::placeholders::_1));
    addCommandDefinition("load object", std::bind(&SceneGraph::cmdLoadObject, &_hm, std::placeholders::_1));
    addCommandDefinition("lightmaps", std::bind(&SceneGraph::cmdCalcLightmaps, &_hm, std::placeholders::_1));
}
