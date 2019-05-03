//
// Created by Dado on 08/02/2018.
//

#include "render_orchestrator.h"
#include <core/resources/resource_utils.hpp>
#include <core/resources/resource_manager.hpp>
#include <core/math/vector_util.hpp>
#include <core/raw_image.h>
#include <core/camera.h>
#include <core/node.hpp>
#include <core/geom.hpp>
#include <core/resources/material.h>
#include <poly/resources/ui_shape_builder.h>
#include <graphics/renderer.h>
#include <graphics/shader_manager.h>
#include <graphics/vp_builder.hpp>
#include <graphics/audio/audio_manager_openal.hpp>
#include <graphics/window_handling.hpp>
#include <render_scene_graph/scene_bridge.h>

std::vector<std::string> RenderOrchestrator::callbackPaths;
Vector2i RenderOrchestrator::callbackResizeWindow = Vector2i(-1, -1);
Vector2i RenderOrchestrator::callbackResizeFrameBuffer = Vector2i(-1, -1);
std::vector<PresenterUpdateCallbackFunc> RenderOrchestrator::sUpdateCallbacks;

void GDropCallback( [[maybe_unused]] GLFWwindow *window, int count, const char **paths ) {
    ASSERT( count > 0 );

    RenderOrchestrator::callbackPaths.clear();
    for ( auto i = 0; i < count; i++ ) {
        RenderOrchestrator::callbackPaths.emplace_back( std::string(paths[i]) );
    }
}

void GResizeWindowCallback( [[maybe_unused]] GLFWwindow *, int w, int h ) {
    RenderOrchestrator::callbackResizeWindow = Vector2i{w, h};
}

void GResizeFramebufferCallback( [[maybe_unused]] GLFWwindow *, int w, int h ) {
    RenderOrchestrator::callbackResizeFrameBuffer = Vector2i{w, h};
}

void RenderOrchestrator::setDragAndDropFunction( DragAndDropFunction dd ) {
    dragAndDropFunc = dd;
}

void RenderOrchestrator::addUpdateCallback( PresenterUpdateCallbackFunc uc ) {
    sUpdateCallbacks.push_back( uc );
}

void RenderOrchestrator::updateCallbacks() {

    if ( !sUpdateCallbacks.empty() ) {
        for ( auto& c : sUpdateCallbacks ) {
            c( this );
        }
        sUpdateCallbacks.clear();
    }

    if ( !callbackPaths.empty() ) {
        for ( auto& path : callbackPaths ) {
            if ( dragAndDropFunc ) dragAndDropFunc( this, path );
        }
        callbackPaths.clear();
    }

    if ( callbackResizeWindow.x() > 0 && callbackResizeWindow.y() > 0 ) {
        // For now we do everything in the callbackResizeFrameBuffer so this is redundant for now, just a nop
        // to be re-enabled in the future if we need it
//		LOGR("Resized window: [%d, %d]", callbackResizeWindow.x(), callbackResizeWindow.y() );
        callbackResizeWindow = Vector2i{-1, -1};
    }

    if ( callbackResizeFrameBuffer.x() > 0 && callbackResizeFrameBuffer.y() > 0 ) {
        WH::resizeWindow( callbackResizeFrameBuffer );
        WH::gatherMainScreenInfo();
		rr.resetDefaultFB(callbackResizeFrameBuffer);
        resizeCallback( callbackResizeFrameBuffer );
        callbackResizeFrameBuffer = Vector2i{-1, -1};
    }

}

RenderOrchestrator::RenderOrchestrator( Renderer& rr, SceneGraph& _sg ) : rr( rr ), sg(_sg) {

    sg.TL().connect( [this](const ResourceTransfer<RawImage>& _val ) {
        LOGRS( "[SG-Resrouce] Add " << ResourceVersioning<RawImage>::Prefix() << ": "  << *_val.names.begin() );
        this->RR().addTextureResource(_val);
        if ( _val.ccf ) _val.ccf();
    });

    sg.ML().connect( [this](const ResourceTransfer<Material>& _val ) {
        LOGRS( "[SG-Resrouce] Add " << ResourceVersioning<Material>::Prefix() << ": "  << *_val.names.begin() );
        this->RR().addMaterialResource(_val);
        if ( _val.ccf ) _val.ccf();
    });

    sg.VL().connect( [this](const ResourceTransfer<VData>& _val ) {
        LOGRS( "[SG-Resrouce] Add " << ResourceVersioning<VData>::Prefix() << ": "  << *_val.names.begin() );
        this->RR().addVDataResource(_val);
        if ( _val.ccf ) _val.ccf();
    });

    sg.nodeAddConnect( [this]( NodeGraphConnectParamsSig _geom ) {
        auto bEmpty = _geom->empty();
        LOGRS( "[SG-Node] Add " << (bEmpty ? "Root " : "") << _geom->Name() );
        if ( bEmpty ) return;
        auto dataRef = _geom->DataRef(0);
        auto transformMatrix = std::make_shared<Matrix4f>(_geom->getLocalHierTransform());
        auto vp = VPBuilder<PosTexNorTanBinUV2Col3dStrip>{ this->RR(), dataRef.material, dataRef.vData}.n(_geom->UUiD()).t(transformMatrix).build();
        this->RR().VPL( CommandBufferLimits::PBRStart, vp);
    });

    am = std::make_shared<AudioManagerOpenAL>();
}

void RenderOrchestrator::updateInputs( const AggregatedInputData& _aid ) {
    updateCallbacks();

    for ( auto& [k,v] : mRigs ) {
        v->updateFromInputData( _aid );
    }
}

void RenderOrchestrator::init() {
    WH::setDropCallback( GDropCallback );
    WH::setResizeWindowCallback( GResizeWindowCallback );
    WH::setResizeFramebufferCallback( GResizeFramebufferCallback );

#ifndef _PRODUCTION_
    Socket::on( "shaderchange",
                std::bind(&RenderOrchestrator::reloadShaders, this, std::placeholders::_1 ) );
#endif

    // Set a fullscreen camera by default
    addRig<CameraControlFly>( Name::Foxtrot, 0.0f, 1.0f, 0.0f, 1.0f );
}

void RenderOrchestrator::reloadShaders( SocketCallbackDataTypeConstRef _data ) {

    ShaderLiveUpdateMap shadersToUpdate{_data};

	for ( const auto& ss : shadersToUpdate.shaders ) {
		rr.injectShader( ss.first, ss.second );
	}

	addUpdateCallback( [this](UpdateCallbackSign) { rr.cmdReloadShaders( {} ); } );

}

//void RenderOrchestrator::addImpl( NodeVariants _geom ) {
//}

//void RenderOrchestrator::removeImpl( const UUID& _uuid ) {
//    auto removeF = [&](const UUID& _uuid) { rr.removeFromCL(_uuid ); };
//    if ( auto it = geoms.find(_uuid); it != geoms.end() ) {
//        std::visit( [&](auto&& arg) { arg->visitHashRecF(removeF);}, it->second );
//    }
//}
//
//void RenderOrchestrator::cmdChangeTimeImpl( const std::vector<std::string>& _params ) {
//    SB().buildFromString( concatenate( " ", {_params.begin(), _params.end()}) );
//    RR().changeTime( SB().getSunPosition() );
//}
//
//void RenderOrchestrator::cmdloadObjectImpl( const std::vector<std::string>& _params ) {
//    Vector3f pos = Vector3f::ZERO;
//    Vector3f rot = Vector3f::ZERO;
//    std::string objName = _params[0];
//    if ( _params.size() == 4 || _params.size() == 7 ) {
//        pos = { std::stof(_params[1]), std::stof(_params[2]), std::stof(_params[3]) };
//    }
//    if ( _params.size() == 7 ) {
//        rot = { std::stof(_params[4]), std::stof(_params[5]), std::stof(_params[6]) };
//    }
////    ### I'm pretty positive Geom Files need to go through the load function of a dependent geom builder.
////    GF{*this, _params[0] }.build();
//}

void RenderOrchestrator::changeMaterialTagCallback( const std::vector<std::string>& _params ) {
//    std::shared_ptr<Material> mat = std::dynamic_pointer_cast<Material>(sg.ML().get(concatParams(_params, 1)));
//    rr.changeMaterialOnTagsCallback( { sg.getGeomType( _params[0] ), mat->Name() } );
}

//void RenderOrchestrator::changeMaterialTagImpl( const std::vector<std::string>& _params ) {
//    std::string keys = concatenate(" ", { _params.begin()+1, _params.end()} );
//    MB{ML(), concatParams(_params, 1)}.load(
//            std::bind( &RenderOrchestrator::changeMaterialTagCallback, this, _params ),
//            _params );
//}

void RenderOrchestrator::changeMaterialColorCallback( const std::vector<std::string>& _params ) {
    rr.changeMaterialColorOnTags( sg.getGeomType( _params[0] ), sg.CL().get(concatParams(_params, 1))->color );
}

Renderer& RenderOrchestrator::RR() { return rr; }

void RenderOrchestrator::addBoxToViewport( const std::string& _name, const SceneScreenBox& _box ) {
    if ( boxes.find(_name) != boxes.end() ) return;
    boxes[_name] = _box;
}

std::shared_ptr<CameraRig> RenderOrchestrator::getRig( const std::string& _name ) {
    return sg.CM().get(_name);
}

std::shared_ptr<Camera> RenderOrchestrator::getCamera( const std::string& _name ) {
    return sg.CM().get(_name)->getMainCamera();
}

const Camera* RenderOrchestrator::getCamera( const std::string& _name ) const {
    return sg.CM().get(_name)->getMainCamera().get();
}

void RenderOrchestrator::setViewportOnRig( std::shared_ptr<CameraRig> _rig, const Rect2f& _viewport ) {
    rr.getTarget(_rig->Name())->getRig()->setViewport(_viewport);
}

void RenderOrchestrator::setViewportOnRig( const std::string& _rigName, const Rect2f& _viewport ) {
    rr.getTarget(_rigName)->getRig()->setViewport(_viewport);
}

void RenderOrchestrator::addBox( const std::string& _name, float _l, float _r, float _t, float _b, bool _bVisible ) {
//    if ( auto rlf = boxFunctionMapping.find( _name ); rlf != boxFunctionMapping.end() ) {
//        addBoxToViewport( _name,{ { _l, _r, _t, _b}, rlf->second } );
//        boxes[_name].setVisible( _bVisible );
//    }
}

void RenderOrchestrator::resizeCallback( const Vector2i& _resize ) {
    for ( auto& [k,v] : boxes ) {
        orBitWiseFlag( v.flags, BoxFlags::Resize );
        if ( getRig(k) ) {
            auto r = v.updateAndGetRect();
            rr.getTarget( k )->resize( r );
            sg.CM().get(k)->setViewport( r );
        }
    }
}

PickRayData RenderOrchestrator::rayViewportPickIntersection( const V2f& _screenPos ) const {
    return getCamera( Name::Foxtrot )->rayViewportPickIntersection( _screenPos );
}


//void RenderOrchestrator::changeMaterialColorTagImpl( const std::vector<std::string>& _params ) {
//    ColorBuilder{cl, concatParams(_params, 1)}.load(std::bind( &RenderOrchestrator::changeMaterialColorCallback,
//                                                               this,
//                                                               _params), _params);
//}
//
//void RenderOrchestrator::cmdRemoveGeometryImpl( const std::vector<std::string>& _params ) {
//    remove( _params[0] );
//}
//
//void RenderOrchestrator::updateImpl() {
//    am->update();
//}

AVInitCallback RenderOrchestrator::avcbTM() {
    return std::bind(&TextureManager::preparingStremingTexture,
                     rr.TM().get(),
                     std::placeholders::_1,
                     std::placeholders::_2);
}