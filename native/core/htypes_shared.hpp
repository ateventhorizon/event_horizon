//
//  htypes.hpp
//  sixthview
//
//  Created by Dado on 01/10/2015.
//
//

#pragma once

#include <cstdint>
#include <type_traits>
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>

using HashEH = int64_t;

using CommandArgumentsT 		= std::vector< std::string >;
using CommandCallbackFunction 	= std::function<void(const CommandArgumentsT& )>;
using SerializableContainer     = std::vector<unsigned char>;
using SerializableContainerDict = std::unordered_map<std::string, SerializableContainer>;
using KVStringMap               = std::unordered_map<std::string, std::string>;
using KVWStringMap              = std::unordered_map<std::string, std::wstring>;
using StringsPair               = std::pair<std::string, std::string>;
using SetOfStringPair           = std::set<StringsPair>;
using DependencyList            = std::set<StringsPair>;
using GenericCallback           = std::function<void()>;

typedef std::pair<void*, uint64_t> void_p;
typedef std::pair<char*, uint64_t> char_p;
typedef std::pair<unsigned char*, uint64_t> uchar_p;
typedef std::pair<const unsigned char*, uint64_t> ucchar_p;
typedef std::pair<std::unique_ptr<uint8_t[]>, uint64_t> uint8_p;

typedef std::pair<int32_t, int32_t> IndexPair;

struct HashIndexPairU32 {
    std::string hash;
    uint32_t    index;
};

using StringUniqueCollection = std::unordered_set<std::string>;

enum class TextFormatting {
	Left,
	Center,
	Right
};

enum class SDVTouch {
	NONE = 0,
	TITLE,
	NAME,
	DATA
};

struct WindingOrder {
	static const uint64_t CW = 0;
	static const uint64_t CCW = 1;
};

enum class ReverseFlag {
	False = 0,
	True = 1
};

using WindingOrderT = uint64_t;

enum class PivotPointPosition {
	Center,

	BottomCenter,
	TopCenter,
	LeftCenter,
	RightCenter,
	FrontCenter,

	BottomRight,
	BottomLeft,
	TopLeft,
	TopRight,

	Custom,
	Invalid
};

using PPP = PivotPointPosition;

enum class PillowEdges {
	None,
	All,
	SidesHorizontal,
	SidesVertical,
	Top,
	TopAndSides,
	Bottom,
	BottomAndSides,
	Left,
	Right
};

enum class WrapRotations {
	WR_ON,
	WR_OFF
};

enum class MappingDirection {
	X_POS = 0,
	Y_POS = 1
};

enum class MappingMirrorE : int {
	None = 0,
	X_Only = 1,
	Y_Only = 2,
	BothWays = 3
};

enum class RectFillMode {
    Scale,
    AspectFit,
    AspectFill,
    AspectFitLeft,
    AspectFitRight,
    AspectFitTop,
    AspectFitBottom,
};

enum UiControlFlag {
	NoFlags = 0,
	UseScreenSpace = 1 << 0,
	HasRoundedCorners = 1 << 1,
	TextAlignCenter = 1 << 2,
	TextAlignLeft = 1 << 3,
	TextAlignRight = 1 << 4,
	HasShadow = 1 << 5,
	FillScale = 1 << 6,
	FloatLeft = 1 << 7,
	FloatRight = 1 << 8,
	FloatCenter = 1 << 9,
	Use3d = 1 << 10,
	IsArray = 1 << 11,
	Invalid = 1 << 31
};

typedef float subdivisionAccuracy;

[[maybe_unused]] const static float accuracy1Meter = 1.0f;
[[maybe_unused]] const static float accuracy50cm = .5f;
[[maybe_unused]] const static float accuracy25cm = .25f;
[[maybe_unused]] const static float accuracy10cm = .1f;
[[maybe_unused]] const static float accuracy5cm = .05f;
[[maybe_unused]] const static float accuracy1cm = .01f;
[[maybe_unused]] const static float accuracy1mm = .001f;
[[maybe_unused]] const static float accuracy1Sqmm = 0.000001f;
[[maybe_unused]] const static float accuracyNone = 0.0f;

enum class FollowerSide {
	In = -1,
	Out = 1,
	Center = 0
};

enum class FollowerQuadAlignment {
	Center,
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
};

enum FollowerFlags {
	Defaults = 0,
	NoOverRide = 0,
	WrapPath = 1,
	NoCaps = 1 << 1,
	UsePlanarMapping = 1 << 2
	//	StraightAngles =			1 << 1,
};

using T = std::underlying_type_t <FollowerFlags>;

inline FollowerFlags operator | ( FollowerFlags lhs, FollowerFlags rhs ) {
	return (FollowerFlags)( static_cast<T>( lhs ) | static_cast<T>( rhs ) );
}

inline FollowerFlags& operator |= ( FollowerFlags& lhs, FollowerFlags rhs ) {
	lhs = (FollowerFlags)( static_cast<T>( lhs ) | static_cast<T>( rhs ) );
	return lhs;
}

enum class FollowerGapSide : int {
	NoGap,
	Start,
	//	Middle,
	End,
	NotVisible
};

struct FollowerGapData {
	FollowerGapData( FollowerGapSide s, float i = 0.0f ) : side( s ), inset( i ) {}
	FollowerGapSide	side;
	float			inset;
};

enum UiSwipeDirection {
	UI_SWIPE_DOWN,
	UI_SWIPE_UP,
	UI_SWIPE_LEFT,
	UI_SWIPE_RIGHT,
};

enum TouchResult {
	TOUCH_RESULT_HIT,
	TOUCH_RESULT_NEAR_MISS,
	TOUCH_RESULT_NOT_HIT,
};

enum class YGestureInvert {
	Yes,
	No
};

enum class SwipeDirection {
	INVALID = 0,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

enum class MouseButtonStatusValues {
	UP,
	DOWN,
	MOVING,
    UNKNOWN
};

enum class MouseCursorType {
	ARROW,
	IBEAM,
	CROSSHAIR,
	HAND,
	HRESIZE,
	VRESIZE
};

using NodeType = uint64_t;
[[maybe_unused]] static const uint64_t NodeTypeGeneric = 1;
[[maybe_unused]] static const uint64_t NodeTypeLocator = 1 << 23;
[[maybe_unused]] static const uint64_t NodeTypeAll = 0xffffffffffffffff;

namespace InitializeWindowFlags {
	[[maybe_unused]] const static uint64_t Normal = 0;
	[[maybe_unused]] const static uint64_t FullScreen = 1 << 1;
	[[maybe_unused]] const static uint64_t Minimize = 1 << 2;
	[[maybe_unused]] const static uint64_t Maximize = 1 << 3;
	[[maybe_unused]] const static uint64_t HalfSize = 1 << 4;
	[[maybe_unused]] const static uint64_t ThreeQuarter = 1 << 5;
}

using InitializeWindowFlagsT = uint64_t;

[[maybe_unused]] static const float SINGLE_TAP_TIME_LIMIT = 0.33f;
[[maybe_unused]] static const float DOUBLE_TAP_TIME_LIMIT = 0.5f;
[[maybe_unused]] static const float TAP_AREA = 0.05f;
[[maybe_unused]] static const float MAX_SWIPE_TIME_LIMIT = 0.3f;
[[maybe_unused]] static const float LONG_TAP_TIME_LIMIT = 0.5f;
static const std::string defaultFontName = "Amaranth-Bold";

[[maybe_unused]] const static uint64_t SHADOW_MAGIC_TAG = 50420;

using ScreenShotContainer = std::vector<unsigned char>;
using ScreenShotContainerPtr = std::shared_ptr<ScreenShotContainer>;

namespace Name {
	const static std::string Alpha	  = "Alpha	 ";
	const static std::string Bravo	  = "Bravo	 ";
	const static std::string Charlie  = "Charlie ";
	const static std::string Delta	  = "Delta	 ";
	const static std::string Echo	  = "Echo	 ";
	const static std::string Foxtrot  = "Foxtrot ";
	const static std::string Golf	  = "Golf	 ";
	const static std::string Hotel	  = "Hotel	 ";
	const static std::string India	  = "India	 ";
	const static std::string Juliett  = "Juliett ";
	const static std::string Kilo	  = "Kilo	 ";
	const static std::string Lima	  = "Lima	 ";
	const static std::string Mike	  = "Mike	 ";
	const static std::string November = "November";
	const static std::string Oscar	  = "Oscar	 ";
	const static std::string Papa	  = "Papa	 ";
	const static std::string Quebec   = "Quebec  ";
	const static std::string Romeo	  = "Romeo	 ";
	const static std::string Sierra   = "Sierra  ";
	const static std::string Tango	  = "Tango	 ";
	const static std::string Uniform  = "Uniform ";
	const static std::string Victor   = "Victor  ";
	const static std::string Whiskey  = "Whiskey ";
	const static std::string X_Ray	  = "X_Ray	 ";
	const static std::string Yankee   = "Yankee  ";
	const static std::string Zulu	  = "Zulu	 ";
}

namespace ResourceGroup {
	const static std::string AppData   = "appdata";
	const static std::string Material  = "material";
	const static std::string Color     = "color";
	const static std::string VData     = "vdata";
	const static std::string Geom      = "geom";
    const static std::string Image     = "image";
    const static std::string Light     = "light";
    const static std::string Profile   = "profile";
    const static std::string Font      = "font";
	const static std::string UI        = "ui";
	const static std::string CameraRig = "camera_rig";
}

namespace ResourceCatalog {
    const static std::string Key       = "catalog";
}

enum class HttpUrlEncode {
	Yes,
	No
};

namespace Http { struct Result; }
using HttpResponeParams = const Http::Result&;
using ResponseCallbackFunc = std::function<void(HttpResponeParams)>;
using HttpResourceCBSign = const std::string&;
using HttpResourceCB = std::function<void( HttpResourceCBSign)>;
using CHttpResourceCB = const std::function<void( HttpResourceCBSign)>;

// Dependencies

using ddContainer = std::vector<std::string>;

enum class DependencyStatus {
	Loading = 0,
	LoadedSuccessfully,
	LoadedSuccessfully204,
	LoadingFailed,
	CallbackSuccessfullyExecuted,
	CallbackFailedOnExecution,
	Complete,
	CompleteWithErrors
};

#define RESOURCE_CTORS(N) \
    public: \
    explicit N( uint8_p&& data ) { \
        bufferDecode( data.first.get(), data.second ); \
    } \
    explicit N( const SerializableContainer& _data ) { \
        bufferDecode( _data.data(), _data.size() ); \
    } \
    explicit N( const unsigned char* _buffer, size_t _length ) { \
        bufferDecode( _buffer, _length ); \
    } \

#define RESOURCE_CTORS_ONLY(N) \
    RESOURCE_CTORS(N) \
    private: \
    void bufferDecode( const unsigned char* rawData, size_t length ); \
    public:

using ResourceRef = std::string;
using CResourceRef = const std::string&;
using ResourceDependencyDict    = std::unordered_map<std::string, std::vector<ResourceRef>>;

template<typename T, typename ... Args>
T perfectForward( Args&& ... args ) {
    return T(std::forward<Args>(args)...);
}
