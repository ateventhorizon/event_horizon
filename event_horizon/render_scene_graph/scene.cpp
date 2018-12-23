//
//  Scene
//
//

#include "scene.hpp"

#include "graphics/ui/ui_control_manager.hpp"
#include "graphics/camera_manager.h"
#include "graphics/renderer.h"
#include "graphics/render_list.h"
#include "graphics/window_handling.hpp"
#include "graphics/ui/imgui_console.h"

#include <render_scene_graph/scene_layout.h>
#include <render_scene_graph/render_scene_graph.h>

#include <stb/stb_image.h>

#include "di_modules.h"

std::vector<std::string> Scene::callbackPaths;
Vector2i Scene::callbackResizeWindow = Vector2i(-1, -1);
Vector2i Scene::callbackResizeFrameBuffer = Vector2i(-1, -1);
std::vector<PresenterUpdateCallbackFunc> Scene::sUpdateCallbacks;

class CommandScriptPresenterManager : public CommandScript {
public:
	explicit CommandScriptPresenterManager( Scene& hm );
	virtual ~CommandScriptPresenterManager() = default;
};

CommandScriptPresenterManager::CommandScriptPresenterManager( Scene& _hm ) {
    addCommandDefinition("enable keyboard", std::bind(&Scene::cmdEnableKeyboard, &_hm, std::placeholders::_1));
    addCommandDefinition("disable keyboard", std::bind(&Scene::cmdDisableKeyboard, &_hm, std::placeholders::_1));
}

void GDropCallback( [[maybe_unused]] GLFWwindow *window, int count, const char **paths ) {
	ASSERT( count > 0 );

	Scene::callbackPaths.clear();
	for ( auto i = 0; i < count; i++ ) {
		Scene::callbackPaths.emplace_back( std::string(paths[i]) );
	}
}

void GResizeWindowCallback( [[maybe_unused]] GLFWwindow *, int w, int h ) {
	Scene::callbackResizeWindow = Vector2i{w, h};
}

void GResizeFramebufferCallback( [[maybe_unused]] GLFWwindow *, int w, int h ) {
	Scene::callbackResizeFrameBuffer = Vector2i{w, h};
}

Scene::Scene( Renderer& _rr, RenderSceneGraph& _rsg, UiControlManager& _uicm, TextInput& ti, MouseInput& mi,
						  CameraManager& cm, CommandQueue& cq ) :
		cm(cm), rr(_rr), rsg(_rsg), uicm( _uicm ), ti( ti), mi( mi ), cq( cq) {
	hcs = std::make_shared<CommandScriptPresenterManager>(*this);
	cq.registerCommandScript(hcs);
	console = std::make_shared<ImGuiConsole>(cq);
}

void Scene::updateCallbacks() {

	if ( !sUpdateCallbacks.empty() ) {
		for ( auto& c : sUpdateCallbacks ) {
			c( this );
		}
		sUpdateCallbacks.clear();
	}

	if ( !callbackPaths.empty() ) {
		for ( auto& path : callbackPaths ) {
			if ( layout->dragAndDropFunc ) layout->dragAndDropFunc( this, path );
		}
		callbackPaths.clear();
	}

	if ( callbackResizeWindow.x() > 0 && callbackResizeWindow.y() > 0 ) {
		LOGR("Resized window: [%d, %d]", callbackResizeWindow.x(), callbackResizeWindow.y() );
		callbackResizeWindow = Vector2i{-1, -1};
	}

	if ( callbackResizeFrameBuffer.x() > 0 && callbackResizeFrameBuffer.y() > 0 ) {
		WH::gatherMainScreenInfo();
		RR().resetDefaultFB();
		layout->resizeCallback( this, callbackResizeFrameBuffer );
		LOGR("Resized framebuffer: [%d, %d]", callbackResizeFrameBuffer.x(), callbackResizeFrameBuffer.y() );
		callbackResizeFrameBuffer = Vector2i{-1, -1};
	}

}

void Scene::update() {

	if ( !activated() ) {
		activate();
		if ( postActivateFunc ) postActivateFunc( this );
	}

	if ( !mbActivated ) return;

	inputPollUpdate();
	updateCallbacks();
}

void Scene::deactivate() {
}

void Scene::activate() {

	WH::setDropCallback( GDropCallback );
	WH::setResizeWindowCallback( GResizeWindowCallback );
	WH::setResizeFramebufferCallback( GResizeFramebufferCallback );

    //stbi_set_flip_vertically_on_load(true);

	Socket::on( "shaderchange",
				std::bind(&Scene::reloadShaders, this, std::placeholders::_1 ) );

	MaterialBuilder{"white"}.makeDefault(rsg.ML());
	ImageBuilder{"white"}.makeDirect( rsg.TL(), RawImage::WHITE4x4() );

	CQ().script("change time 14:00");
	layout->activate( this );

	if ( const auto& it = eventFunctions.find( PresenterEventFunctionKey::Activate); it != eventFunctions.end() ) {
		it->second(this);
	}
	mbActivated = true;
}

void Scene::reloadShaders( const std::string& _data ) {
	auto commands = split( _data, " " );

	if ( commands.size() >= 3 && commands.size() % 2 == 1 ) {
		for ( size_t si = 1; si < commands.size(); si += 2 ) {
			rr.injectShader( commands[si], commands[si+1] );
		}
		cq.script( "reload shaders" );
	}
}

void Scene::enableInputs( bool _bEnabled ) {
	ti.setEnabled( _bEnabled );
}

void Scene::inputPollUpdate() {

	ViewportTogglesT cvtTggles = ViewportToggles::None;
	// Keyboards
	if ( ti.checkKeyToggleOn( GMK_1 ) ) cvtTggles |= ViewportToggles::DrawWireframe;
    if ( ti.checkKeyToggleOn( GMK_G ) ) cvtTggles |= ViewportToggles::DrawGrid;

	static float camVelocity = 1.000f;
	static float accumulatedVelocity = .0003f;
	float moveForward = 0.0f;
	float strafe = 0.0f;
	float moveUp = 0.0f;

	if ( ti.checkWASDPressed() != -1 ) {
		float vel = 0.003f*GameTime::getCurrTimeStep();
		camVelocity = vel + accumulatedVelocity;
		if ( ti.checkKeyPressed( GMK_W ) ) moveForward = camVelocity;
		if ( ti.checkKeyPressed( GMK_S ) ) moveForward = -camVelocity;
		if ( ti.checkKeyPressed( GMK_A ) ) strafe = camVelocity;
		if ( ti.checkKeyPressed( GMK_D ) ) strafe = -camVelocity;
		if ( ti.checkKeyPressed( GMK_R ) ) moveUp = -camVelocity;
		if ( ti.checkKeyPressed( GMK_F ) ) moveUp = camVelocity;
		accumulatedVelocity += GameTime::getCurrTimeStep()*0.025f;
		if ( camVelocity > 3.50f ) camVelocity = 3.50f;
	} else {
		accumulatedVelocity = 0.0003f;
	}

	cm.updateFromInputData( { cvtTggles,
						      mi.getCurrPos(),
	                          mi.isTouchedDown(),
						      mi.getScrollValue(),
							  mi.getCurrMoveDiff( YGestureInvert::No ).dominant()*0.01f,
							  mi.getCurrMoveDiffNorm().dominant(),
							  moveForward, strafe, moveUp} );
    cm.update();
}

bool Scene::checkKeyPressed( int keyCode ) {
	return ti.checkKeyPressed( keyCode );
}

void Scene::notified( MouseInput& _source, const std::string& generator ) {

	if ( generator == "onTouchUp" ) {
		onTouchUpImpl( mi.getCurrPosSS(), ti.mModKeyCurrent );
	} else
	if ( generator == "onSingleTap" ) {
		onSimpleTapImpl( mi.getCurrPosSS(), ti.mModKeyCurrent );
	} else
	if ( generator == "onDoubleTap" ) {
		onDoubleTapImpl( mi.getCurrPosSS(), ti.mModKeyCurrent );
	}
	//LOGR( generator.c_str() );
}

void Scene::render() {
	layout->render( this );
}

void Scene::addUpdateCallback( PresenterUpdateCallbackFunc uc ) {
	sUpdateCallbacks.push_back( uc );
}

const std::string Scene::DC() {
	return Name::Foxtrot;
}

void Scene::cmdEnableKeyboard( const std::vector<std::string>& params ) {
    WH::enableInputCallbacks();
}

void Scene::cmdDisableKeyboard( const std::vector<std::string>& params ) {
    WH::disableInputCallbacks();
}

void Scene::addEventFunction( const std::string& _key, std::function<void(Scene*)> _f ) {
	eventFunctions[_key] = _f;
}

void Scene::Layout( std::shared_ptr<SceneLayout> _l ) {
	layout = _l;
}

InitializeWindowFlagsT Scene::getLayoutInitFlags() const {
	return layout->getInitFlags();
}

const std::shared_ptr<ImGuiConsole>& Scene::Console() const {
	return console;
}

void Scene::takeScreenShot( const JMATH::AABB& _box, ScreenShotContainerPtr _outdata ) {
    addViewport<RLTargetPBR>( Name::Sierra, Rect2f( Vector2f::ZERO, Vector2f{128.0f} ), BlitType::OffScreen );
    getCamera(Name::Sierra)->center(_box);
    rr.getTarget(Name::Sierra)->takeScreenShot( _outdata );
}

RenderSceneGraph& Scene::RSG() { return rsg; }

MaterialManager& Scene::ML() { return rsg.ML(); }

Renderer& Scene::RR() { return rr; }

CameraManager& Scene::CM() { return cm; }

TextureManager& Scene::TM() { return rr.TM(); }

CommandQueue& Scene::CQ() { return cq; }

std::shared_ptr<Camera> Scene::getCamera( const std::string& _name ) { return CM().getCamera(_name); }


