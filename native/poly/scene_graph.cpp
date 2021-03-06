//
// Created by Dado on 08/02/2018.
//

#include "scene_graph.h"
#include <core/node.hpp>
#include <core/raw_image.h>
#include <core/camera_rig.hpp>
#include <core/default_font.hpp>
#include <core/resources/profile.hpp>
#include <core/TTF.h>
#include <core/image_util.h>
#include <core/geom.hpp>
#include <core/camera.h>
#include <core/names.hpp>
#include <core/resources/resource_builder.hpp>
#include <core/resources/material.h>
#include <core/resources/ui_container.hpp>
#include <core/file_manager.h>
#include <poly/converters/gltf2/gltf2.h>
#include <poly/baking/xatlas_client.hpp>
#include <poly/scene_events.h>
#include <poly/collision_mesh.hpp>

GenericSceneCallback           SceneGraph::genericSceneCallback;
EventSceneCallback             SceneGraph::eventSceneCallback;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackVData;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackRawImage;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackMaterial;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackFont;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackProfile;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackMaterialColor;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackCameraRig;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackGeom;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackUI;
LoadedResouceCallbackContainer SceneGraph::resourceCallbackComposite;

namespace HOD { // HighOrderDependency

    template<>
    DepRemapsManager resolveDependencies<ResourceScene>( const ResourceScene *_resources, SceneGraph& sg ) {
        DepRemapsManager ret{};

        _resources->visit(ResourceGroup::Geom, [&]( const std::string& key, const std::string& entry ) {
            ret.addDep(sg, key, entry);
        });

        return ret;
    }
}

void SceneGraph::traverseNode( GeomSP _node, int _nodeBucket, NodeTraverseMode _ntm ) {
    if ( _ntm == NodeTraverseMode::Add ) {
        nodeAddSignal({_node, _nodeBucket});
    } else if ( _ntm == NodeTraverseMode::Upsert) {
        nodeUpdateSignal({_node, _nodeBucket});
    }
    if ( _node->isRoot()) {
        nodes.emplace(_node->UUiD(), _node);
    }
    for ( const auto& c : _node->Children()) {
        traverseNode(c, _nodeBucket, _ntm);
    }
}

void SceneGraph::traverseNode( const UUID& _uuid, int _nodeBucket, NodeTraverseMode _ntm ) {
    traverseNode(get<Geom>(_uuid), _nodeBucket, _ntm );
}

void SceneGraph::updateNodes( const FlattenGeomSP& _nodes, int _nodeBucket) {
    for ( const auto& v : _nodes ) {
        nodeUpdateSignal({v, _nodeBucket});
    }
}

void SceneGraph::removeNode( const UUID& _uuid ) {
    if ( auto it = nodes.find(_uuid); it != nodes.end()) {
        nodes.erase(it);
    }
}

void SceneGraph::removeNode( GeomSP _node ) {
    gmNodeRemoveSignal({_node, 0});
    if ( _node ) {
        for ( const auto& c : _node->Children()) {
            removeNode(c);
        }
        removeNode(_node->UUiD());
    }
}

void SceneGraph::getNodeRec( const UUID& _uuid, const GeomSP& _node, GeomSP& ret ) {
    if ( _node->UUiDCopy() != _uuid ) {
        for ( const auto& c : _node->Children()) {
            getNodeRec(_uuid, c, ret );
        }
    } else {
        ret = _node;
    }
}

GeomSP SceneGraph::getNode( const UUID& _uuid ) {
    GeomSP ret = nullptr;
    for ( const auto& [key, value] : nodes ) {
        getNodeRec( _uuid, value, ret );
    }
    return ret;
}

void SceneGraph::addSkybox( const std::string& skyName ) {
    if ( auto res = TL().getHash(skyName); !res.empty()) {
        nodeSetSkyboxSignal(res);
    }
}

std::shared_ptr<Camera> SceneGraph::DC() {
    return CM().get(Name::Foxtrot)->getCamera();
}

JSONDATA(MatGeomSerData, mrefs)

    std::unordered_map<std::string, MaterialThumbnail> mrefs;
};

JSONDATA(LoadFinishData, flag)

    bool flag = true;
};

void MaterialThumbnail::setThumbnailFor( const std::string& _textureType ) {
    auto dt = sg->get<RawImage>(getTexture(_textureType));
    constexpr int ts = 64;

    if ( dt ) {
        thumbValues[_textureType] = imageUtil::rawResizeToPng64gzip(dt, ts, ts);
//        bool isBigEnough = dt->width > ts || dt->height > ts;
//        thumbValues[_textureType] = isBigEnough ?
//                                    imageUtil::rawResizeToPng64gzip( dt, ts, ts )
//                                                : imageUtil::rawToPng64gzip( dt );
    }

}

MaterialThumbnail::MaterialThumbnail( SceneGraph *_sg, const Material& _mat ) : Material(_mat), sg(_sg) {
    setThumbnailFor(UniformNames::diffuseTexture);
    setThumbnailFor(UniformNames::normalTexture);
    setThumbnailFor(UniformNames::metallicTexture);
    setThumbnailFor(UniformNames::roughnessTexture);
    setThumbnailFor(UniformNames::aoTexture);
    setThumbnailFor(UniformNames::opacityTexture);
}

void SceneGraph::materialsForGeomSocketMessage() {
    MatGeomSerData matSet{};
    for ( const auto&[k, node] : nodes) {
        LOGRS(node)
        for ( const auto& data : node->DataV()) {
            LOGRS("Data material: " << data.material)
            auto mat = get<Material>(data.material);
            LOGRS("mat key: " << mat->Key())
            matSet.mrefs.emplace(mat->Key(), MaterialThumbnail(this, *mat));
        }
    }
    Socket::send("materialsForGeom", matSet);
}

void SceneGraph::replaceMaterialOnNodes( const std::string& _key ) {
    LOGRS("OldMaterialRef: " << signalValueMap["source_material_id"])
    for ( auto&[k, node] : nodes) {
        for ( auto& data : node->DataVRef()) {
            auto mat = get<Material>(data.material);
            if ( mat->Key() == signalValueMap["source_material_id"] ) {
                data.material = _key;
            }
        }
    }
    materialsForGeomSocketMessage();
    replaceMaterialSignal(signalValueMap["source_material_id"], _key);
}

void SceneGraph::genericCallbacks() {
    for ( auto& cb : genericSceneCallback ) {
        cb();
    }
    genericSceneCallback.clear();
}

void SceneGraph::clearGMNodes() {
    removeNode(GeomSP{});
    GM().clear();
    nodes.clear();
}

void SceneGraph::clearNodes() {
    nodes.clear();
}

void SceneGraph::resetAndLoadEntity( CResourceRef v0, const std::string& entityGroup, bool bTakeScreenShot ) {

    clearGMNodes();
    Http::clearRequestCache();
    currLoadedEntityID = v0;

    if ( entityGroup == ResourceGroup::Geom ) {
        GB<GT::Shape>(ShapeType::Cube, GT::Tag(SHADOW_MAGIC_TAG), V3fc::UP_AXIS_NEG * 0.051f,
                      GT::Scale(500.0f, 0.1f, 500.0f), C4fc::XTORGBA("e76848"));
        addGeomScene(v0, bTakeScreenShot);
    } else if ( entityGroup == ResourceGroup::Material ) {
        load<Material>(v0, [this, v0]( HttpResourceCBSign key ) {
            nodeFullScreenMaterialSignal(key);
        });
    } else if ( entityGroup == ResourceGroup::Image ) {
        load<RawImage>(v0, [this, v0]( HttpResourceCBSign key ) {
            nodeFullScreenImageSignal(key);
        });
    } else if ( entityGroup == ResourceGroup::Font ) {
        load<Font>(v0, [this, v0]( HttpResourceCBSign key ) {
            nodeFullScreenFontSonnetSignal(key);
        });
    } else if ( entityGroup == ResourceGroup::Profile ) {
        load<Profile>(v0, [this, v0]( HttpResourceCBSign key ) {
            nodeFullScreenProfileSignal(key);
        });
    } else if ( entityGroup == ResourceGroup::UI ) {
        load<UIContainer>(v0, [this, v0]( HttpResourceCBSign key ) {
            nodeFullScreenUIContainerSignal(key);
        });
    }
}

void SceneGraph::realTimeCallbacks() {
    for ( auto&[k, doc] : eventSceneCallback ) {

        if ( k == SceneEvents::ReloadLuaScript ) {
            runLUAScriptSignal(doc["data"].GetString());
        } else if ( k == SceneEvents::UpdateEntity ) {
            clearGMNodes();
            auto entityGroup = doc["data"]["group"].GetString();

            if ( entityGroup == ResourceGroup::UI ) {
                um.clear();
                auto rref = addUIIM(UUIDGen::make(), UIContainer{ doc["data"]["data"] });
                nodeFullScreenUIContainerSignal(rref);
            }
        } else if ( k == SceneEvents::AddPlaceHolderEntity ) {
            clearGMNodes();
            auto entityGroup = doc["data"]["group"].GetString();

            if ( entityGroup == ResourceGroup::UI ) {
                um.clear();
                auto rref = addUIIM(UUIDGen::make(), UIContainer::placeHolder());
                nodeFullScreenUIContainerSignal(rref);
            }
        } else if ( k == SceneEvents::LoadGeomAndReset ) {
            resetAndLoadEntity(doc["data"]["entity_id"].GetString(), doc["data"]["group"].GetString(), false);
        } else if ( k == SceneEvents::ReplaceMaterialOnCurrentObject ) {
            auto matId = getFileName(doc["data"]["mat_id"].GetString());
            auto objId = getFileName(doc["data"]["entity_id"].GetString());
            signalValueMap["source_material_id"] = std::string(doc["data"]["source_id"].GetString());
            load<Material>(matId, [&]( HttpResourceCBSign key ) {
                replaceMaterialOnNodes(key);
            });
        } else if ( k == SceneEvents::ChangeMaterialProperty ) {
            if ( doc["data"]["value_type"].GetString() == std::string("hexcolor")) {
                auto value = C4fc::XTORGBA(doc["data"]["value_str"].GetString()).xyz();
                changeMaterialPropertyV3fSignal(doc["data"]["property_id"].GetString(),
                                                doc["data"]["mat_id"].GetString(), value);
            } else if ( doc["data"]["value_type"].GetString() == std::string("float100")) {
                float value = std::stof(doc["data"]["value_str"].GetString()) / 100.0f;
                changeMaterialPropertyFloatSignal(doc["data"]["property_id"].GetString(),
                                                  doc["data"]["mat_id"].GetString(), value);
            } else if ( doc["data"]["value_type"].GetString() == std::string("string")) {
                std::string value = doc["data"]["value_str"].GetString();
                std::string pid = doc["data"]["property_id"].GetString();
                std::string matid = doc["data"]["mat_id"].GetString();
                acquire<RawImage>(value, [this, pid, matid]( HttpResourceCBSign key ) {
                    auto mat = get<Material>(matid);
                    if ( mat ) {
                        mat->Values()->assign(pid, key);
                        changeMaterialPropertyStringSignal(pid,
                                                           matid, key);
                        materialsForGeomSocketMessage();
                    }
                });
            }
        }
    }
    eventSceneCallback.clear();
}

void SceneGraph::loadCallbacks() {
    loadResourceCallback<VData, VB>(resourceCallbackVData);
    loadResourceCallback<RawImage, IB>(resourceCallbackRawImage);
    loadResourceCallback<Font, FB>(resourceCallbackFont);
    loadResourceCallback<Profile, PB>(resourceCallbackProfile);
    loadResourceCallback<UIContainer, UIB>(resourceCallbackUI);
    loadResourceCallback<MaterialColor, MCB>(resourceCallbackMaterialColor);
    loadResourceCallbackWithKey<Material, MB>(resourceCallbackMaterial);
    loadResourceCallbackWithLoader<Geom, GRB>(resourceCallbackGeom, GLTF2Service::load);
    loadResourceCompositeCallback(resourceCallbackComposite);
}

void SceneGraph::update() {

    static bool firstFrameEver = true;

    if ( firstFrameEver ) {
        firstFrameEver = false;
        return;
    }

    // NDDado, these are mainly dealt with the react portal now, might become obsolete soon
    genericCallbacks();
    realTimeCallbacks();
    loadCallbacks();

    VL().update();
    TL().update();
    PL().update();
    ML().update();
    CL().update();
    CM().update();
    FM().update();
    MC().update();
    GM().update();
    UM().update();

    for ( auto&[k, v] : nodes ) {
        v->updateAnim();
    }
}

void SceneGraph::cmdChangeMaterialTag( const std::vector<std::string>& _params ) {
//    changeMaterialTagImpl( _params );
}

void SceneGraph::cmdChangeMaterialColorTag( const std::vector<std::string>& _params ) {
//    changeMaterialColorTagImpl( _params );
}

void SceneGraph::cmdLoadObject( const std::vector<std::string>& _params ) {
//    cmdloadObjectImpl( _params );
}

void SceneGraph::cmdCalcLightmaps( const std::vector<std::string>& _params ) {
//    cmdCalcLightmapsImpl( _params );
}

void SceneGraph::cmdChangeTime( const std::vector<std::string>& _params ) {
//    cmdChangeTimeImpl( _params );
}

void SceneGraph::cmdReloadShaders( const std::vector<std::string>& _params ) {
//    cmdChangeTimeImpl( _params );
}

SceneGraph::SceneGraph( VDataManager& _vl,
                        ImageManager& _tl,
                        ProfileManager& _pl,
                        MaterialManager& _ml,
                        ColorManager& _cl,
                        FontManager& _fm,
                        CameraManager& _cm,
                        GeomManager& _gm,
                        UIManager& _um,
                        LightManager& _ll ) : vl(_vl), tl(_tl), pl(_pl), ml(_ml), cl(_cl), fm(_fm),
                                              cm(_cm), gm(_gm), um(_um), ll(_ll) {

    mapGeomType(0, "none");
    mapGeomType(1, "generic");
}

size_t SceneGraph::countGeoms() const {
    return nodes.size();
}

void SceneGraph::mapGeomType( const uint64_t _value, const std::string& _key ) {
    geomTypeMap[_key] = _value;
}

uint64_t SceneGraph::getGeomType( const std::string& _key ) const {
    if ( auto ret = geomTypeMap.find(_key); ret != geomTypeMap.end()) {
        return ret->second;
    }
    try {
        return static_cast<uint64_t>(std::stoi(_key));
    }
    catch ( ... ) {
        return 0;
    }
    //return 0;
}

bool SceneGraph::rayIntersect( const V3f& _near, const V3f& _far, SceneRayIntersectCallback _callback ) {

    bool ret = false;

    for ( const auto&[k, v] : nodes ) {
        for ( const auto& d : v->DataV() ) {
            auto box = d.BBox3d();
            float tn = 0.0f;
            float tf = std::numeric_limits<float>::max();
            auto lDir = normalize(_far - _near);
            if ( box.intersectLine(_near, lDir, tn, tf)) {
                _callback(v, tn);
                ret = true;
                break;
            }
        }
    }

    return ret;
}

void SceneGraph::init() {
    B<IB>(S::WHITE).addIM(RawImage::WHITE4x4());
    B<IB>(S::BLACK).addIM(RawImage::BLACK_ARGB4x4());
    B<IB>(S::NORMAL).addIM(RawImage::NORMAL4x4());
    B<IB>(S::NOISE4x4).addIM(RawImage::NOISE4x4());
    B<IB>(S::DEBUG_UV).addIM(RawImage::DEBUG_UV());
    B<IB>(S::LUT_3D_TEST).addIM(RawImage::LUT_3D_TEST());

    B<MB>(S::WHITE_PBR).addIM(Material{ S::SH });
    Profile squared;
    squared.createRect(Rect2f::IDENTITY_CENTERED);
    B<PB>(S::SQUARE).addIM(squared);
    B<CB>(Name::Foxtrot).addIM(CameraRig{ Name::Foxtrot });
    B<FB>(S::DEFAULT_FONT).addIM(Font{ getDefaultFont() });
}

ResourceRef SceneGraph::addVData( const ResourceRef& _key, const VData& _res, HttpResourceCB _ccf ) {
    B<VB>(_key).addDF(_res, _ccf);
    return _key;
}

ResourceRef SceneGraph::addVDataIM( const ResourceRef& _key, const VData& _res ) {
    B<VB>(_key).addIM(_res);
    return _key;
}

ResourceRef SceneGraph::addRawImage( const ResourceRef& _key, const RawImage& _res, HttpResourceCB _ccf ) {
    B<IB>(_key).addDF(_res, _ccf);
    return _key;
}

void SceneGraph::updateRawImage( const ResourceRef& _key ) {
    B<IB>(_key).updateIM( getHash<RawImage>(_key) );
}

ResourceRef SceneGraph::addMaterial( const ResourceRef& _key, const Material& _res, HttpResourceCB _ccf ) {
    B<MB>(_key).addDF(_res, _ccf);
    return _key;
}

ResourceRef SceneGraph::addRawImageIM( const ResourceRef& _key, const RawImage& _res ) {
    B<IB>(_key).addIM(_res);
    return _key;
}

ResourceRef SceneGraph::addMaterialIM( const ResourceRef& _key, const Material& _res ) {
    B<MB>(_key).addIM(_res);
    return _key;
}

ResourceRef SceneGraph::addProfileIM( const ResourceRef& _key, const Profile& _res ) {
    B<PB>(_key).addIM(_res);
    return _key;
}

ResourceRef SceneGraph::addFont( const ResourceRef& _key, const Font& _res, HttpResourceCB _ccf ) {
    B<FB>(_key).addDF(_res, _ccf);
    return _key;
}

ResourceRef SceneGraph::addProfile( const ResourceRef& _key, const Profile& _res, HttpResourceCB _ccf ) {
    B<PB>(_key).addDF(_res, _ccf);
    return _key;
}

ResourceRef SceneGraph::addMaterialColor( const ResourceRef& _key, const MaterialColor& _res, HttpResourceCB _ccf ) {
    B<MCB>(_key).addDF(_res, _ccf);
    return _key;
}

ResourceRef SceneGraph::addCameraRig( const ResourceRef& _key, const CameraRig& _res, HttpResourceCB _ccf ) {
    B<CB>(_key).addDF(_res, _ccf);
    return _key;
}

ResourceRef SceneGraph::addGeom( const ResourceRef& _key, GeomSP _res, HttpResourceCB _ccf ) {
    B<GRB>(_key).addIM(_res);
    if ( _ccf ) _ccf(_key);
    return _key;
}

ResourceRef SceneGraph::addLight( const ResourceRef& _key, const Light& _res, HttpResourceCB _ccf ) {
    B<LB>(_key).addIM(_res);
    if ( _ccf ) _ccf(_key);
    return _key;
}

ResourceRef SceneGraph::addUI( const ResourceRef& _key, const UIContainer& _res, HttpResourceCB _ccf ) {
    B<UIB>(_key).addDF(_res, _ccf);
    return _key;
}

ResourceRef SceneGraph::addUIIM( const ResourceRef& _key, const UIContainer& _res ) {
    B<UIB>(_key).addIM(_res);
    return _key;
}

void SceneGraph::addResources( CResourceRef _key, const SerializableContainer& _data, HttpResourceCB _ccf ) {

    auto fs = tarUtil::untar(_data);
    if ( fs.find(ResourceCatalog::Key) != fs.end()) {
        auto dict = deserializeArray<ResourceTarDict>(fs[ResourceCatalog::Key]);

        std::sort(dict.begin(), dict.end(), []( const auto& a, const auto& b ) -> bool {
            return resourcePriority(a.group) < resourcePriority(b.group);
        });

        for ( const auto& rd : dict ) {
            if ( rd.group == ResourceGroup::Image ) {
                B<IB>(rd.filename).make(fs[rd.filename], rd.hash);
            } else if ( rd.group == ResourceGroup::Font ) {
                B<FB>(rd.filename).make(fs[rd.filename], rd.hash);
            } else if ( rd.group == ResourceGroup::Profile ) {
                B<PB>(rd.filename).make(fs[rd.filename], rd.hash);
            } else if ( rd.group == ResourceGroup::Color ) {
                B<MCB>(rd.filename).make(fs[rd.filename], rd.hash);
            } else if ( rd.group == ResourceGroup::Material ) {
                auto mat = B<MB>(rd.filename).make(fs[rd.filename], rd.hash, _key);
                mat->Key(_key);
            } else {
                LOGRS("{" << rd.group << "} Resource not supported yet in dependency unpacking");
                ASSERT(0);
            }
        }
    } else {
        for ( const auto& entry : fs ) {
            if ( isFileExtAnImage(getFileNameExt(entry.first)) ) {
                B<IB>(entry.first).make(entry.second, Hashable<>::hashOf(entry.first), entry.first);
            }
        }
        for ( const auto& entry : fs ) {
            if ( getFileNameExt(entry.first) == ".mat" ) {
                auto mat = B<MB>(entry.first).make(entry.second, Hashable<>::hashOf(entry.first), _key);
                mat->Key(_key);
            }
        }
    }

    if ( _ccf ) _ccf(_key);
}

std::tuple<ResourceRef, Material*> SceneGraph::GBMatInternal( CResourceRef _matref, const C4f& _color ) {
    auto matRef = ML().getHashAndPointer(_matref);
    if ( std::get<0>(matRef).empty()) {
        matRef = ML().getHashAndPointer(S::WHITE_PBR);
    }
    if ( _color != C4fc::WHITE ) {
        matRef = ML().getHashAndPointer(_matref + _color.toString());
        if ( std::get<0>(matRef).empty()) {
            Material matCopy = EF::copy(ML().get(_matref));
            matCopy.setDiffuseColor(_color.xyz());
            matCopy.setOpacity(_color.w());
            auto mRef = B<MB>(_matref + _color.toString()).addIM(matCopy);
            matRef = ML().getHashAndPointer(mRef);
        }
    }
    return matRef;
}

void SceneGraph::loadVData( std::string _names, HttpResourceCB _ccf ) {
    B<VB>(_names).load(_ccf);
}

void SceneGraph::loadRawImage( std::string _names, HttpResourceCB _ccf ) {
    B<IB>(_names).load(_ccf);
}

void SceneGraph::loadMaterial( std::string _names, HttpResourceCB _ccf ) {
    if ( auto ret = ML().get(_names); ret != nullptr ) {
        _ccf(ret->Key());
        return;
    }
    B<MB>(_names).load(_ccf);
}

void SceneGraph::loadFont( std::string _names, HttpResourceCB _ccf ) {
    B<FB>(_names).load(_ccf);
}

void SceneGraph::loadProfile( std::string _names, HttpResourceCB _ccf ) {
    B<PB>(_names).load(_ccf);
}

void SceneGraph::loadMaterialColor( std::string _names, HttpResourceCB _ccf ) {
    B<MCB>(_names).load(_ccf);
}

void SceneGraph::loadCameraRig( std::string _names, HttpResourceCB _ccf ) {
    B<CB>(_names).load(_ccf);
}

void SceneGraph::loadUI( std::string _names, HttpResourceCB _ccf ) {
    B<UIB>(_names).load(_ccf);
}

void SceneGraph::loadGeom( std::string _names, HttpResourceCB _ccf ) {
    B<GRB>(_names).load(_ccf);
}

void SceneGraph::loadAsset( const std::string& _names ) {
    B<GRB>(_names).load([this]( HttpResourceCBSign key ) {
        GB<GT::Asset>(key);
    });
}

void SceneGraph::loadScene( const ResourceScene& gs, HODResolverCallback _ccf ) {
    HOD::resolver<ResourceScene>(*this, &gs, _ccf);
}

bool SceneGraph::nodeExists( const std::string& _name ) const {
    for ( const auto& it : nodes ) {
        if ( it.second->Name() == _name ) {
            return true;
        }
    }
    return false;
}

void SceneGraph::addScene( const ResourceScene& gs, bool bTakeScreenShot ) {
    HOD::resolver<ResourceScene>(*this, &gs, [this, gs, bTakeScreenShot]() {
        gs.visit(ResourceGroup::Geom, [&, bTakeScreenShot]( const std::string& _key, const std::string& _value ) {
            auto geom = GB<GT::Asset>(_value, GT::Tag(1001));
            if ( geom ) {
                geom->updateExistingTransform(V3fc::UP_AXIS_NEG * geom->volume().minPoint().y(),
                                              Quaternion{ (float) M_PI, V3fc::UP_AXIS }, V3fc::ONE);
                DC()->Mode(CameraControlType::Orbit);
                DC()->center(geom->volume(), CameraCenterAngle::HalfwayOpposite);
                takeScreenShotOnImportSignal(getCurrLoadedEntityId(), bTakeScreenShot);
            }
        });
    });
}

void SceneGraph::addGeomScene( const std::string& geomName, bool bTakeScreenShot ) {
    // If the object is already there, do not load it again
    if ( nodeExists(geomName)) {
        return;
    }
    addScene({ ResourceGroup::Geom, geomName }, bTakeScreenShot );
}

void SceneGraph::setMaterialRemap( const MaterialMap& _materialRemap ) {
    materialRemap = _materialRemap;
}

[[maybe_unused]] std::string SceneGraph::possibleRemap( const std::string& _key, const std::string& _value ) const {

    if ( const auto& it = materialRemap.find(_key + "," + _value); it != materialRemap.end()) {
        return it->second;
    }
    return _value;
}

void SceneGraph::HODResolve( const DependencyList& deps, HODResolverCallback ccf ) {
    SceneDependencyResolver sdr(*this);

    sdr.addDeps(deps);
    sdr.addResolveCallback(ccf);

    dependencyResolvers.push_back(sdr);
    dependencyResolvers.back().resolve();
}

void SceneGraph::loadCollisionMesh( std::shared_ptr<CollisionMesh> _cm ) {
    collisionMesh = _cm;
}

float SceneGraph::cameraCollisionDetection( std::shared_ptr<Camera> cam ) {
    if ( !collisionMesh || !bCollisionEnabled ) return 0.0f;
    float ret = collisionMesh->collisionDetection( cam->getPosition(), 0.10f );
    cam->setPosition(collisionMesh->getLastKnownGoodPosition());
    return ret;
}

void SceneGraph::setLastKnownGoodPosition( const V3f& _pos ) {
    if ( !collisionMesh ) return;
    collisionMesh->setLastKnownGoodPosition(_pos);
}

void SceneGraph::addGenericCallback( GenericSceneCallbackValue _value ) {
    SceneGraph::genericSceneCallback.emplace_back( _value );
}

void SceneGraph::addEventCallback( const std::string& _key, SocketCallbackDataType&& _value ) {
    SceneGraph::eventSceneCallback.emplace( _key, std::move(_value) );
}

bool SceneGraph::isCollisionEnabled() const {
    return bCollisionEnabled;
}

void SceneGraph::setCollisionEnabled( bool _bCollisionEnabled ) {
    bCollisionEnabled = _bCollisionEnabled;
}

void SceneGraph::enableCollisionGravity( bool _bGravity ) {
    if ( collisionMesh ) {
        collisionMesh->enableGravity(_bGravity);
    }
}

bool SceneGraph::isCollisionGravityEnabled() const {
    return collisionMesh && collisionMesh->isGravityEnabled();
}

const ResourceRef& SceneGraph::getCurrLoadedEntityId() const {
    return currLoadedEntityID;
}


void SceneGraph::getSceneStatsRec( const Geom* gg, SceneStats& stats, const std::unordered_set<uint64_t>& _exclusionTags ) const {

    for ( const auto& tag : _exclusionTags ) {
        if ( gg->Tag() == tag ) return;
    }

    for ( const auto& dd : gg->DataV() ) {
        auto vData = vl.get(dd.vData);
        stats.numNodes++;
        stats.numVerts += vData->numVerts();
        stats.numIndices += vData->numIndices();
    }

    for ( const auto& c : gg->Children() ) {
        getSceneStatsRec( c.get(), stats, _exclusionTags );
    }

}

SceneStats SceneGraph::getSceneStats( const std::unordered_set<uint64_t>& _exclusionTags ) const {
    SceneStats ret{};

    for ( const auto& [k, gg] : nodes ) {
        getSceneStatsRec(  gg.get(), ret, _exclusionTags );
    }

    return ret;
}

void SceneGraph::getFlattenNodesRec( const GeomSP& gg, std::vector<GeomSP>& _ret, const std::unordered_set<uint64_t>& _exclusionTags ) const {

    // Check if we need to exclude the geom
    for ( const auto& tag : _exclusionTags ) {
        if ( gg->Tag() == tag ) return;
    }

    _ret.emplace_back(gg);

    for ( const auto& c : gg->Children() ) {
        getFlattenNodesRec( c, _ret, _exclusionTags );
    }

}

std::vector<GeomSP> SceneGraph::getFlattenNodes(const std::unordered_set<uint64_t>& _exclusionTags) const {
    std::vector<GeomSP> ret;
    for ( const auto& [k, gg] : nodes ) {
        getFlattenNodesRec( gg, ret, _exclusionTags );
    }
    return ret;

}

void HOD::DepRemapsManager::addDep( SceneGraph& sg, const std::string& group, const std::string& resName ) {
    if ( !sg.exists(group, resName) && !resName.empty() ) {
        ret.emplace(group, resName);
        if ( group == ResourceGroup::Geom ) {
            geoms.emplace(resName);
        }
    }
}

void HOD::reducer( SceneGraph& sg, HOD::DepRemapsManager& deps, HODResolverCallback ccf ) {
    HOD::EntityList el{ deps.geoms };
    sg.HODResolve(deps.ret, ccf);
}
