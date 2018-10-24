//
//  mouse_input.cpp
//

#include <graphics/imgui/imgui.h>
#include "mouse_input.hpp"
#include "window_handling.hpp"
#include "core/app_globals.h"
#include "camera_manager.h"

MouseInput::MouseInput() {
	mGesturesTaps.reserve( 100 );
}

void MouseInput::onTouchDown( const Vector2f& pos, UpdateSignals& _updateSignals ) {
	mMouseButtonStatus = MouseButtonStatusValues::DOWN;
	mGesturesTaps.clear();
	mGesturesTaps.push_back( pos );
	mGestureTime = 0.0f;
	mOverridedSwipe = false;
	mCanTriggerLongTap = true;
	mHasTouchedUp = false;
	mTouchedDown = true;
	mMoveDiff = Vector2f::ZERO;
	mRawTouchDownPos = pos;
	mNormTouchDownPos = pos / getScreenSizef;
	m3dUIPlaneTouchDownPos = mousePickOnUIPlane( pos, mUIPlane );
	m2dUIPlaneTouchDownPos = mousePickOnUIPlane( pos, mUIPlane, CameraProjectionType::Orthogonal );
    notify( *this, "OnTouchDown");
	_updateSignals.NeedsUpdate(true);
}

void MouseInput::onTouchMove( const Vector2f& pos, UpdateSignals& _updateSignals ) {
	mMouseButtonStatus = MouseButtonStatusValues::MOVING;
	if ( mGesturesTaps.size() == 0 ) {
		mGesturesTaps.push_back( pos );
	}
	if ( pos != mGesturesTaps.back() ) {
		Vector3f p1 = mousePickOnUIPlane( mGesturesTaps.back(), mUIPlane );
		Vector3f p2 = mousePickOnUIPlane( pos, mUIPlane );
		mMoveDiffMousePick = p2 - p1;

		mMoveDiff = pos - mGesturesTaps.back();
		mGesturesTaps.push_back( pos );
	} else {
		mMoveDiff = Vector2f::ZERO;
		mMoveDiffMousePick = Vector3f::ZERO;
	}

	if ( mGestureTime > MAX_SWIPE_TIME_LIMIT ) {
		checkSwipe();
	}
	mCanTriggerLongTap = checkLongTapDistance();
    notify( *this, "onTouchMove");
	_updateSignals.NeedsUpdate(true);
}

void MouseInput::onTouchMoveM( float* x, float *y, int size, UpdateSignals& _updateSignals ) {
	mMouseButtonStatus = MouseButtonStatusValues::MOVING;
	mCanTriggerLongTap = false;
    notify( *this, "onTouchMoveM");
	_updateSignals.NeedsUpdate(true);
}

void MouseInput::onTouchUp( const Vector2f& pos, UpdateSignals& _updateSignals ) {
	mMouseButtonStatus = MouseButtonStatusValues::UP;
	mGesturesTaps.push_back( pos );
	mCanTriggerLongTap = false;
	mHasTouchedUp = true;
	mTouchedDown = false;
	// Check if a single tap was performed
	mSingleTapEvent = false;
	if ( mGesturesTaps.size() > 0 && mGestureTime < SINGLE_TAP_TIME_LIMIT ) {
		Vector2f xyd = mGesturesTaps.back() - mGesturesTaps.front();
		if ( length( xyd ) < TAP_AREA ) {
			mSingleTapEvent = true;
			mSingleTapPos = mGesturesTaps.back();
		}
	}
	// Handle and check if double tap
	mDoubleTapEvent = false;
	mTouchupTimeStamps.push_back( mCurrTimeStamp );
	if ( mTouchupTimeStamps.size() > 1 ) {
		float time2 = mTouchupTimeStamps.back();
		float time1 = mTouchupTimeStamps[mTouchupTimeStamps.size() - 2];
		if ( time2 - time1 < DOUBLE_TAP_TIME_LIMIT ) {
			mDoubleTapEvent = true;
			mTouchupTimeStamps.clear();
		}
	}

	if ( mDoubleTapEvent ) {
		notify( *this, "onDoubleTap");
	} else if ( mSingleTapEvent ) {
		notify( *this, "onSingleTap");
	} else {
		notify( *this, "onTouchUp");
	}
	_updateSignals.NeedsUpdate(true);
}

void MouseInput::onScroll( float amount, UpdateSignals& _updateSignals ) {
	float scroll_dir = -1.0f;
#ifdef OSX
	scroll_dir *= -1.0f;
#endif
	mScrollValue = amount*scroll_dir;
	if ( amount != 0.0f ) {
		notify( *this, "onScroll");
		_updateSignals.NeedsUpdate(true);
	}
}

void MouseInput::clearTaps() {
	mGesturesTaps.clear();
}

void MouseInput::updateRectInput() {
	std::lock_guard<std::mutex> lock( mInputRectLock );
	if ( mIsRectInput && mGesturesTaps.size() > 1 && !mInputRects.empty() ) {
		Vector3f p1 = mousePickOnUIPlane( mGesturesTaps.back(), mUIPlane );
		Vector3f p2 = mousePickOnUIPlane( mGesturesTaps.front(), mUIPlane );
		mInputRects.back() = Rect2f{ {p1.xy(), p2.xy()} };
		mCanDrawRectInput = true;
	}
}

void MouseInput::pushNewRectInput() {
	std::lock_guard<std::mutex> lock( mInputRectLock );
	clearTaps();
	mInputRects.push_back( Rect2f::INVALID );
}

void MouseInput::ToggleRectInput() {
	mIsRectInput = !mIsRectInput;
	setupRectInputAfterStatusChange();
}

void MouseInput::setRectInput( const bool _val ) {
	if ( _val != mIsRectInput ) {
		ToggleRectInput();
	}
}

void MouseInput::setupRectInputAfterStatusChange() {
//	rr.CM().enableInputs( !mIsRectInput );
	mInputRects.clear();
	if ( mIsRectInput ) {
		pushNewRectInput();
	} else {
		clearTaps();
		mCanDrawRectInput = false;
	}
}

JMATH::Rect2f MouseInput::CurrRectInput() const {
	return mInputRects.size() > 0 ? mInputRects.back() : Rect2f::INVALID;
}

bool MouseInput::hasBeenTappedInRect( JMATH::Rect2f& rect ) {
	return mSingleTapEvent && rect.contains( mSingleTapPos );
}

SwipeDirection MouseInput::checkSwipe() {
	// Check Swipe
	if ( mOverridedSwipe ) {
		return SwipeDirection::INVALID;
	}

	Vector2f xyd = mGesturesTaps.back() - mGesturesTaps.front();

	const float SWIPE_ANGLE = 0.5f;
	const float SWIPE_LENGTH = 0.05f;

	if ( ( fabs( xyd.x() ) < SWIPE_ANGLE || fabs( xyd.y() ) < SWIPE_ANGLE ) && ( length( xyd ) > SWIPE_LENGTH ) ) {
		mOverridedSwipe = true;
		if ( fabs( xyd.x() ) > fabs( xyd.y() ) ) {
			if ( xyd.x() > 0.0f ) {
				return SwipeDirection::LEFT;
			} else {
				return SwipeDirection::RIGHT;
			}
		} else {
			if ( xyd.y() > 0.0f ) {
				return SwipeDirection::UP;
			} else {
				return SwipeDirection::DOWN;
			}
		}
	}
	return SwipeDirection::INVALID;
}

// Check Swipe on an arbitrary direction
// Return deviation from direction
float MouseInput::checkLinearSwipe( const vector2fList& targetSwipes, const vector2fList& playerSwipes ) {
	if ( mGesturesTaps.size() > 2 && mHasTouchedUp ) {
		float dist1 = distance( targetSwipes.front(), playerSwipes.front() );
		float dist2 = distance( targetSwipes.back(), playerSwipes.back() );

		float deviation = dist1 + dist2;
		return deviation;
	} else {
		return -1.0f;
	}
}

bool MouseInput::checkLongTapDistance() {
	// Check Swipe
	if ( !mCanTriggerLongTap ) {
		return false;
	}

	Vector2f xyd = mGesturesTaps.back() - mGesturesTaps.front();

	if ( length( xyd ) < TAP_AREA ) {
		return true;
	} else {
		return false;
	}
}

void MouseInput::setPaused( bool isPaused ) {
	mPaused = isPaused;
}

Vector2f MouseInput::getLastTap( YGestureInvert yInv /*= YGestureInvert::No*/ ) const {
	Vector2f pos = Vector2f::ZERO;
	if ( mGesturesTaps.size() > 0 ) {
		pos = mGesturesTaps.back();
		if ( yInv == YGestureInvert::Yes ) pos.invertY();
	}
	return pos;
}

Vector2f MouseInput::getCurrMoveDiff( YGestureInvert yInv ) const {
	Vector2f ret = mMoveDiff;
	if ( yInv == YGestureInvert::Yes ) ret.invertY();
	return  ret;
}

Vector2f MouseInput::getCurrMoveDiffNorm( YGestureInvert yInv ) const {
	return ( getCurrMoveDiff( yInv ) / getScreenSizef ) * getScreenAspectRatioVector;
}

Vector3f MouseInput::getCurrMoveDiffMousePick() const {
	return mMoveDiffMousePick;
}

Vector3f MouseInput::mousePickOnUIPlane( const Vector2f& mousePos ) const {
	return mousePickOnUIPlane( mousePos, mUIPlane );
}

Vector3f MouseInput::mousePickOnUIPlane( const Vector2f& mousePos, const Plane3f& uiPlane, CameraProjectionType pt ) const {
	if ( pt == CameraProjectionType::Perspective ) {
		// TODO Reimplement mousePickOnUIPlane on perspective camera
		return Vector3f::ZERO;
//		Vector3f nearP;
//		Vector3f farP;
//		rr.CM().getCamera()->mousePickRay( mousePos, nearP, farP );
//
//		return uiPlane.intersectLine( nearP, farP );
	} else {
		Vector2f posWithAspectRatio = ( mousePos * getScreenAspectRatioVector ) / getScreenSizef;
		return Vector3f( posWithAspectRatio, 0.0f );
	}
}

void MouseInput::setCursorType( MouseCursorType mct ) {
	switch ( mct ) {
		case MouseCursorType::ARROW:
		if ( mCursorArrow == nullptr ) mCursorArrow = glfwCreateStandardCursor( GLFW_ARROW_CURSOR );
		glfwSetCursor( WH::window, mCursorArrow );
		break;
		case MouseCursorType::HRESIZE:
		if ( mCursorHResize == nullptr ) mCursorHResize = glfwCreateStandardCursor( GLFW_HRESIZE_CURSOR );
		glfwSetCursor( WH::window, mCursorHResize );
		break;

		default:
		break;
	}
}

void MouseInput::accumulateArrowTouches( float direction ) {
	accumulatedArrowVelocity += ( mCurrTimeStep * direction ) / 10.0f;
	if ( accumulatedArrowVelocity > 0.1f ) accumulatedArrowVelocity = 0.1f;
}

void MouseInput::leftArrowPressed( const float speed ) {
	accumulateArrowTouches( -speed );
}

void MouseInput::rightArrowPressed( const float speed ) {
	accumulateArrowTouches( speed );
}

void MouseInput::update( const GameTime& gt, UpdateSignals& _updateSignals ) {
	mCurrTimeStamp = gt.mCurrTimeStamp;
	mCurrTimeStep = gt.mCurrTimeStep;
	mGestureTime += gt.mCurrTimeStep;
	accumulatedArrowVelocity *= 0.90f;
	mGestureTapsFront = Vector2f::ZERO;
	mGestureTapsBack = Vector2f::ZERO;
	if ( mGesturesTaps.size() > 0 ) {
		mGestureTapsFront = mGesturesTaps.front();
		mGestureTapsBack = mGesturesTaps.back();
	}

	if ( mCanTriggerLongTap && mGestureTime > LONG_TAP_TIME_LIMIT ) {
		mCanTriggerLongTap = false;
	}

	glfwGetCursorPos( WH::window, &xpos, &ypos );
//	if ( xpos != xposOld || ypos != yposOld ) {
		xposOld = xpos;
		yposOld = ypos;
#ifdef OSX
		xpos *= 2.0;
		ypos *= 2.0;
#endif
		ypos = ( getScreenSizef.y() - ypos );
//		_updateSignals.NeedsUpdate(true);
//	}

	ImGuiIO& io = ImGui::GetIO();

	if ( io.MouseDown[0] ) {
		if ( glfwMousePressLeftTick == 0 ) {
			onTouchDown( { xpos, ypos }, _updateSignals );
		} else {
			onTouchMove( { xpos, ypos }, _updateSignals );
		}
		++glfwMousePressLeftTick;
	} else {
		if ( glfwMousePressLeftTick != 0 ) {
			onTouchUp( { xpos, ypos }, _updateSignals );
		}
		glfwMousePressLeftTick = 0;
	}

	//io.MouseDown[1] = true;
	onScroll( GScrollData.y() != io.MouseWheel ? io.MouseWheel : 0.0f, _updateSignals );
	GScrollData = { io.MouseWheelH, io.MouseWheel };
}

void MouseInput::enableMouseCursor( const bool val ) {
	int cursor = val ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
	glfwSetInputMode( WH::window, GLFW_CURSOR, cursor );
}

Vector2f MouseInput::getCurrPos() const {
	return Vector2f{ xpos, ypos };
}

Vector2f MouseInput::getCurrPosSS() const {
	return Vector2f{ xpos, getScreenSizef.y() - ypos };
}