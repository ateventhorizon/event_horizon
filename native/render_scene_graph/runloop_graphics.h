//
//  runloop.h
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#pragma once

#include <core/runloop_core.h>
#include <core/util.h>
#include <core/http/webclient.h>
#include <core/di.hpp>
#include <core/resources/resource_manager.hpp>
#include <core/state_machine_helper.hpp>
#include <core/command.hpp>
#include <graphics/window_handling.hpp>
#include <core/math/anim.h>
#include <poly/scene_graph.h>
#include <render_scene_graph/scene_bridge.h>
#include <render_scene_graph/render_orchestrator.h>

namespace di = boost::di;

class RunLoopBackEndBase {
public:
    RunLoopBackEndBase( SceneGraph& _sg, RenderOrchestrator& _rsg) : sg(_sg), rsg(_rsg) {
    }
    virtual ~RunLoopBackEndBase() = default;

    void update( AggregatedInputData& _aid ) {
        rsg.updateInputs( _aid );
        sg.update();
        updateImpl( _aid );
    };

    void activate() {
        sg.init();
        rsg.init();
//        lua.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io);
        activateImpl();
    }

    virtual void updateImpl( const AggregatedInputData& _aid ) = 0;
    virtual void activateImpl() = 0;

protected:
    SceneGraph& sg;
    RenderOrchestrator& rsg;
};

class RunLoopGraphics : public RunLoop {
public:
    using RunLoop::RunLoop;
	RunLoopGraphics( CommandQueue& _cq, Renderer& rr, TextInput& ti,
	                 MouseInput& mi, SceneGraph& _sg, RenderOrchestrator& _rsg )
                     : RunLoop( _cq ), rr( rr ), ti( ti), mi( mi), sg(_sg), rsg(_rsg) {}

    void setBackEnd( std::unique_ptr<RunLoopBackEndBase>&& _be ) {
        rlbackEnd = std::move(_be);
	}

    void init( InitializeWindowFlagsT _initFlags ) {
        WH::initializeWindow( _initFlags, rr );
        rr.init();
        rlbackEnd->activate();
        //	mi.subscribe( pm );
    }

    void run(){
        // This will be use for multithreading rendering
        // to be supported properly within wasm, atm out of the scope of current understanding :D
    }

    void updateLuaScript( const std::string& _lscript ) {
	    rsg.setLuaScriptHotReload( _lscript );
	}

    void singleThreadLoop() {

        update();
        render();

        nTicks++;
    }

    void runSingleThread() override {
        while ( !WH::shouldWindowBeClosed() ) {
            singleThreadLoop();
        }
        Http::shutDown();
    }


    void addScriptLine( const std::string& _cmd ) {
        cq.script( _cmd );
    }

    Renderer& RR() { return rr; }
    TextInput& TI() { return ti; }
	MouseInput& MI() { return mi; }

protected:
	void update() {
        updateTime();
        WH::preUpdate();
        mUpdateSignals.NeedsUpdate(false);
        Timeline::update();
        cq.execute();
        WH::pollEvents();
        mi.update( mUpdateSignals );
        auto aid = aggregateInputs();
        rlbackEnd->update( aid );
    }

    void render() {
        RR().directRenderLoop();
        WH::flush();
    }

    AggregatedInputData aggregateInputs() {
        return AggregatedInputData{ ti, mi.getScrollValue(), mi.Status() };
    }

protected:
    Renderer& rr;
    TextInput& ti;
    MouseInput& mi;
    SceneGraph& sg;
    RenderOrchestrator& rsg;
    std::unique_ptr<RunLoopBackEndBase> rlbackEnd;

	int nUpdates = 0;
	int nRenders = 0;
	int nTicks = 0;
	UpdateSignals mUpdateSignals;
};
