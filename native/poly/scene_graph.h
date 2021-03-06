//
// Created by Dado on 08/02/2018.
//

#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <typeinfo>
#include <utility>
#include <variant>

#include <core/file_manager.h>
#include <core/uuid.hpp>
#include <core/geom.hpp>
#include <core/resources/material.h>
#include <core/recursive_transformation.hpp>
#include <core/resources/light.hpp>
#include <core/resources/resource_utils.hpp>
#include <core/resources/resource_manager.hpp>
#include <core/http/webclient_types.hpp>
#include <poly/poly.hpp>
#include <poly/node_graph.hpp>
#include <poly/vdata_assembler.h>
#include <poly/scene_dependency_resolver.hpp>

class SceneGraph;
struct LightmapSceneExchanger;
class Camera;
class CollisionMesh;

using HODResolverCallback = std::function<void()>;
using MaterialMap = std::unordered_map<std::string, std::string>;

JSONDATA( ResourceScene, resources )

    ResourceScene( const std::string& _resGroup, const std::string& _geomRes ) {
        resources[_resGroup].emplace_back(_geomRes);
    }

    void visit( const std::string& _key, const std::function<void( const std::string&, const std::string& )>& cb ) const {
        if ( auto it = resources.find(_key); it != resources.end() ) {
            for ( const auto& entry : it->second ) {
                cb( _key, entry );
            }
        }
    }

    std::unordered_map<std::string, std::vector<std::string>> resources;
};

namespace HOD { // HighOrderDependency

    JSONDATA( EntityList, entities )
        explicit EntityList( std::unordered_set<std::string>  entities ) : entities(std::move( entities )) {}
        std::unordered_set<std::string> entities;
    };

    class DepRemapsManager {
        public:
            void addDep( SceneGraph& sg, const std::string& group, const std::string& resName );
        public:
            std::unordered_set<std::string> geoms;
            DependencyList ret{};
    };

    template <typename T>
    DepRemapsManager resolveDependencies( const T* data, SceneGraph& sg );

    void reducer( SceneGraph& sg, DepRemapsManager& deps, HODResolverCallback ccf );

    template<typename T>
    void resolver( SceneGraph& sg, const T* data, HODResolverCallback ccf ) {
        auto deps = HOD::resolveDependencies<T>( data, sg );
        reducer( sg, deps, ccf );
    }

}

using GenericSceneCallbackValue = std::function<void()>;
using GenericSceneCallback = std::vector<GenericSceneCallbackValue>;
using EventSceneCallback = std::unordered_map<std::string, SocketCallbackDataType>;

struct SceneStats {
    unsigned int numNodes = 0;
    unsigned int numVerts = 0;
    unsigned int numIndices = 0;
};

enum class NodeTraverseMode {
    Add,
    Upsert
};

class SceneGraph : public NodeGraph {
public:
    explicit SceneGraph( VDataManager& _vl,
                         ImageManager& _tl,
                         ProfileManager& _pm,
                         MaterialManager& _ml,
                         ColorManager& _cl,
                         FontManager& _fm,
                         CameraManager& _cm,
                         GeomManager& _gm,
                         UIManager& _um,
                         LightManager& _ll );

    void init();
    GeomSP getNode( const UUID& _uuid );
    void traverseNode( GeomSP _node, int _nodeBucket, NodeTraverseMode _ntm );
    void traverseNode( const UUID& _uuid, int _nodeBucket, NodeTraverseMode _ntm );
    void updateNodes( const FlattenGeomSP& _nodes, int _nodeBucket);
    void removeNode( const UUID& _uuid );
    void removeNode( GeomSP _node );

    void addSkybox( CResourceRef skyName );

    void cmdChangeMaterialTag( const std::vector<std::string>& _params );
    void cmdChangeMaterialColorTag( const std::vector<std::string>& _params );
    void cmdLoadObject( const std::vector<std::string>& _params );
    void cmdCalcLightmaps( const std::vector<std::string>& _params );
    void cmdChangeTime( const std::vector<std::string>& params );
    void cmdReloadShaders( const std::vector<std::string>& _params );

    bool rayIntersect( const V3f& _near, const V3f& _far, SceneRayIntersectCallback _callback );

    size_t countGeoms() const;
    std::shared_ptr<Camera> DC();

    ImageManager&    TL() { return tl; }
    VDataManager&    VL() { return vl; }
    ProfileManager&  PL() { return pl; }
    MaterialManager& ML() { return ml; }
    ColorManager&    CL() { return cl; }
    CameraManager&   CM() { return cm; }
    FontManager&     FM() { return fm; }
    ColorManager&    MC() { return cl; }
    GeomManager&     GM() { return gm; }
    UIManager&       UM() { return um; }
    LightManager&    LL() { return ll; }

    [[maybe_unused,nodiscard]] std::shared_ptr<VData        >  VL( const ResourceRef& _ref ) const { return vl.get(_ref); }
    [[maybe_unused,nodiscard]] std::shared_ptr<RawImage     >  TL( const ResourceRef& _ref ) const { return tl.get(_ref); }
    [[maybe_unused,nodiscard]] std::shared_ptr<Material     >  ML( const ResourceRef& _ref ) const { return ml.get(_ref); }
    [[maybe_unused,nodiscard]] std::shared_ptr<Font         >  FM( const ResourceRef& _ref ) const { return fm.get(_ref); }
    [[maybe_unused,nodiscard]] std::shared_ptr<Profile      >  PL( const ResourceRef& _ref ) const { return pl.get(_ref); }
    [[maybe_unused,nodiscard]] std::shared_ptr<MaterialColor>  CL( const ResourceRef& _ref ) const { return cl.get(_ref); }
    [[maybe_unused,nodiscard]] std::shared_ptr<CameraRig    >  CM( const ResourceRef& _ref ) const { return cm.get(_ref); }
    [[maybe_unused,nodiscard]] std::shared_ptr<Geom         >  GM( const ResourceRef& _ref ) const { return gm.get(_ref); }
    [[maybe_unused,nodiscard]] std::shared_ptr<MaterialColor>  MC( const ResourceRef& _ref ) const { return cl.get(_ref); }
    [[maybe_unused,nodiscard]] std::shared_ptr<UIContainer>    UL( const ResourceRef& _ref ) const { return um.get(_ref); }
    [[maybe_unused,nodiscard]] std::shared_ptr<Light>          LL( const ResourceRef& _ref ) const { return ll.get(_ref); }

    template <typename T>
    std::shared_ptr<T> get( const ResourceRef& _ref ) {
        if constexpr ( std::is_same_v<T, VData>         ) { return vl.get(_ref); }
        if constexpr ( std::is_same_v<T, RawImage>      ) { return tl.get(_ref); }
        if constexpr ( std::is_same_v<T, Material>      ) { return ml.get(_ref); }
        if constexpr ( std::is_same_v<T, Font>          ) { return fm.get(_ref); }
        if constexpr ( std::is_same_v<T, Profile>       ) { return pl.get(_ref); }
        if constexpr ( std::is_same_v<T, MaterialColor> ) { return cl.get(_ref); }
        if constexpr ( std::is_same_v<T, CameraRig>     ) { return cm.get(_ref); }
        if constexpr ( std::is_same_v<T, Geom>          ) { return gm.get(_ref); }
        if constexpr ( std::is_same_v<T, UIContainer>   ) { return um.get(_ref); }
        if constexpr ( std::is_same_v<T, Light>         ) { return ll.get(_ref); }
    }

    template <typename T>
    std::vector<std::string> getNames( const ResourceRef& _ref ) {
        if constexpr ( std::is_same_v<T, VData>         ) { return vl.getNames(_ref); }
        if constexpr ( std::is_same_v<T, RawImage>      ) { return tl.getNames(_ref); }
        if constexpr ( std::is_same_v<T, Material>      ) { return ml.getNames(_ref); }
        if constexpr ( std::is_same_v<T, Font>          ) { return fm.getNames(_ref); }
        if constexpr ( std::is_same_v<T, Profile>       ) { return pl.getNames(_ref); }
        if constexpr ( std::is_same_v<T, MaterialColor> ) { return cl.getNames(_ref); }
        if constexpr ( std::is_same_v<T, CameraRig>     ) { return cm.getNames(_ref); }
        if constexpr ( std::is_same_v<T, Geom>          ) { return gm.getNames(_ref); }
        if constexpr ( std::is_same_v<T, UIContainer>   ) { return um.getNames(_ref); }
        if constexpr ( std::is_same_v<T, Light>         ) { return ll.getNames(_ref); }
    }

    template <typename T>
    [[nodiscard]] const T* get( const ResourceRef& _ref ) const {
        if constexpr ( std::is_same_v<T, VData>         ) { return vl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, RawImage>      ) { return tl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Material>      ) { return ml.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Font>          ) { return fm.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Profile>       ) { return pl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, MaterialColor> ) { return cl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, CameraRig>     ) { return cm.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Geom>          ) { return gm.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, UIContainer>   ) { return um.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Light>         ) { return ll.get(_ref).get(); }
    }

    template <typename T>
    [[nodiscard]] const T* getPtr( const ResourceRef& _ref ) const {
        if constexpr ( std::is_same_v<T, VData>         ) { return vl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, RawImage>      ) { return tl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Material>      ) { return ml.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Font>          ) { return fm.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Profile>       ) { return pl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, MaterialColor> ) { return cl.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, CameraRig>     ) { return cm.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Geom>          ) { return gm.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, UIContainer>   ) { return um.get(_ref).get(); }
        if constexpr ( std::is_same_v<T, Light>         ) { return ll.get(_ref).get(); }
    }

    template <typename T>
    ResourceRef getHash( const ResourceRef& _ref ) {
        if constexpr ( std::is_same_v<T, VData>         ) { return vl.getHash(_ref); }
        if constexpr ( std::is_same_v<T, RawImage>      ) { return tl.getHash(_ref); }
        if constexpr ( std::is_same_v<T, Material>      ) { return ml.getHash(_ref); }
        if constexpr ( std::is_same_v<T, Font>          ) { return fm.getHash(_ref); }
        if constexpr ( std::is_same_v<T, Profile>       ) { return pl.getHash(_ref); }
        if constexpr ( std::is_same_v<T, MaterialColor> ) { return cl.getHash(_ref); }
        if constexpr ( std::is_same_v<T, CameraRig>     ) { return cm.getHash(_ref); }
        if constexpr ( std::is_same_v<T, Geom>          ) { return gm.getHash(_ref); }
        if constexpr ( std::is_same_v<T, UIContainer>   ) { return um.getHash(_ref); }
        if constexpr ( std::is_same_v<T, Light>         ) { return ll.getHash(_ref); }
    }

    template <typename R>
    auto& M() {
        if constexpr ( std::is_same_v<R, VData>         ) return VL();
        if constexpr ( std::is_same_v<R, RawImage>      ) return TL();
        if constexpr ( std::is_same_v<R, Material>      ) return ML();
        if constexpr ( std::is_same_v<R, Font>          ) return FM();
        if constexpr ( std::is_same_v<R, Profile>       ) return PL();
        if constexpr ( std::is_same_v<R, MaterialColor> ) return MC();
        if constexpr ( std::is_same_v<R, CameraRig>     ) return CM();
        if constexpr ( std::is_same_v<R, Geom>          ) return GM();
        if constexpr ( std::is_same_v<R, UIContainer>   ) return UM();
        if constexpr ( std::is_same_v<R, Light      >   ) return LL();
    }

    template <typename R>
    static void addDeferred( const ResourceRef& _key, const ResourceRef& _hash, SerializableContainer&& _res, HttpResourceCB _ccf = nullptr ) {
        if constexpr ( std::is_same_v<R, VData          > ) resourceCallbackVData        .emplace_back( _key, _hash, std::move(_res), _ccf );
        else if constexpr ( std::is_same_v<R, RawImage       > ) resourceCallbackRawImage     .emplace_back( _key, _hash, std::move(_res), _ccf );
        else if constexpr ( std::is_same_v<R, Material       > ) resourceCallbackMaterial     .emplace_back( _key, _hash, std::move(_res), _ccf );
        else if constexpr ( std::is_same_v<R, Font           > ) resourceCallbackFont         .emplace_back( _key, _hash, std::move(_res), _ccf );
        else if constexpr ( std::is_same_v<R, Profile        > ) resourceCallbackProfile      .emplace_back( _key, _hash, std::move(_res), _ccf );
        else if constexpr ( std::is_same_v<R, MaterialColor  > ) resourceCallbackMaterialColor.emplace_back( _key, _hash, std::move(_res), _ccf );
        else if constexpr ( std::is_same_v<R, CameraRig      > ) resourceCallbackCameraRig    .emplace_back( _key, _hash, std::move(_res), _ccf );
        else if constexpr ( std::is_same_v<R, Geom           > ) resourceCallbackGeom         .emplace_back( _key, _hash, std::move(_res), _ccf );
        else if constexpr ( std::is_same_v<R, UIContainer    > ) resourceCallbackUI           .emplace_back( _key, _hash, std::move(_res), _ccf );
        else if constexpr ( std::is_same_v<R, Light          > ) resourceCallbackLight        .emplace_back( _key, _hash, std::move(_res), _ccf );
    }

    static void addDeferredComp( const ResourceRef& _key, SerializableContainer&& _data, CHttpResourceCB& _ccf = nullptr ) {
        resourceCallbackComposite.emplace_back( _key, "", std::move(_data), _ccf );
    }

    bool exists( const std::string& group, const std::string& resName ) {
        if ( ResourceGroup::VData         == group ) return VL().exists(resName) != nullptr;
        if ( ResourceGroup::Image         == group ) return TL().exists(resName) != nullptr;
        if ( ResourceGroup::Material      == group ) return ML().exists(resName) != nullptr;
        if ( ResourceGroup::Font          == group ) return FM().exists(resName) != nullptr;
        if ( ResourceGroup::Profile       == group ) return PL().exists(resName) != nullptr;
        if ( ResourceGroup::Color         == group ) return MC().exists(resName) != nullptr;
        if ( ResourceGroup::CameraRig     == group ) return CM().exists(resName) != nullptr;
        if ( ResourceGroup::Geom          == group ) return GM().exists(resName) != nullptr;
        if ( ResourceGroup::UI            == group ) return UM().exists(resName) != nullptr;
        if ( ResourceGroup::Light         == group ) return LL().exists(resName) != nullptr;
        return false;
    }

    template <typename R>
    ResourceRef add( const ResourceRef& _key, const R& _res, CHttpResourceCB& _ccf = nullptr ) {
        if constexpr ( std::is_same_v<R, VData          > ) return addVData        ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, RawImage       > ) return addRawImage     ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, Material       > ) return addMaterial     ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, Font           > ) return addFont         ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, Profile        > ) return addProfile      ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, MaterialColor  > ) return addMaterialColor( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, CameraRig      > ) return addCameraRig    ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, Geom           > ) return addGeom         ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, UIContainer    > ) return addUI           ( _key, _res, _ccf );
        if constexpr ( std::is_same_v<R, Light          > ) return addLight        ( _key, _res, _ccf );
    }

    ResourceRef addVData         ( const ResourceRef& _key, const VData        & _res, HttpResourceCB _ccf = nullptr );
    ResourceRef addRawImage      ( const ResourceRef& _key, const RawImage     & _res, HttpResourceCB _ccf = nullptr );
    ResourceRef addMaterial      ( const ResourceRef& _key, const Material     & _res, HttpResourceCB _ccf = nullptr );
    ResourceRef addFont          ( const ResourceRef& _key, const Font         & _res, HttpResourceCB _ccf = nullptr );
    ResourceRef addProfile       ( const ResourceRef& _key, const Profile      & _res, HttpResourceCB _ccf = nullptr );
    ResourceRef addMaterialColor ( const ResourceRef& _key, const MaterialColor& _res, HttpResourceCB _ccf = nullptr );
    ResourceRef addCameraRig     ( const ResourceRef& _key, const CameraRig    & _res, HttpResourceCB _ccf = nullptr );
    ResourceRef addGeom          ( const ResourceRef& _key,       GeomSP         _res, HttpResourceCB _ccf = nullptr );
    ResourceRef addUI            ( const ResourceRef& _key, const UIContainer&   _res, HttpResourceCB _ccf = nullptr );
    ResourceRef addLight         ( const ResourceRef& _key, const Light&         _res, HttpResourceCB _ccf = nullptr );
    void addResources( CResourceRef _key, const SerializableContainer& _data, HttpResourceCB _ccf = nullptr );

    ResourceRef addVDataIM       ( const ResourceRef& _key, const VData& _res );
    ResourceRef addRawImageIM    ( const ResourceRef& _key, const RawImage     & _res );
    ResourceRef addMaterialIM    ( const ResourceRef& _key, const Material     & _res );
    ResourceRef addProfileIM     ( const ResourceRef& _key, const Profile      & _res );
    ResourceRef addUIIM          ( const ResourceRef& _key, const UIContainer& _res );

    static void addGenericCallback( GenericSceneCallbackValue _value );
    static void addEventCallback( const std::string& _key, SocketCallbackDataType&& _value );

    void updateRawImage      ( const ResourceRef& _key );

    template <typename T>
    T B( const std::string& _name ) {
        return T{ *this, _name };
    }

    void loadVData         ( std::string _names, HttpResourceCB _ccf = nullptr );
    void loadRawImage      ( std::string _names, HttpResourceCB _ccf = nullptr );
    void loadMaterial      ( std::string _names, HttpResourceCB _ccf = nullptr );
    void loadFont          ( std::string _names, HttpResourceCB _ccf = nullptr );
    void loadProfile       ( std::string _names, HttpResourceCB _ccf = nullptr );
    void loadMaterialColor ( std::string _names, HttpResourceCB _ccf = nullptr );
    void loadCameraRig     ( std::string _names, HttpResourceCB _ccf = nullptr );
//    void loadLogicalUI     ( std::string _names, HttpResourceCB _ccf = nullptr );
    void loadGeom          ( std::string _names, HttpResourceCB _ccf = nullptr );
    void loadUI            ( std::string _names, HttpResourceCB _ccf = nullptr );

    void loadAsset         ( const std::string& _names );

    void loadScene         ( const ResourceScene& gs, HODResolverCallback _ccf = nullptr );
    void addScene          ( const ResourceScene& gs, bool bTakeScreenShot );
    void addGeomScene      ( const std::string& geomName, bool bTakeScreenShot );
    void resetAndLoadEntity( CResourceRef v0, const std::string& entityGroup, bool bTakeScreenShot );

    template <typename R>
    void acquire( const std::string& _names, CHttpResourceCB& _ccf = nullptr ) {
        auto filenameKey = getFileNameKey(_names);
        if ( get<RawImage>(filenameKey) ) {
            _ccf(filenameKey);
        } else {
            load<RawImage>( filenameKey, _ccf );
        }
    }

    template <typename R>
    void load( std::string _names, CHttpResourceCB& _ccf = nullptr ) {

        replaceAllStrings( _names, " ", "," );
        if constexpr ( std::is_same_v<R, VData               > ) loadVData        ( std::move(_names), _ccf );
        else if constexpr ( std::is_same_v<R, RawImage            > ) loadRawImage     ( std::move(_names), _ccf );
        else if constexpr ( std::is_same_v<R, Material            > ) loadMaterial     ( std::move(_names), _ccf );
        else if constexpr ( std::is_same_v<R, Font                > ) loadFont         ( std::move(_names), _ccf );
        else if constexpr ( std::is_same_v<R, Profile             > ) loadProfile      ( std::move(_names), _ccf );
        else if constexpr ( std::is_same_v<R, MaterialColor       > ) loadMaterialColor( std::move(_names), _ccf );
        else if constexpr ( std::is_same_v<R, CameraRig           > ) loadCameraRig    ( std::move(_names), _ccf );
        else if constexpr ( std::is_same_v<R, UIContainer         > ) loadUI           ( std::move(_names), _ccf );
        else if constexpr ( std::is_same_v<R, Geom                > ) loadGeom         ( std::move(_names), _ccf );
    }

    std::tuple<std::string, V3f> getGeomNameSize( const ResourceRef& _ref ) const {
        return { _ref, get<Geom>( _ref )->size() };
    }

    void mapGeomType( uint64_t _value, const std::string& _key );
    uint64_t getGeomType( const std::string& _key ) const;

    void update();

//    NodeGraphContainer& Nodes();
//    const NodeGraphContainer& Nodes() const ;

    [[nodiscard]] bool nodeExists( const std::string& _name ) const;

    template <typename T>
    void visitNode( const UUID& _uuid, T _visitor ) {
        if ( auto it = nodes.find(_uuid); it != nodes.end() ) {
            std::visit(_visitor, it->second);
        }
    }

    template <typename T>
    void visitNodes( T _visitor ) {
        for ( const auto& it : nodes ) {
            it.second->visit( _visitor );
        }
    }


    template <typename T>
    void foreachNodeDataRec( const Geom* gg, T _f ) {

        for ( const auto& dd : gg->DataV() ) {
            _f(vl.get(dd.vData).get(), dd.vData);
        }

        for ( const auto& c : gg->Children() ) {
            foreachNodeDataRec( c.get(), _f );
        }
    }

    template <typename T>
    void foreachNodeData( T _f ) {
        for ( auto& it : nodes ) {
            foreachNodeDataRec( it.second.get(), _f );
        }
    }

    template <typename T>
    void foreachNode( T _f ) {
        for ( auto& it : nodes ) {
            it.second->foreach( _f );
        }
    }

    template <typename T>
    void transformNode( const std::string& _name, T _f ) {
        for ( auto& it : nodes ) {
            if ( it.second->Name() == _name ) {
                it.second->transform( _f );
            }
        }
    }

    template <typename T, typename ...Args>
    GeomSP GB( Args&&... args ) {
        VDataAssembler<T> gb{std::forward<Args>(args)...};
        GeomSP elem;
        bool hasBeenPrepared = true;
        if constexpr ( std::is_same_v<T, GT::Asset> ) {
            if ( auto elemToClone = get<Geom>(gb.dataTypeHolder.nameId); elemToClone ) {
                elem = EF::clone(elemToClone);
                if ( gb.tag ) elem->setTag(gb.tag);
                if ( gb.elemInjFather ) gb.elemInjFather->addChildren(elem, gb.dataTypeHolder.pos, gb.dataTypeHolder.axis, gb.dataTypeHolder.scale);
                if ( !gb.matRef.empty() && gb.matRef != S::WHITE_PBR ) {
                    auto mt = GBMatInternal(gb.matRef, gb.matColor );
                    ResourceRef matRef = std::get<0>(mt);
                    auto programRef = gb.programRef;
                    elem->foreach( [matRef, programRef](const GeomSP& _geom) {
                        if ( !_geom->empty() ) {
                            _geom->DataRef().material = matRef;
                            _geom->DataRef().program = programRef;
                        }
                    });
                }
            }
        } else {
            auto [matRef, matPtr] = GBMatInternal(gb.matRef, gb.matColor );

            hasBeenPrepared = VDataServices::prepare( *this, gb.dataTypeHolder, matPtr );
            if ( hasBeenPrepared ) {
                auto hashRefName = VDataServices::refName( gb.dataTypeHolder );
                auto vDataRef = VL().getHash(hashRefName );

                if ( vDataRef.empty() ) {
                    vDataRef = B<VB>(hashRefName ).addIM(VDataServices::build(gb.dataTypeHolder), hashRefName );
                }
                auto vDataPtr = VL().getFromHash(vDataRef);

                elem = std::make_shared<Geom>(gb.Name());
                if ( gb.tag ) elem->setTag(gb.tag);
                elem->pushData( AABB{vDataPtr->getMin(), vDataPtr->getMax()}, vDataRef, matRef, gb.programRef );

                if ( gb.elemInjFather ) gb.elemInjFather->addChildren(elem, gb.dataTypeHolder.pos, gb.dataTypeHolder.axis, gb.dataTypeHolder.scale);
                B<GRB>( gb.Name() ).addIM( elem );
            }
        }
        if ( !gb.elemInjFather && hasBeenPrepared ) {
            elem->updateTransform(gb.dataTypeHolder.pos, gb.dataTypeHolder.axis, gb.dataTypeHolder.scale);
            traverseNode(elem, gb.bucketIndex, NodeTraverseMode::Add);
        }
        return elem;
    }

    void loadCollisionMesh( std::shared_ptr<CollisionMesh> _cm );
    float cameraCollisionDetection( std::shared_ptr<Camera> cam );
    void setLastKnownGoodPosition( const V3f& _pos );

    static GenericSceneCallback           genericSceneCallback         ;
    static EventSceneCallback             eventSceneCallback           ;
    static LoadedResouceCallbackContainer resourceCallbackVData        ;
    static LoadedResouceCallbackContainer resourceCallbackRawImage     ;
    static LoadedResouceCallbackContainer resourceCallbackMaterial     ;
    static LoadedResouceCallbackContainer resourceCallbackFont         ;
    static LoadedResouceCallbackContainer resourceCallbackProfile      ;
    static LoadedResouceCallbackContainer resourceCallbackMaterialColor;
    static LoadedResouceCallbackContainer resourceCallbackCameraRig    ;
    static LoadedResouceCallbackContainer resourceCallbackGeom         ;
    static LoadedResouceCallbackContainer resourceCallbackUI           ;
    static LoadedResouceCallbackContainer resourceCallbackLight        ;
    static LoadedResouceCallbackContainer resourceCallbackComposite    ;

protected:
    template <typename  T>
    void loadResourceCompositeCallback( T& cba ) {
        if ( !cba.empty()) {
            auto res = cba.back();
            addResources( res.key, res.data, res.ccf );
            cba.pop_back();
        }
    }

    template <typename R, typename BB>
    void loadResourceCallback( LoadedResouceCallbackContainer& cba ) {
        if ( !cba.empty()) {
            auto res = cba.back();
            if ( !res.data.empty() ) {
                B<BB>( res.key ).addDF( R{ res.data }, res.ccf );
            }
            cba.pop_back();
        }
    }

    template <typename R, typename BB>
    void loadResourceCallbackWithKey( LoadedResouceCallbackContainer& cba ) {
        if ( !cba.empty()) {
            auto res = cba.back();
            auto r = R{ res.data };
            r.Key( res.key );
            B<BB>( res.key ).addDF( r, res.ccf );
            cba.pop_back();
        }
    }

    template <typename R, typename BB, typename T>
    void loadResourceCallbackWithLoader( LoadedResouceCallbackContainer& cba, T loadFunc ) {
        if ( !cba.empty()) {
            auto res = cba.back();
            auto ent = loadFunc( *this, res.key, res.hash, res.data );
            if ( ent ) {
                B<BB>( res.key ).addIM( ent );
                if ( res.ccf ) res.ccf( res.key );
            } else {
                LOGRS( "[LOAD-RESOURCE][ERROR] " << res.key << " failed to load" )
            }
            cba.pop_back();
        }
    }

    void getNodeRec( const UUID& _uuid, const GeomSP& _node, GeomSP& ret );
    void getFlattenNodesRec( const GeomSP& gg, FlattenGeomSP& _ret, const std::unordered_set<uint64_t>& _exclusionTags ) const;
    void getSceneStatsRec( const Geom* gg, SceneStats& stats, const std::unordered_set<uint64_t>& _exclusionTags ) const;

    void genericCallbacks();
    void realTimeCallbacks();
    void loadCallbacks();

    std::tuple<ResourceRef, Material*> GBMatInternal( CResourceRef _matRef, const C4f& _color );
    void materialsForGeomSocketMessage();
    void replaceMaterialOnNodes( const std::string& _key );

public:
    bool isCollisionEnabled() const;
    void setCollisionEnabled( bool );
    [[nodiscard]] bool isCollisionGravityEnabled() const;
    void enableCollisionGravity( bool _bGravity );

    void clearNodes();
    void clearGMNodes();
    [[nodiscard]] FlattenGeomSP getFlattenNodes(const std::unordered_set<uint64_t>& _exclusionTags) const;
    [[nodiscard]] SceneStats getSceneStats(const std::unordered_set<uint64_t>& _exclusionTags = {}) const;

    const ResourceRef& getCurrLoadedEntityId() const;
    void setMaterialRemap( const MaterialMap& materialRemap );
    [[maybe_unused]] [[nodiscard]] std::string possibleRemap( const std::string& _key, const std::string& _value ) const;
    void HODResolve( const DependencyList& deps, HODResolverCallback ccf );

protected:
    VDataManager& vl;
    ImageManager& tl;
    ProfileManager& pl;
    MaterialManager& ml;
    ColorManager& cl;
    FontManager& fm;
    CameraManager& cm;
    GeomManager& gm;
    UIManager& um;
    LightManager& ll;

    std::shared_ptr<CollisionMesh> collisionMesh;
    bool bCollisionEnabled = true;
    MaterialMap materialRemap;
    ResourceRef currLoadedEntityID{};
    std::vector<SceneDependencyResolver> dependencyResolvers;
};

class MaterialThumbnail : public Material {
public:
    explicit MaterialThumbnail( SceneGraph* _sg, const Material& _mat );
    JSONSERIALONLY( MaterialThumbnail, mKey, values, thumbValues );

protected:
    void setThumbnailFor( const std::string& _textureType );
protected:
    std::unordered_map<std::string, std::string> thumbValues;
    SceneGraph* sg;
};
