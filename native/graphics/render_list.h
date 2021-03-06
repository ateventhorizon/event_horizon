//
//  render_list.h
//  SixthView
//
//  Created byDado on 11/01/1613.
//  Copyright (c) 2013Dado. All rights reserved.
//

#pragma once

#include <map>
#include <ostream>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include "graphic_constants.h"
#include "vertex_processing.h"
#include "skybox.h"
#include <core/camera_utils.hpp>

class Framebuffer;
class CommandBuffer;
class CameraRig;
class ShaderManager;
class Renderer;
class CommandBufferList;
class ShadowMapManager;
class Skybox;
class CubeEnvironmentMap;
class PrefilterSpecularMap;
class PrefilterBRDF;
class RLTarget;
class Camera;
struct FrameBufferTextureValues;

using MVList = std::map<std::string, std::shared_ptr<VPList>>;
using CubeMapRenderFunctionParams = const Camera*;
using CubeMapRenderFunction = std::function<void(CubeMapRenderFunctionParams)>;

//#define USE_UIBLITBUFFER 1

enum CommandBufferFlags {
    CBF_None = 0,
    CBF_DoNotSort = 1,
};

enum class CommandBufferCommandName {
    nop,
    depthWriteTrue,
    depthWriteFalse,
    depthTestFalse,
    depthTestTrue,
    depthTestLEqual,
    depthTestLess,

    cullModeNone,
    cullModeFront,
    cullModeBack,

    alphaBlendingTrue,
    alphaBlendingFalse,

    wireFrameModeTrue,
    wireFrameModeFalse,

    colorBufferBind,
    colorBufferBindAndClear,
    colorBufferBindAndClearDepthOnly,
    colorBufferClear,

    defaultFrameBufferBind,
    clearDefaultFramebuffer,

    setCameraUniforms,
    setGlobalTextures,

    shadowMapBufferBind,
    shadowMapClearDepthBufferZero,
    shadowMapClearDepthBufferOne,

    depthMapBufferBindAndClear,
    normalMapBufferBindAndClear,
    ssaoBufferBindAndClear,
#ifdef USE_UIBLITBUFFER
    uiBufferBindAndClear,
#endif
    ssaoRender,

    resolvePBR,
#ifdef USE_UIBLITBUFFER
    resolveUI,
#endif

    blitToScreen,
    blitPRB,
    blitPBRToScreen,

    takeScreenShot,

    targetVP,
    preFlush
};

std::string commandToNmeHumanReadable( CommandBufferCommandName cname );

class CommandBufferCommand {
public:
    CommandBufferCommand( CommandBufferCommandName name = CommandBufferCommandName::nop ) : name( name ) {}

    void issue( Renderer& rr, CommandBuffer *sourceCB ) const;

    CommandBufferCommandName name;
};

class CommandBufferEntry {
public:
    std::shared_ptr<VPList>         mVPList;
    std::shared_ptr<RenderMaterial> mMaterial;
    std::shared_ptr<Matrix4f>       mModelMatrix;
    Program*                        mProgram = nullptr;
};

enum class CommandBufferFrameBufferType {
    sourceColor,
    shadowMap,
    depthMap,
    normalMap,
    ssaoMap,
#ifdef USE_UIBLITBUFFER
    uiMap,
    uiMapResolve,
#endif
    finalResolve,
    finalBlit,
    blurVertical,
    blurHorizontal,
    screen
};

enum class CommandBufferEntryCommandType {
    Comamnd,
    VP
};

class CommandBufferEntryCommand {
public:
    CommandBufferEntryCommand( CommandBufferCommand _command );
    CommandBufferEntryCommand( CommandBufferEntry _vp );

    // Returns the number of drawcalls, 0 if it's just setting state
    size_t run( Renderer& rr, CommandBuffer* cb ) const;

    CommandBufferEntryCommandType Type() const;
    void Type( CommandBufferEntryCommandType type );

    std::string entryName() const;

    std::shared_ptr<CommandBufferEntry>& entry();

private:
    CommandBufferEntryCommandType mType;
    CommandBufferCommand mCommand;
    std::shared_ptr<CommandBufferEntry> mVP;
};

class CommandBuffer {
public:
    CommandBuffer() = default;
    CommandBuffer( CommandBufferFlags flags ) : flags( flags ) {}
    void push( const CommandBufferEntry& entry );
    void push( const CommandBufferCommand& entry );
    void clear();
    size_t render( Renderer& rr );
    void sort();
    bool findEntry( const std::string& _key, std::weak_ptr<CommandBufferEntry>& _wp );
    void                         postBlit();

    std::shared_ptr<Framebuffer> fb( CommandBufferFrameBufferType fbt );
    Rect2f                       destViewport();
    Rect2f                       sourceViewport();

    std::shared_ptr<RLTarget>&   Target() {
        return mTarget;
    }
private:
    std::vector<CommandBufferEntryCommand> mCommandList;

public:
    // Stack
    std::shared_ptr<RLTarget> mTarget;
    std::unique_ptr<char[]> UBOCameraBuffer;
    std::unique_ptr<FrameBufferTextureValues> frameBufferTextureValues;
    CommandBufferFlags flags = CommandBufferFlags::CBF_None;
};

class CommandBufferNewEntry {
public:
    std::weak_ptr<VPList> mVPList;
    std::shared_ptr<cpuVBIB> mVBIB;
//    std::string mName;
};

class CommandBufferList {
public:
    explicit CommandBufferList( Renderer& rr ) : rr( rr ) {}

    void start();
    void end();

    void startTarget( std::shared_ptr<Framebuffer> _fbt, Renderer& _rr );

    void pushVP( std::shared_ptr<VPList> _vp,
                 std::shared_ptr<RenderMaterial> _mat = nullptr,
                 std::shared_ptr<Matrix4f> _modelMatrix = nullptr,
                 Program* _forceProgram = nullptr,
                 float alphaDrawThreshold = 0.0f );
    void pushCommand( const CommandBufferCommand& _cmd );

    void startList( std::shared_ptr<RLTarget> _target, CommandBufferFlags flags = CommandBufferFlags::CBF_None );
    size_t render( int eye );
    void setCameraUniforms( std::shared_ptr<Camera> c0 );
    void setFramebufferTexture( const FrameBufferTextureValues& values );
    void getCommandBufferEntry( const std::string& _key, std::weak_ptr<CommandBufferEntry>& wp );

private:
    Renderer& rr;
    CommandBuffer* mCurrent = nullptr;
    std::vector<CommandBuffer> mCommandBuffers;
};

struct CommandBufferListVector {
    CommandBufferFlags flags = CommandBufferFlags::CBF_None;
    VPListContainer mVList;
    VPListContainer mVListTransparent;

    template <typename F, typename ...Args>
    void foreach( F func, Args ...args ) {
        func( mVList, std::forward<Args>(args)... );
        func( mVListTransparent, std::forward<Args>(args)... );
    }
};

struct VSGUIDData {
    Color4f color2d;
};

using GUIDT = std::tuple<std::string, VSGUIDData>;

template<typename T>
struct GUIDMapInput {
    uint64_t Hash() const { return as->hash; }

    std::shared_ptr<T> as;
    Color4f color2d;
};

class VPSGUID {
public:
    static VPSGUID& getInstance() {
        static VPSGUID instance; // Guaranteed to be destroyed.
        return instance;// Instantiated on first use.
    }

private:
    VPSGUID() = default;
    VPSGUID( VPSGUID const& ) = delete;
    void operator=( VPSGUID const& ) = delete;
public:

    template<typename T>
    std::string add( const GUIDMapInput<T>& _data ) {

        mHashMapCount[_data.Hash()].push_back( { _data.color2d } );

        return GUID( _data.Hash(), mHashMapCount[_data.Hash()].size() );
    }

    template<typename T>
    std::vector<GUIDT> getNames( std::shared_ptr<T> _data ) {
        std::vector<GUIDT> ret;
        size_t size = mHashMapCount[_data->hash].size();

        for ( uint32_t q = 0; q < size; q++ ) {
            uint64_t k1 = _data->hash;
            uint64_t k2 = q+1;
            ret.push_back( { GUID( k1, k2 ), mHashMapCount[_data->hash][q] } );
        }

        return ret;
    }

private:
    std::string GUID( uint64_t k1, uint32_t k2 ) {
        uint64_t z = ((k1+k2)*(k1+k2+1)/2) + k2;
//        uint32_t ci = (( static_cast<uint32_t >(_col.x()*255.0f) & 0xff) << 16) +
//        (( static_cast<uint32_t >(_col.y()*255.0f) & 0xff) << 8) +
//        ( static_cast<uint32_t >(_col.z()*255.0f) & 0xff);
        return prefix + std::to_string( z );
    }

private:
    std::unordered_map<uint64_t, std::vector<VSGUIDData>> mHashMapCount;
    const std::string prefix = "VPG2D";
};

#define VPG VPSGUID::getInstance()
#define VPGADD( X ) VPG.add<X>( { Data(), color2d } )

