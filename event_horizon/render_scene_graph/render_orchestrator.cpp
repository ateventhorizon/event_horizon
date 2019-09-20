//
// Created by Dado on 08/02/2018.
//

#include "render_orchestrator.h"
#include <core/resources/resource_utils.hpp>
#include <core/math/vector_util.hpp>
#include <core/raw_image.h>
#include <core/camera.h>
#include <core/geom.hpp>
#include <core/image_mapping.hpp>
#include <core/image_params.hpp>
#include <core/TTF.h>
#include <core/font_params.hpp>
#include <core/resources/material.h>
#include <core/resources/ui_container.hpp>
#include <graphics/renderer.h>
#include <graphics/shader_manager.h>
#include <graphics/vp_builder.hpp>
#include <graphics/window_handling.hpp>
#include <render_scene_graph/render_orchestrator_callbacks.hpp>
#include <render_scene_graph/lua_scripts.hpp>

static const inline std::string colorScheme = "\n"
                                              "#####  Color Palette by Paletton.com\n"
                                              "#####  Palette URL: http://paletton.com/#uid=73L2m0krkqlryQxn2CRsUccmrc7\n"
                                              "\n"
                                              "\n"
                                              "*** Primary color:\n"
                                              "\n"
                                              "   shade 0 = #21428D = rgb( 33, 66,141) = rgba( 33, 66,141,1) = rgb0(0.129,0.259,0.553)\n"
                                              "   shade 1 = #3469E3 = rgb( 52,105,227) = rgba( 52,105,227,1) = rgb0(0.204,0.412,0.89)\n"
                                              "   shade 2 = #4469BF = rgb( 68,105,191) = rgba( 68,105,191,1) = rgb0(0.267,0.412,0.749)\n"
                                              "   shade 3 = #0C1C41 = rgb( 12, 28, 65) = rgba( 12, 28, 65,1) = rgb0(0.047,0.11,0.255)\n"
                                              "   shade 4 = #182441 = rgb( 24, 36, 65) = rgba( 24, 36, 65,1) = rgb0(0.094,0.141,0.255)\n"
                                              "\n"
                                              "*** Secondary color (1):\n"
                                              "\n"
                                              "   shade 0 = #D2701F = rgb(210,112, 31) = rgba(210,112, 31,1) = rgb0(0.824,0.439,0.122)\n"
                                              "   shade 1 = #FF8723 = rgb(255,135, 35) = rgba(255,135, 35,1) = rgb0(1,0.529,0.137)\n"
                                              "   shade 2 = #FF9B47 = rgb(255,155, 71) = rgba(255,155, 71,1) = rgb0(1,0.608,0.278)\n"
                                              "   shade 3 = #613109 = rgb( 97, 49,  9) = rgba( 97, 49,  9,1) = rgb0(0.38,0.192,0.035)\n"
                                              "   shade 4 = #613C1D = rgb( 97, 60, 29) = rgba( 97, 60, 29,1) = rgb0(0.38,0.235,0.114)\n"
                                              "\n"
                                              "*** Secondary color (2):\n"
                                              "\n"
                                              "   shade 0 = #127E7E = rgb( 18,126,126) = rgba( 18,126,126,1) = rgb0(0.071,0.494,0.494)\n"
                                              "   shade 1 = #1FDEDE = rgb( 31,222,222) = rgba( 31,222,222,1) = rgb0(0.122,0.871,0.871)\n"
                                              "   shade 2 = #32B2B2 = rgb( 50,178,178) = rgba( 50,178,178,1) = rgb0(0.196,0.698,0.698)\n"
                                              "   shade 3 = #063A3A = rgb(  6, 58, 58) = rgba(  6, 58, 58,1) = rgb0(0.024,0.227,0.227)\n"
                                              "   shade 4 = #113A3A = rgb( 17, 58, 58) = rgba( 17, 58, 58,1) = rgb0(0.067,0.227,0.227)\n"
                                              "\n"
                                              "*** Complement color:\n"
                                              "\n"
                                              "   shade 0 = #D2961F = rgb(210,150, 31) = rgba(210,150, 31,1) = rgb0(0.824,0.588,0.122)\n"
                                              "   shade 1 = #FFB623 = rgb(255,182, 35) = rgba(255,182, 35,1) = rgb0(1,0.714,0.137)\n"
                                              "   shade 2 = #FFC247 = rgb(255,194, 71) = rgba(255,194, 71,1) = rgb0(1,0.761,0.278)\n"
                                              "   shade 3 = #614409 = rgb( 97, 68,  9) = rgba( 97, 68,  9,1) = rgb0(0.38,0.267,0.035)\n"
                                              "   shade 4 = #614A1D = rgb( 97, 74, 29) = rgba( 97, 74, 29,1) = rgb0(0.38,0.29,0.114)\n"
                                              "\n"
                                              "\n"
                                              "#####  Generated by Paletton.com (c) 2002-2014";

std::vector<std::string> RenderOrchestrator::callbackPaths;
std::vector<PresenterUpdateCallbackFunc> RenderOrchestrator::sUpdateCallbacks;

#ifdef USE_GLFW

Vector2i ResizeData::callbackResizeFrameBuffer = Vector2i::ZERO;
Vector2i ResizeData::callbackResizeFrameBufferOld = Vector2i::ZERO;

#endif

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

    if ( dragAndDropFunc ) dragAndDropFunc( callbackPaths );

    resizeCallbacks();
}

template <typename T>
void setMatProperty( std::vector<std::shared_ptr<VPList>>& vList, std::shared_ptr<RenderMaterial> mat,
                     const std::string& _uniform, const T& _value ) {
    for ( const auto& v : vList ) {
        if ( v->getMaterial() == mat ) v->getMaterial()->setConstant( _uniform, _value );
    }
}

template <typename F, typename T, typename ...Args>
void foreachCL( CommandBufferListVectorMap& CL, F func, const T& _value, Args ...args ) {
    for ( auto&[k, vl] : CL ) {
        vl.foreach( func, std::forward<Args>( args )..., _value );
    }
}

RenderOrchestrator::RenderOrchestrator( Renderer& rr, SceneGraph& _sg ) : rr( rr ), sg(_sg), uiView(_sg, colorScheme) {

    sg.preloadCompleteConnect( [this]( ConnectVoidParamSig _value ) {
        this->RR().setLoadingFlag(!_value );
    });

    sg.propagateDirtyFlagConnect( [this]( ConnectPairStringBoolParamSig _value ) {
        setDirtyFlagOnPBRRender( Name::Foxtrot, _value.first, _value.second );
    });

    sg.FM().connect( [](const ResourceTransfer<Font>& _val ) {
        LOGRS( "[SG-Resource] Add " << ResourceVersioning<Font>::Prefix() << ": "  << *_val.names.begin() );
        if ( _val.ccf ) _val.ccf(_val.hash);
    });

    sg.UM().connect( [](const ResourceTransfer<UIContainer>& _val ) {
        LOGRS( "[SG-Resource] Add " << ResourceVersioning<UIContainer>::Prefix() << ": "  << *_val.names.begin() );
        if ( _val.ccf ) _val.ccf(_val.hash);
    });

    sg.PL().connect( [](const ResourceTransfer<Profile>& _val ) {
        LOGRS( "[SG-Resource] Add " << ResourceVersioning<Profile>::Prefix() << ": "  << *_val.names.begin() );
        if ( _val.ccf ) _val.ccf(_val.hash);
    });

    sg.TL().connect( [this](const ResourceTransfer<RawImage>& _val ) {
        LOGRS( "[SG-Resource] Add " << ResourceVersioning<RawImage>::Prefix() << ": "  << *_val.names.begin() );
        this->RR().addTextureResource(_val);
        if ( _val.ccf ) _val.ccf(_val.hash);
    });

    sg.ML().connect( [this](const ResourceTransfer<Material>& _val ) {
        LOGRS( "[SG-Resource] Add " << ResourceVersioning<Material>::Prefix() << ": "  << *_val.names.begin() );
        this->RR().addMaterialResource(_val);
        if ( _val.ccf ) _val.ccf(_val.hash);
    });

    sg.VL().connect( [this](const ResourceTransfer<VData>& _val ) {
        LOGRS( "[SG-Resource] Add " << ResourceVersioning<VData>::Prefix() << ": "  << *_val.names.begin() );
        this->RR().addVDataResource(_val);
        if ( _val.ccf ) _val.ccf(_val.hash);
    });

    sg.GM().connect( [](const ResourceTransfer<Geom>& _val ) {
        LOGRS( "[SG-Resource] Add " << ResourceVersioning<VData>::Prefix() << ": "  << *_val.names.begin() );
        if ( _val.ccf ) _val.ccf(_val.hash);
    });

    sg.nodeAddConnect( [this]( NodeGraphConnectParamsSig _geom ) {
        auto bEmpty = _geom->empty();
        LOGRS( "[SG-Node] Add " << (bEmpty ? "Root " : "") << _geom->Name() );
        if ( bEmpty ) return;
        auto dataRef = _geom->DataRef(0);
        auto vp = VPBuilder<PosTexNorTanBinUV2Col3dStrip>{ this->RR(), dataRef.material, dataRef.vData}.
                  n(_geom->UUiD()).
                  g(_geom->Tag()).
                  t(_geom->getLocalHierTransform()).
                  b(_geom->BBox3d()).
                  build();
        this->RR().VPL( CommandBufferLimits::PBRStart, vp);
        this->RR().invalidateOnAdd();
    });

    sg.nodeRemoveConnect( [this]( NodeGraphConnectParamsSig _geom ) {
        this->RR().clearBucket( CommandBufferLimits::UI2dStart );
        this->RR().clearBucket( CommandBufferLimits::PBRStart );
        setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
    });

    sg.nodeFullScreenImageConnect( [this](CResourceRef _node) {
        auto image = sg.TL( _node );
        V2f iar = image->getAspectRatioV();
        auto fit = getFullScreenAspectFit( iar.ratio() );
        this->RR().drawRect2d( CommandBufferLimits::UI2dStart, fit.first, fit.second, _node );
        Socket::send( "wasmClientFinishedLoadingData", image->serializeParams() );
    });

    sg.nodeFullScreenUIContainerConnect( [this](CResourceRef _node) {
        auto ui = sg.UL( _node );
        Socket::send( "wasmClientFinishedLoadingData", *ui.get() );
    });

    sg.nodeFullScreenFontSonnetConnect( [this](CResourceRef _node) {
        auto pfont = sg.FM( _node );
        std::string message = "Roads? Where we're going, we don't need roads";
        std::vector<std::pair<float,C4f>> fsize{
                {0.02f, C4f::LIGHT_GREY},
                 {0.03f, C4f::DARK_BLUE},
                  {0.04f, C4f::DARK_CYAN},
                   {0.05f, C4f::DARK_YELLOW},
                    {0.07f, C4f::DARK_PURPLE},
                     {0.10f, C4f::DARK_RED},
                      {0.12f, C4f::DARK_SALMON},
                       {0.15f, C4f::DARK_GREEN},
                        {0.18f, C4f::DARK_BROWN},
            };
        float fOff = 0.0f;
        for ( auto t = 0; t < fsize.size(); t++ ) {
            this->RR().draw<DText2d>( CommandBufferLimits::UI2dStart,FDS{message, pfont.get(), V2f{0.02f, 0.98f - fOff}, fsize[t].first }, fsize[t].second );
            fOff += fsize[t].first + fsize[t].first*0.25f;
        }
        Socket::send( "wasmClientFinishedLoadingData", pfont->serializeParams() );
    });

    sg.replaceMaterialConnect( [this]( const std::string& _oldMatRef , const std::string& _newMatRef ) {
        this->RR().replaceMaterial( _oldMatRef, _newMatRef );
        setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
    });

    sg.changeMaterialPropertyConnectString( [this]( const std::string& _prop, const std::string& _key,
                                                 const std::string& _value ) {
        foreachCL( this->RR().CL(), setMatProperty<decltype(_value)>, _value,
                   this->RR().getRenderMaterialFromHash( _key ), _prop );
        setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
    });

    sg.changeMaterialPropertyConnectFloat( [this]( const std::string& _prop, const std::string& _key,
                                                 const float& _value ) {
        foreachCL( this->RR().CL(), setMatProperty<decltype(_value)>, _value,
                   this->RR().getRenderMaterialFromHash( _key ), _prop );
        setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
    });

    sg.changeMaterialPropertyConnectV3f( [this]( const std::string& _prop, const std::string& _key,
                                              const V3f& _value ) {
        foreachCL( this->RR().CL(), setMatProperty<decltype(_value)>, _value,
                this->RR().getRenderMaterialFromHash( _key ), _prop );
        setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
    });

    sg.changeMaterialPropertyConnectV4f( [this]( const std::string& _prop, const std::string& _key,
                                                 const V4f& _value ) {
        foreachCL( this->RR().CL(), setMatProperty<decltype(_value)>, _value,
                   this->RR().getRenderMaterialFromHash( _key ), _prop );
        setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
    });

}

//#define LUA_HARDCODED_DEBUG

void RenderOrchestrator::luaUpdate( const AggregatedInputData& _aid ) {

#ifdef     LUA_HARDCODED_DEBUG
    lua.script( R"(

function update(aid)
  if ( aid:isMouseSingleTap(0) == true ) then
    print("Z")
    end
  end

)");
#else
    if ( auto luaScript = getLuaScriptHotReload(); !luaScript.empty() ) {
        try {
            lua.safe_script( luaScript );
        } catch (const std::exception& e) { // caught by reference to base
            std::cout << " a standard exception was caught, with message '"
                      << e.what() << "'\n";
        } catch (...) {
            std::cout << "... Except not handled";
        }

        setLuaScriptHotReload("");
    }
#endif
    // Call update every frame
    try {
        sol::protected_function updateFunction = lua["update"];
        if ( updateFunction ) {
            updateFunction(_aid);
        }
    } catch (const std::exception& e) { // caught by reference to base
        std::cout << " a standard exception was caught, with message '"
                  << e.what() << "'\n";
    } catch (...) {
        std::cout << "... Except not handled";
    }
}

void RenderOrchestrator::updateInputs( const AggregatedInputData& _aid ) {
    luaUpdate(_aid);
    updateCallbacks();

    for ( auto& [k,v] : mRigs ) {
        v->updateFromInputData( _aid );
    }
}

void RenderOrchestrator::init() {
    initWHCallbacks();

    lua.open_libraries( sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table
            , sol::lib::debug, sol::lib::bit32, sol::lib::io, sol::lib::ffi);

    auto v2fLua = lua.new_usertype<Vector2f>("V2f",
                                          sol::constructors<>());
    v2fLua["x"] = &Vector2f::x;
    v2fLua["y"] = &Vector2f::y;

    auto aid = lua.new_usertype<AggregatedInputData>("AggregatedInputData",
            sol::constructors<>() );

    aid["scrollValue"] = &AggregatedInputData::scrollValue;
    aid.set("isMouseTouchedDownFirstTime", sol::readonly(&AggregatedInputData::isMouseTouchedDownFirstTime));
    aid.set("isMouseTouchedUp", sol::readonly(&AggregatedInputData::isMouseTouchedUp));
    aid.set("isMouseTouchedDown", sol::readonly(&AggregatedInputData::isMouseTouchedDown));
    aid.set("isMouseSingleTap", sol::readonly(&AggregatedInputData::isMouseSingleTap));
    aid.set("hasMouseMoved", sol::readonly(&AggregatedInputData::hasMouseMoved));

    aid.set("mousePos", sol::readonly(&AggregatedInputData::mousePos));
    aid.set("moveDiffSS", sol::readonly(&AggregatedInputData::moveDiffSS));
    aid.set("moveDiff", sol::readonly(&AggregatedInputData::moveDiff));
    aid.set("checkKeyToggleOn", sol::readonly(&AggregatedInputData::checkKeyToggleOn));

    // State machine require
    lua.require_script("statemachine", luaStateMachine);

    auto luarr = lua["rr"].get_or_create<sol::table>();

    luarr["clearColor"] = [&](const std::string& _col ) {
        Renderer::clearColor( V4f::XTORGBA(_col) );
        setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
    };
    luarr["useSkybox"] = [&](bool _flag) {
        useSkybox(_flag);
        setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
    };

    luarr["useVignette"] = [&](bool _flag) {
        rr.SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine", "_VIGNETTING_", boolAlphaBinary(_flag) );
        setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
    };

    luarr["useFilmGrain"] = [&](bool _flag) {
        rr.SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine",
                                     "_GRAINING_", boolAlphaBinary(_flag) );
        setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
    };

    luarr["useBloom"] = [&](bool _flag) {
        rr.SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine",
                               "_BLOOMING_", boolAlphaBinary(_flag) );
        setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
    };

    luarr["useDOF"] = [&](bool _flag) {
        useDOF( _flag );
    };

    luarr["useSSAO"] = [&](bool _flag) {
        useSSAO(_flag);
        rr.SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine",
                               "_SSAOING_", boolAlphaBinary(_flag) );
        setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
    };

#ifndef _PRODUCTION_
    Socket::on( "shaderchange",
                std::bind(&RenderOrchestrator::reloadShaders, this, std::placeholders::_1, std::placeholders::_2 ) );
#endif

    // Set a fullscreen camera by default
    addRig<CameraControlFly>( Name::Foxtrot, 0.0f, 1.0f, 0.0f, 1.0f );
}

void RenderOrchestrator::reloadShaders( const std::string& _msg, SocketCallbackDataType&& _data ) {

    ShaderLiveUpdateMap shadersToUpdate{std::move(_data)};

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

void RenderOrchestrator::setDirtyFlagOnPBRRender( const std::string& _target, const std::string& _sub, bool _flag ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->setDirty( _sub, _flag );
    }
}

void RenderOrchestrator::clearPBRRender( const std::string& _target ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->clearCB();
    }
}

void RenderOrchestrator::hidePBRRender( const std::string& _target ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->enableBucket( false );
    }
}

void RenderOrchestrator::showPBRRender( const std::string& _target ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->enableBucket( true );
    }
}

void RenderOrchestrator::createSkybox( const SkyBoxInitParams& _skyboxParams ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->createSkybox( _skyboxParams );
    }
}

void RenderOrchestrator::changeTime( const std::string& _time ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->changeTime( _time );
    }
}

void RenderOrchestrator::useSkybox( bool _value ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->enableSkybox( _value );
    }
}

void RenderOrchestrator::useSSAO( bool _value ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->useSSAO( _value );
    }
    rr.useSSAO(_value);
    setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
}

void RenderOrchestrator::useDOF( bool _value ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->useDOF( _value );
    }
    rr.useDOF(_value);
    setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
}

void RenderOrchestrator::useMotionBlur( bool _value ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->useMotionBlur( _value );
    }
    rr.useMotionBlur(_value);
    setDirtyFlagOnPBRRender( Name::Foxtrot, S::PBR, true );
}

floata& RenderOrchestrator::skyBoxDeltaInterpolation() {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        return pbrTarget->skyBoxDeltaInterpolation();
    }
    ASSERT("Target doesnt have skybox!");
    static floata reterror;
    return reterror;
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
    LOGR("ResizeCallbackViewports Start");
    for ( auto& [k,v] : boxes ) {
        LOGRS("Resizing " << k);
        orBitWiseFlag( v.flags, BoxFlags::Resize );
        if ( getRig(k) ) {
            LOGRS("Resizing Rig " << k);
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

void RenderOrchestrator::setVisible( uint64_t _cbIndex, bool _value ) {
    if ( auto pbrTarget = dynamic_cast<RLTargetPBR*>( rr.getTarget( Name::Foxtrot ).get() ); pbrTarget ) {
        pbrTarget->setVisibleCB( _cbIndex, _value );
    }
}

SceneGraph& RenderOrchestrator::SG() { return sg; }

UIView& RenderOrchestrator::UI() { return uiView; }

UICallbackMap& RenderOrchestrator::UICB() { return uiView.Callbacks(); }

void RenderOrchestrator::addUIContainer( const MPos2d& _at, CResourceRef _res, UIElementStatus _initialStatus ) {
    UIViewContainer container{ *this, _res, SG().get<UIContainer>( _res ).get() };
    uiView.add( _at, container, _initialStatus);
}

