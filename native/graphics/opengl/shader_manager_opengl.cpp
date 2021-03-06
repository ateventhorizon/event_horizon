#include <regex>

#include "shader_manager_opengl.h"
#include "shaders.hpp"
#include "../shader_list.h"
#include "core/zlib_util.h"
#include "core/tar_util.h"
#include "core/http/basen.hpp"

ShaderManager::ShaderManager() {

//    gShaderInjection["loading_screen.fsh"] = bn::encode_b64(loadingScreen);
    for ( const auto& [k,v] : gShaderInjection ) {
        shaderSourcesMap.insert( {k, v } );
    }

    createCCInjectionMap();

    allocateProgram( ShaderProgramDesc{ S::LOADING_SCREEN }.vsh( "vertex_shader_blitcopy" ).fsh( "loading_screen" ) );
    allocateProgram( ShaderProgramDesc{ S::WIREFRAME } );
    allocateProgram( ShaderProgramDesc{ S::SHADOW_MAP } );
    allocateProgram( ShaderProgramDesc{ S::NORMAL_MAP } );
    allocateProgram( ShaderProgramDesc{ S::DEPTH_MAP }.vsh( "vertex_shader_3d_c" ).fsh( "depthmap" ) );
    allocateProgram( ShaderProgramDesc{ S::SKYBOX } );
    allocateProgram( ShaderProgramDesc{ S::FONT_2D }.vsh( "vertex_shader_2d_font" ).fsh( "plain_font" ));
    allocateProgram( ShaderProgramDesc{ S::FONT }.vsh( "vertex_shader_3d_font" ).fsh( "plain_font" ));
    allocateProgram( ShaderProgramDesc{ S::BLUR_HORIZONTAL }.vsh( "vertex_shader_blitcopy" ).fsh( "plain_blur_horizontal" ));
    allocateProgram( ShaderProgramDesc{ S::BLUR_VERTICAL }.vsh( "vertex_shader_blitcopy" ).fsh( "plain_blur_vertical" ));
    allocateProgram( ShaderProgramDesc{ S::FINAL_COMBINE }.vsh( "vertex_shader_blitcopy_viewspace" ).fsh( "plain_final_combine" ));
    allocateProgram( ShaderProgramDesc{ S::SSAO }.vsh( "vertex_shader_blitcopy_viewspace" ).fsh( "ssao" ));
    allocateProgram( ShaderProgramDesc{ S::COLOR_2D }.vsh( "vertex_shader_2d_c" ).fsh( "fragment_shader_color" ));
    allocateProgram( ShaderProgramDesc{ S::COLOR_3D }.vsh( "vertex_shader_3d_c" ).fsh( "fragment_shader_color" ));
    allocateProgram( ShaderProgramDesc{ S::TEXTURE_2D }.vsh( "vertex_shader_2d_t" ).fsh( "fragment_shader_texture" ));
    allocateProgram( ShaderProgramDesc{ S::TEXTURE_3D }.vsh( "vertex_shader_3d_t" ).fsh( "fragment_shader_texture" ));
    allocateProgram( ShaderProgramDesc{ S::YUV_GREENSCREEN }.vsh( "vertex_shader_3d_t" ).fsh( "plain_yuv_to_rgb_greenscreen" ));
    allocateProgram( ShaderProgramDesc{ S::EQUIRECTANGULAR }.vsh( "equirectangular" ).fsh( "equirectangular" ));
    allocateProgram( ShaderProgramDesc{ S::PLAIN_CUBEMAP }.vsh( "plain_cubemap" ).fsh( "plain_cubemap" ));
    allocateProgram( ShaderProgramDesc{ S::SKYBOX_CUBEMAP }.vsh( "skybox_cubemap" ).fsh( "plain_cubemap" ));
    allocateProgram( ShaderProgramDesc{ S::CONVOLUTION }.vsh( "plain_cubemap" ).fsh( "irradiance_convolution" ));
    allocateProgram( ShaderProgramDesc{ S::IBL_SPECULAR }.vsh( "plain_cubemap" ).fsh( "ibl_specular_prefilter" ));
    allocateProgram( ShaderProgramDesc{ S::IBL_BRDF }.vsh( "vertex_shader_brdf" ).fsh( "plain_brdf" ));
    allocateProgram( ShaderProgramDesc{ S::SH }.vsh( "vertex_shader_3d_sh" ).fsh( "plain_sh" ));
    allocateProgram( ShaderProgramDesc{ S::SH_DIRECT_LIGHTING }.vsh( "vertex_shader_3d_sh" ).fsh( "plain_sh_direct_lighting" ));
    allocateProgram( ShaderProgramDesc{ S::SH_CITY_ATLAS }.vsh( "vertex_shader_osm" ).fsh( "fragment_shader_osm" ));
}

void ShaderManager::allocateProgram( const ShaderProgramDesc& _pd ) {
    programDescs.emplace_back( _pd );

    allocateShader( _pd.vertexShader, Shader::TYPE_VERTEX_SHADER );
    allocateShader( _pd.tessControlShader, Shader::TYPE_TESSELATION_CONTROL_SHADER );
    allocateShader( _pd.tessEvaluationShader, Shader::TYPE_TESSELATION_EVALUATION_SHADER );
    allocateShader( _pd.geometryShader, Shader::TYPE_GEOMETRY_SHADER );
    allocateShader( _pd.fragmentShader, Shader::TYPE_FRAGMENT_SHADER );
    allocateShader( _pd.computeShader, Shader::TYPE_COMPUTE_SHADER );
}


std::string ShaderManager::injectIncludes( std::string& sm ) {
    size_t includeFind = 0;
    while (( includeFind = sm.find( "#include" )) != std::string::npos ) {
        auto fi = sm.find_first_of( '"', includeFind );
        auto fe = sm.find_first_of( '"', fi + 1 );
        auto filename = sm.substr( fi + 1, fe - fi - 1 );
        filename = string_trim_upto( filename, "." );

        sm.erase( sm.begin() + includeFind, sm.begin() + fe + 1 );
        sm.insert( includeFind, bn::decode_b64(shaderSourcesMap[filename]) );
    }

    return sm;
}

std::string ShaderManager::parsePreprocessorMacro( std::string& sm ) {

    for ( const auto& [k,v] : ccShaderMap ) {
        size_t pfind = 0;
        while (( pfind = sm.find( k )) != std::string::npos ) {
            auto fe = pfind + k.length();
            sm.erase( sm.begin() + pfind, sm.begin() + fe );
            sm.insert( pfind, v );

        }
    }

    std::string defStartString = "#define_code ";
    std::string defEndString = "#end_code";

    size_t defineFind = 0;
    while (( defineFind = sm.find( defStartString )) != std::string::npos ) {
        std::istringstream iss( sm.substr( defineFind, sm.size() - defineFind ));
        std::string defineName;
        iss >> defineName;
        iss >> defineName;

        auto definenNameCut = sm.find( defineName );
        auto defineNameCurEnd = sm.find( defEndString );

        ASSERT( definenNameCut != std::string::npos );
        ASSERT( defineNameCurEnd != std::string::npos );

        definenNameCut += defineName.size();

        mDefineMap[defineName] = sm.substr( definenNameCut, defineNameCurEnd - definenNameCut );

        sm.erase( sm.begin() + defineFind, sm.begin() + defineNameCurEnd + defEndString.size() + 1 );
    }

    return sm;
}

std::string ShaderManager::injectPreprocessorMacro( std::string& sm ) {
    // Inject global macros

    size_t fi = 0;
    for ( auto& w : mDefineMap ) {
        std::istringstream smStream(sm);
        size_t numCharRead = 0;
        for (std::string line; std::getline(smStream, line); ) {
            numCharRead += line.size();
            if (( fi = line.find( w.first )) != std::string::npos ) {
                size_t fe = line.find( "//" );
                if ( fe == std::string::npos || fe > fi ) {
                    fi = sm.find( w.first, numCharRead );
                    fe = fi + w.first.size();
                    sm.erase( sm.begin() + fi, sm.begin() + fe + 1 );
                    sm.insert( fi, w.second );
                }
            }
        }
    }

    return sm;
}

std::string ShaderManager::openFileWithIncludeParsing( const std::string& filename ) {
    std::string sb = bn::decode_b64(shaderSourcesMap[filename]);
//    FileManager::writeLocalFile( filename, sb.c_str(), sb.length(), true );
    std::string includeResolved = injectIncludes( sb );
    parsePreprocessorMacro( includeResolved );
    auto finalShader = injectPreprocessorMacro( includeResolved );
//    FileManager::writeLocalFile( filename, finalShader.c_str(), finalShader.length(), true );
    return finalShader;
}

void ShaderManager::allocateShader( const std::string& id, Shader::Type stype ) {
    //LOGI("Adding vertex shader: %s", id);
    if ( id.empty() ) return;

    switch ( stype ) {
        case Shader::TYPE_VERTEX_SHADER:
            if ( mVertexShaders.find( id ) == mVertexShaders.end() ) {
                mVertexShaders[id] = std::make_shared<Shader>( stype, id);
            }
            break;
        case Shader::TYPE_TESSELATION_CONTROL_SHADER:
            if ( mTesselationControlShaders.find( id ) == mTesselationControlShaders.end() ) {
                mTesselationControlShaders[id] = std::make_shared<Shader>( stype, id);
            }
            break;
        case Shader::TYPE_TESSELATION_EVALUATION_SHADER:
            if ( mTesselationEvaluationShaders.find( id ) == mTesselationEvaluationShaders.end() ) {
                mTesselationEvaluationShaders[id] = std::make_shared<Shader>( stype, id);
            }
            break;
        case Shader::TYPE_GEOMETRY_SHADER:
            if ( mGeometryShaders.find( id ) == mGeometryShaders.end() ) {
                mGeometryShaders[id] = std::make_shared<Shader>( stype, id);
            }
            break;
        case Shader::TYPE_FRAGMENT_SHADER:
            if ( mFragmentShaders.find( id ) == mFragmentShaders.end() ) {
                mFragmentShaders[id] = std::make_shared<Shader>( stype, id);
            }
            break;
        case Shader::TYPE_COMPUTE_SHADER:
            if ( mComputeShaders.find( id ) == mComputeShaders.end() ) {
                mComputeShaders[id] = std::make_shared<Shader>( stype, id);
            }
            break;
        default:
            break;
    }
}

bool ShaderManager::addShader( const std::string& id, Shader::Type stype ) {
    //LOGI("Adding vertex shader: %s", id);
    if ( id.empty() ) return true;

    std::shared_ptr<Shader> sid = nullptr;

    switch ( stype ) {
        case Shader::TYPE_VERTEX_SHADER:
            mVertexShaders[id]->setSource( openFileWithIncludeParsing( id + ".vsh" ) );
            sid = mVertexShaders[id];
            break;
        case Shader::TYPE_TESSELATION_CONTROL_SHADER:
            mTesselationControlShaders[id]->setSource( openFileWithIncludeParsing( id + ".tch" ) );
            sid =  mTesselationControlShaders[id];
            break;
        case Shader::TYPE_TESSELATION_EVALUATION_SHADER:
            mTesselationEvaluationShaders[id]->setSource(  openFileWithIncludeParsing( id + ".teh" ) );
            sid = mTesselationEvaluationShaders[id];
            break;
        case Shader::TYPE_GEOMETRY_SHADER:
            mGeometryShaders[id]->setSource( openFileWithIncludeParsing( id + ".gsh" ) );
            sid = mGeometryShaders[id];
            break;
        case Shader::TYPE_FRAGMENT_SHADER:
            mFragmentShaders[id]->setSource( openFileWithIncludeParsing( id + ".fsh" ) );
            sid = mFragmentShaders[id];
            break;
        case Shader::TYPE_COMPUTE_SHADER:
            mComputeShaders[id]->setSource( openFileWithIncludeParsing( id + ".csh" ) );
            sid = mComputeShaders[id];
            break;
        default:
            return false;
    }

    return sid->compile();

}

std::shared_ptr<ProgramOpenGL> ShaderManager::initProgram( const ShaderProgramDesc& sb ) {
    if ( mPrograms.find( sb.name ) == mPrograms.end() ) {
        mPrograms[sb.name] = std::make_shared<ProgramOpenGL>( sb.name,
                                                              sb.vertexShader,
                                                              sb.tessControlShader,
                                                              sb.tessEvaluationShader,
                                                              sb.geometryShader,
                                                              sb.fragmentShader,
                                                              sb.computeShader );
    }

    return mPrograms[sb.name];
}

bool ShaderManager::loadProgram( const ShaderProgramDesc& sb ) {

    auto program = initProgram(sb);

    return program->createOrUpdate( vshForProgram(program),
                                    tchForProgram(program),
                                    tehForProgram(program),
                                    gshForProgram(program),
                                    fshForProgram(program),
                                    cshForProgram(program) );
}

bool ShaderManager::injectDefines( Shader::Type stype, const std::string& id, const std::string& _define, const std::string& _value ) {

    std::shared_ptr<Shader> sid = nullptr;
    switch ( stype ) {
        case Shader::TYPE_VERTEX_SHADER:
            sid = mVertexShaders[id];
            break;
        case Shader::TYPE_TESSELATION_CONTROL_SHADER:
            sid = mTesselationControlShaders[id];
            break;
        case Shader::TYPE_TESSELATION_EVALUATION_SHADER:
            sid = mTesselationEvaluationShaders[id];
            break;
        case Shader::TYPE_GEOMETRY_SHADER:
            sid = mGeometryShaders[id];
            break;
        case Shader::TYPE_FRAGMENT_SHADER:
            sid = mFragmentShaders[id];
            break;
        case Shader::TYPE_COMPUTE_SHADER:
            sid = mComputeShaders[id];
            break;
        default:
            return false;
            break;
    }
    sid->setPreprocessDefine( _define, _value);
    sid->compile();
    return true;
}

bool ShaderManager::injectShadersWithCode() {

    bool successfullChain = true;
    for ( const auto& shader : mVertexShaders ) {
        successfullChain = addShader( shader.first, Shader::TYPE_VERTEX_SHADER );
        if ( !successfullChain ) return false;
    }
    for ( const auto& shader : mTesselationControlShaders ) {
        successfullChain = addShader( shader.first, Shader::TYPE_TESSELATION_CONTROL_SHADER );
        if ( !successfullChain ) return false;
    }
    for ( const auto& shader : mTesselationEvaluationShaders ) {
        successfullChain = addShader( shader.first, Shader::TYPE_TESSELATION_EVALUATION_SHADER );
        if ( !successfullChain ) return false;
    }
    for ( const auto& shader : mGeometryShaders ) {
        successfullChain = addShader( shader.first, Shader::TYPE_GEOMETRY_SHADER );
        if ( !successfullChain ) return false;
    }
    for ( const auto& shader : mFragmentShaders ) {
        successfullChain = addShader( shader.first, Shader::TYPE_FRAGMENT_SHADER );
        if ( !successfullChain ) return false;
    }
    for ( const auto& shader : mComputeShaders ) {
        successfullChain = addShader( shader.first, Shader::TYPE_COMPUTE_SHADER );
        if ( !successfullChain ) return false;
    }
    return true;
}

bool ShaderManager::loadShaders( bool _performCompileOnly ) {

    bool compilationResult = injectShadersWithCode();

    if ( _performCompileOnly || !compilationResult ) return compilationResult;

    for ( const auto& pd : programDescs ) {
        compilationResult = loadProgram( pd );
        if ( !compilationResult ) return false;
    }

    mNumReloads++;
    return true;
}

void ShaderManager::touchProgram( const ShaderProgramDesc& sb ) {
    mProgramsToReload.emplace_back( sb );
}

bool ShaderManager::reloadDirtyPrograms() {
    if ( mProgramsToReload.empty() ) return false;

    for ( const auto& pd : mProgramsToReload ) {
        loadProgram( pd );
    }

    mProgramsToReload.clear();
    return true;
}

bool ShaderManager::injectDefine( const std::string& _shaderName, Shader::Type stype, const std::string& _id, const std::string& _define, const std::string& _value ) {

    bool compilationResult = injectDefines( stype, _id, _define, _value );

    if ( !compilationResult ) return false;

    for ( const auto& pd : programDescs ) {
        if ( pd.name.value == _shaderName ) {
            touchProgram(pd);
        }
    }

    return true;
}

std::shared_ptr<ProgramOpenGL> ShaderManager::P( const std::string& id ) const {
    auto it = mPrograms.find( id );

    if ( it != mPrograms.end()) {
        return it->second;
    }
    LOGE( "Could not find program: %s", id.c_str());
    ASSERT( false );
    return nullptr;
}

std::shared_ptr<Shader> ShaderManager::vshForProgram( std::shared_ptr<ProgramOpenGL> program ) {
    auto vsIt = mVertexShaders.find( program->getVertexShaderId());
    return vsIt != mVertexShaders.end() ? vsIt->second : nullptr;
}

std::shared_ptr<Shader> ShaderManager::tchForProgram( std::shared_ptr<ProgramOpenGL> program ) {
    auto vsIt = mTesselationControlShaders.find( program->getTesselationControlShaderId());
    return vsIt != mTesselationControlShaders.end() ? vsIt->second : nullptr;
}

std::shared_ptr<Shader> ShaderManager::tehForProgram( std::shared_ptr<ProgramOpenGL> program ) {
    auto vsIt = mTesselationEvaluationShaders.find( program->getTesselationEvaluationShaderId());
    return vsIt != mTesselationEvaluationShaders.end() ? vsIt->second : nullptr;
}

std::shared_ptr<Shader> ShaderManager::gshForProgram( std::shared_ptr<ProgramOpenGL> program ) {
    auto vsIt = mGeometryShaders.find( program->getGeometryShaderId());
    return vsIt != mGeometryShaders.end() ? vsIt->second : nullptr;
}

std::shared_ptr<Shader> ShaderManager::fshForProgram( std::shared_ptr<ProgramOpenGL> program ) {
    auto vsIt = mFragmentShaders.find( program->getFragmentShaderId());
    return vsIt != mFragmentShaders.end() ? vsIt->second : nullptr;
}

std::shared_ptr<Shader> ShaderManager::cshForProgram( std::shared_ptr<ProgramOpenGL> program ) {
    auto vsIt = mComputeShaders.find( program->getComputeShaderId());
    return vsIt != mComputeShaders.end() ? vsIt->second : nullptr;
}

int ShaderManager::getProgramCount() const {
    return static_cast<int>( mPrograms.size());
}

std::vector<GLuint> ShaderManager::ProgramsHandles() const {
    std::vector<GLuint> ret;

    for ( const auto& [k,v] : mPrograms ) {
        ret.emplace_back(v->handle());
    }

    return ret;
}



