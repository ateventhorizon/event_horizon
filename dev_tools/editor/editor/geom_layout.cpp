//
// Created by Dado on 2018-10-29.
//

#include "geom_layout.h"
#include <graphics/imgui/imgui_jsonvisit.hpp>
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene_orchestrator.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>
#include <core/node.hpp>
#include <core/geom.hpp>
#include <core/camera.h>
#include <core/raw_image.h>
#include <core/geom.hpp>
#include <core/camera_rig.hpp>
#include <poly/resources/geom_builder.h>
#include <poly/resources/ui_shape_builder.h>
#include <poly/converters/gltf2/gltf2.h>

//std::shared_ptr<GeomBuilder> gbt;
std::string svgString;
std::string svgName;
std::string glftString;
SerializableContainer gltfBufferData;

template <typename T>
struct NodeVisitor {
//    void operator()( GeomSP _v ) { _v->visit<T>(); }
//    void operator()( UIAssetSP _v ) { _v->visit<T>(); }
//    void operator()( CameraAssetSP _v ) { _v->visit<T>(); }
};

void loadGeomInGui( SceneOrchestrator* p, std::shared_ptr<GLTF2> _newObject ) {

    // convert returns a vector of scenes read from GLTF file
    for ( const auto& scene : _newObject->convert() ) {
        auto geom = std::make_shared<Geom>(scene->Name());
    }
//    auto hierScene = imported.getScene();
//    p->getCamera(Name::Foxtrot)->center(hierScene->BBox3d());
//    p->SG().B<GRB>(_newObject->Name()).addIM( hierScene );
//    p->SG().add( hierScene );
}

void addGeomToScene() {
    SceneOrchestrator::sUpdateCallbacks.emplace_back( []( SceneOrchestrator* p ) {
        loadGeomInGui( p, std::make_shared<GLTF2>( glftString ) );
    } );
}

void addGeomToSceneData() {
    SceneOrchestrator::sUpdateCallbacks.emplace_back( []( SceneOrchestrator* p ) {
        loadGeomInGui( p, std::make_shared<GLTF2>( gltfBufferData, glftString ) );
        gltfBufferData.clear();
        glftString = "";
    } );
}

void callbackGeom( const std::string& _filename, const SerializableContainer& _data ) {
    glftString = _filename;
    gltfBufferData = _data;
    addGeomToSceneData();
}

void callbackGeomGLTF( const std::string& _filename ) {
    glftString = _filename;
    addGeomToScene();
}

void callbackGeomSVG( const std::string& _filename, const std::string& _svgString ) {
    svgString = _svgString;
    svgName = getFileNameOnly(_filename);
    SceneOrchestrator::sUpdateCallbacks.emplace_back( [&]( SceneOrchestrator* p ) {
        GB{p->SG(),GeomBuilderType::svg}.n(svgName).ascii(svgString).pb(0.015f, 6.0f).buildr();
    } );
}

void ImGuiGeoms::renderImpl( SceneOrchestrator* p, Rect2f& _r ) {
//    for ( auto& [k,v] : p->SG().Nodes() ) {
//        ImGui::PushID(std::visit(lambdaUUID, v).c_str());
//        ImGui::BeginGroup();
//        std::visit( NodeVisitor<ImGUIJson>{}, v );
//        if ( ImGui::Button( "Save", ImVec2( 80, 20 ))) {
//            VisitLambda( publish, v );
//        }
//        ImGui::EndGroup();
//        ImGui::PopID();
//    }

//    ImGui::EndChild();

//    if ( gbt ) {
//        ImGui::BeginGroup();
//        ImGui::Text( "Name: %s", gbt->Name().c_str());
//        ImGui::EndGroup();
//    }
}